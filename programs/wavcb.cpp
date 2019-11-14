#include <iostream>
#include <vector>
#include <sndfile.hh>
#include "wavcb.h"


int main(int argc, char *argv[]) {

    if(argc < 2) {
        std::cerr << "Usage: wavcp <input file>" << std::endl;
        return 1; 
    }  

    SndfileHandle sndFileIn { argv[argc-1] }; 
    if(sndFileIn.error()) {
        std::cerr << "Error: invalid input file" << std::endl;
        return 1; 
    } 

    if((sndFileIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        std::cerr << "Error: file is not in WAV format" << std::endl;
        return 1; 
    }
    size_t windowSize = 500; 
    
    WAVCb test;

    std::vector<std::vector<short>> codebook = test.getCodebook(sndFileIn, windowSize, windowSize/2, 5);

    return 0;
}




