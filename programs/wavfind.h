#include <iostream>
#include <sndfile.hh>
#include <string>
#include <cstdio>
#include <cstring>
#include <utility>
#include <ctime>
#include <vector>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <numeric>

/**
 * Class responsible for finding the most probable music to which a audio sample belongs,
 * given a collection of codebooks.
 */
class Wavfind {
private:
    std::string probableCodebook = "None";
    double signalNoiseRatio = -std::numeric_limits<double>::infinity();
public:
    ~Wavfind();

    Wavfind();

    void compare(std::string codebook, double result);

    static std::vector<std::vector<short>> getCodebookBlocks(std::ifstream & codebookPath, size_t blockSize);

    static std::vector<std::vector<short>> getSampleBlocks(SndfileHandle sampleFile, size_t blockSize);

    std::string guessMusic();

    static std::vector<std::string> open(const std::string& path);
};
