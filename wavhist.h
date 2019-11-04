#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>

class WAVHist {
  private:
	std::vector<std::map<short, size_t>> counts;
    std::map<double, size_t> counts_mono;

  public:
	WAVHist(const SndfileHandle& sfh) {
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

	void dump(const size_t channel) const {
		for(auto [value, counter] : counts[channel])
            std::cout << value << '\t' << counter << '\n';
	}
    
    
    void dump_mono() const {
        // if the file is already in mono, print its histogram
        if(counts.size()== 1)
            dump(0);
        // if file is stereo
        else
            for(auto [value, counter] : counts_mono)
                std::cout << value << '\t' << counter << '\n';
    }
};

#endif

