#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <cstring>
#include "wavcb.h"
#include <filesystem>
#include <fstream>

using namespace std;


void fileWriter(string name, std::vector<std::vector<short>> codebook){
    ofstream fp;
    fp.open(name.substr(0, name.length() -3) + "codebook");
    for(size_t i = 0; i < codebook.size(); i++){
        for(size_t j = 0; j < codebook[i].size(); j++){
            fp << codebook[i][j] << " ";
        }
        fp << "\n";

    }


    fp.close();
}

bool is_number(std::string s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int main(int argc, char *argv[]) {

    if(argc < 3) {
        std::cerr << "Usage: wavcp [options]" << std::endl;
        std::cerr << "-f 'filename'" << std::endl;
        std::cerr << "-d 'directory" << std::endl;
        std::cerr << "-b block size" << std::endl;
        std::cerr << "-o overlaping factor" << std::endl;
        std::cerr << "-c codebook size" << std::endl;
        std::cerr << "-i max iterations in Kmeans" << std::endl;
        std::cerr << "-t number of threads" << std::endl;
        std::cerr << "Use at least -f or -d options" << std::endl;    
        return 1; 
    }

    string directory = "";
    string file = "";

    size_t blockSize = 5000;
    float overlappingFactor = 0.5;
    size_t codebookSize = 150;
    int iterations = 100;
    int nThreads = 4;

    for(int i = 1; i < argc; i++){
        
        if(strcmp("-f", argv[i]) == 0){
            file = argv[i+1];
        }

        else if(strcmp("-d", argv[i]) == 0 ){
            directory = argv[i+1];
        }

        else if(strcmp("-b", argv[i]) == 0 ){
            blockSize = std::atoi( argv[i+1] );
            if(blockSize <= 0){
                std::cerr << "Error: invalid block size" << std::endl;
                return 1;
            }
            
        }

        else if(strcmp("-o", argv[i]) == 0 ){
            overlappingFactor = std::atof( argv[i+1] );
            if(overlappingFactor == 0){
                std::cerr << "Error: invalid overlaping factor" << std::endl;
                return 1;
            }
            
        }

        else if(strcmp("-c", argv[i]) == 0 ){
            codebookSize = std::atoi( argv[i+1] );
            if(codebookSize <= 0){
                std::cerr << "Error: invalid codebook size" << std::endl;
                return 1;
            }
        }

        else if(strcmp("-i", argv[i]) == 0 ){
            iterations = std::atoi( argv[i+1] );
            if(iterations <= 0){
                std::cerr << "Error: invalid number of iterations" << std::endl;
                return 1;
            }
        }
        else if(strcmp("-t", argv[i]) == 0 ){
            nThreads = std::atoi( argv[i+1] );
            if(nThreads <= 0){
                std::cerr << "Error: invalid number of threads" << std::endl;
                return 1;
            }
        }
        else{
            std::cerr << "Error: Invalid Use of Arguments" << std::endl;
            return 1;
        }
        i += 1;
    } 

    if( file.compare("") != 0 && directory.compare("") == 0){

        SndfileHandle sndFileIn { file }; 
        if(sndFileIn.error()) {
            std::cerr << "Error: invalid input file" << std::endl;
            return 1; 
        }
        if((sndFileIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
            std::cerr << "Error: file is not in WAV format" << std::endl;
            return 1; 
        }

        WAVCb codebookGenerator;

        std::vector<std::vector<short>> codebook = codebookGenerator.getCodebook(sndFileIn, 
                blockSize, 
                blockSize*overlappingFactor, 
                codebookSize, 
                iterations,
                nThreads);

        if(codebook.size() == 0){
            return 1;
        }

        fileWriter(file, codebook);
    }
    else if( file.compare("") == 0 && directory.compare("") != 0){
        
        try{
            for (const auto & entry : std::filesystem::directory_iterator(directory)){

                std::string file = entry.path().string();

                if(file.compare(file.length()-4, 4, ".wav") == 0){
                    
                    SndfileHandle sndFileIn { file }; 
                    if(sndFileIn.error()) {
                        std::cerr << "Error: invalid input file" << std::endl;
                        return 1; 
                    }
                    if((sndFileIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
                        std::cerr << "Error: file is not in WAV format" << std::endl;
                        return 1; 
                    }

                    WAVCb codebookGenerator;

                    std::vector<std::vector<short>> codebook = codebookGenerator.getCodebook(sndFileIn, 
                            blockSize, 
                            blockSize*overlappingFactor, 
                            codebookSize, 
                            iterations,
                            nThreads);
                    if(codebook.size() == 0){
                        return 1;
                    }


                    fileWriter(entry.path().filename(), codebook);
                }
            }
        }
        catch(std::filesystem::filesystem_error & e){
            std::cerr << "Error: invalid directory" << std::endl;
            return 1;
        }
    }
    else{
        std::cerr << "Use only -f or -d not both." << std::endl;
    }

    return 0;
}




