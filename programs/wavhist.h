#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>
#include <fstream>
#include "gnuplot.h"

class WAVHist {
  private:
	std::vector<std::map<short, size_t>> counts;
    std::map<double, size_t> counts_mono;
    std::ofstream myfile;
    
    

  public:
	WAVHist(const SndfileHandle& sfh) {
        myfile.open ("results/hist_dump.tsv");
		counts.resize(sfh.channels());
	}

	void update(const std::vector<short>& samples) {
		size_t n { };
        double channels_sum = 0;
        for(auto s : samples){
            /*
             counts.size() will be 2, if the sound is stereo
             the samples are read one from each channel
             [ s0_channel1, s0_channel2, s1_channel1, s1_channel2, ...]
            */
            
            // for stereo and mono sound
			counts[n % counts.size()][s]++;
            
            // for stereo -> mono sound
            channels_sum += s;
            // if we read the samples from all channels...
            if(n % counts.size() == 1 ){
                counts_mono[ (short) (channels_sum/counts.size() + 0.5) ]++;
                channels_sum = 0;
            }
            n++;
        }
	}

	void dump(const size_t channel)  {
        for(auto [value, counter] : counts[channel]){
            //std::cout << value << '\t' << counter << '\n';
            myfile << value << '\t' << counter << '\n';
        }
        myfile.close();
	}
    
    
    void dump_mono() {
        // if the file is already in mono, print its histogram
        if(counts.size()== 1)
            dump(0);
        // if file is stereo
        else{
            for(auto [value, counter] : counts_mono){
                std::cout << value << '\t' << counter << '\n';
                myfile << value << '\t' << counter << '\n';
            }
            myfile.close();
        }
        
    }
    
    void displayHistogram(){
        GnuplotPipe gp;
        gp.sendLine("set style data histograms");
        gp.sendLine("set style fill solid");
        gp.sendLine("set ylabel 'Quantidade de Amostras'");
        gp.sendLine("set xlabel 'Amplitude da Amostra'");
        gp.sendLine("plot 'results/hist_dump.tsv' using 2:xtic(1)  title 'Histograma - Quantidade de Amostras por Amplitude'");
    }
};

#endif

