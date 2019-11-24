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

/*

*/
class Wavfind {
private:
    std::string probableCodebook;
    double signalNoiseRatio = -std::numeric_limits<double>::infinity();
public:
    ~Wavfind();

    Wavfind();

    void compare(std::string codebook, double result);

    std::string guessMusic();

    static std::vector<std::string> open(const std::string& path);
};
