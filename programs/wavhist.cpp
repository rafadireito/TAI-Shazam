#include <iostream>
#include <vector>
#include <sndfile.hh>
#include "wavhist.h"


using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[]) {

	if(argc < 3) {
		cerr << "Usage: wavhist <input file> <channel>" << endl;
		return 1;
	}

	SndfileHandle sndFile { argv[argc-2] };
    
	if(sndFile.error()) {
		cerr << "Error: invalid input file" << endl;
		return 1;
    }

	if((sndFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file is not in WAV format" << endl;
		return 1;
	}

	if((sndFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: file is not in PCM_16 format" << endl;
		return 1;
	}

	int channel { stoi(argv[argc-1]) };
	if(channel >= sndFile.channels()) {
		cerr << "Error: invalid channel requested" << endl;
		return 1;
	}
    

    

	size_t nFrames;
    vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    
    
	WAVHist hist { sndFile };
	while((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
		samples.resize(nFrames * sndFile.channels());
		hist.update(samples);
	}
    
	//hist.dump(channel);
    hist.dump_mono();
    hist.displayHistogram();
    
	return 0;
}

