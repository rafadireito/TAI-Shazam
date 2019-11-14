#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <math.h>


using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading/writing frames


/**
 Used to extract all samples from a sound file
 */
vector<short> allSampleFromFile( SndfileHandle sndFileIn){
    size_t nFrames;
    vector<short> samples(FRAMES_BUFFER_SIZE * sndFileIn.channels());
    vector<short> all_samples;
    
    while((nFrames = sndFileIn.readf(samples.data(), FRAMES_BUFFER_SIZE)))
        // copy samples (each frame might have 2 samples, if we are in stereo)
        for (int i = 0; i < nFrames * sndFileIn.channels(); i++)
            all_samples.push_back(samples.at(i) );
    
    return all_samples;
}


/**
 Used to get the interval of the quantizer and the reconstruction value for each interval.
 Will return a vector containing all the levels.
 A level can be described as a vector [min_x, max_x, reconstruction_x]
 */
vector<vector<int>> intervalsForQuantizer( vector<short> all_samples, size_t nLevels ){
    /*
      Compute the 'buscket size' -> (max-min)/nLevels
      */
     int max_sample_value = all_samples.at(0);
     int min_sample_value = all_samples.at(0);
     
     for (int i = 0; i < all_samples.size(); i++)
         if(all_samples.at(i) < min_sample_value)
             min_sample_value = all_samples.at(i);
         else if (all_samples.at(i) > max_sample_value)
              max_sample_value= all_samples.at(i);
     
     size_t bucketSize = ceil ((max_sample_value - min_sample_value) / nLevels);

    /*
     Compute the intervals -> an interval will be [min_x, max_x, reconstruction_x]
     */
     vector<vector<int>> intervals;
     int start = min_sample_value;
     int end;
        
     for(int i = 0; i < nLevels; i++){
         vector<int> interval;
         
         end = start + bucketSize;
         
         interval.push_back(start);
         interval.push_back(end);
         interval.push_back((int)((start+end)/2));
         
         intervals.push_back(interval);
         
         start += (bucketSize + 1);
     }
    return intervals;
}


/**
 Returns the quantized samples
 */
vector<short> quantizedSamples(vector<short> samples, vector<vector<int>> intervals){
    vector<short> samples_quantized;
    
    for (int sample_index = 0; sample_index < samples.size(); sample_index++){
        
        for (int interval_index = 0; interval_index < intervals.size(); interval_index++)
            if (samples.at(sample_index) >= intervals.at(interval_index).at(0) &&
                samples.at(sample_index) <= intervals.at(interval_index).at(1))
                samples_quantized.push_back(intervals.at(interval_index).at(2));
    }
    return samples_quantized;
}


/**
 Writes all the samples to a file, taking into consideration the frame buffer size
 */
void writeDataToFile(vector<short> samples, SndfileHandle sndFileOut, int nChannels){
    vector<short> tmp_samples;
    
    for (int i = 0; i < samples.size(); i++){
        tmp_samples.push_back(samples.at(i));
        //(each frame might have 2 samples, if we are in stereo)
        if(i % (FRAMES_BUFFER_SIZE*nChannels) == 0 && i != 0){
            sndFileOut.writef(tmp_samples.data(), FRAMES_BUFFER_SIZE);
            tmp_samples.clear();
        }
        else if ( i == samples.size()-1)
            sndFileOut.writef(tmp_samples.data(), (i+1)%FRAMES_BUFFER_SIZE/nChannels);
    }
}


int main(int argc, char *argv[]) {

	if(argc < 4) {
		cerr << "Usage: wavquant <input file> <output file> <byte resolution>" << endl;
		return 1;
	}

	SndfileHandle sndFileIn { argv[argc-3] };
	if(sndFileIn.error()) {
		cerr << "Error: invalid input file" << endl;
		return 1;
    }

	if((sndFileIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file is not in WAV format" << endl;
		return 1;
	}

	if((sndFileIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: file is not in PCM_16 format" << endl;
		return 1;
	}

	cout << "Input file has:" << endl;
	cout << '\t' << sndFileIn.frames() << " frames" << endl;
	cout << '\t' << sndFileIn.samplerate() << " samples per second" << endl;
	cout << '\t' << sndFileIn.channels() << " channels" << endl;

    
	SndfileHandle sndFileOut { argv[argc-2], SFM_WRITE, sndFileIn.format(),
	  sndFileIn.channels(), sndFileIn.samplerate() };
	if(sndFileOut.error()) {
		cerr << "Error: invalid output file" << endl;
		return 1;
    }
    
    int nBytes = { stoi(argv[argc-1]) };
    if(nBytes > 8 || nBytes <=0) {
        cerr << "Error: invalid byte resolution" << endl;
        return 1;
    }

        
    size_t nLevels = pow (2, nBytes);
    
    // get all the samples from sound file
    vector<short> all_samples = allSampleFromFile(sndFileIn);
    
    // get all the intervals for the quantizer
    vector<vector<int>> intervals = intervalsForQuantizer(all_samples, nLevels);
    
    // get the quantized samples
    vector<short> samplesQuantized = quantizedSamples(all_samples, intervals);
    
    // Write data to file
    writeDataToFile(samplesQuantized, sndFileOut, sndFileIn.channels());
        
	return 0;
}


