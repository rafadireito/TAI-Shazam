

#include <sndfile.hh>
#include <vector>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "kMeans.h"

class WAVCb {

    public:

        std::vector<std::vector<short>> getCodebook(SndfileHandle& wavFile, size_t windowSize, size_t offset, size_t codebookSize){
            std::vector<std::vector<short>> samples;

            size_t nFrames;
            std::vector<short> sample(windowSize * wavFile.channels());
            while((nFrames = wavFile.readf(sample.data(), windowSize))){
                
                if(nFrames == windowSize){
                
                    samples.push_back(sample);
                    wavFile.seek( -offset, SEEK_CUR);                               
                
                }
            
            }

            KMeans km(codebookSize, 12);

            return km.getKMeansClustering(samples);
        }
 };
