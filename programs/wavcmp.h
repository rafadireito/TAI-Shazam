#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <cmath>

/**
 * Class responsible for computing the signal-to-noise ratio of a certain audio file.
 */
class Wavcmp {
    public:
        static std::vector<short> allSampleFromFile(SndfileHandle sndFileIn);
        
        void writeDataToFile(std::vector<short> samples, SndfileHandle sndFileOut, int nChannels);
        
        static double signalEnergy(const std::vector<short>& samples);
        
        static double noiseEnergy(std::vector<short> originalSamples, std::vector<short> modifiedSamples);
        
        static double signalNoiseRatio(double signalEnergy, double noiseEnergy);
};

