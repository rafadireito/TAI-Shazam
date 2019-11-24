#include <sndfile.hh>
#include <vector>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "kMeans.h"

class WAVCb {

    public:

        std::vector<std::vector<short>> getCodebook(SndfileHandle& wavFile, size_t blockSize, size_t overlappingFactor, size_t codebookSize, int maxIterations, int nThreads){
            std::vector<std::vector<short>> blocks;

            size_t readBlockSize;

            std::vector<short> block(blockSize * wavFile.channels());

            /*
              Lê o ficheiro e depois retrocede o valor do overlapping
            */
            while((readBlockSize = wavFile.readf(block.data(), blockSize))){
                
                if(readBlockSize == blockSize){
                
                    blocks.push_back(block);
                    wavFile.seek( -overlappingFactor, SEEK_CUR);                               
                
                }
            
            }
            if(blockSize < codebookSize){
                std::cerr << "Error: codebook size larger than extracted blocks." << std::endl;
                std::cerr << "Use a smaller codebookSize or extract more blocks ( > overlaping or < block size)" << std::endl;
                return std::vector<std::vector<short>>();                
            }
            /*
              Executa o Clustering
            */
            KMeans km(codebookSize, maxIterations);

            return km.getClusters(blocks, nThreads);
        }
 };
