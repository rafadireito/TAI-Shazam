#include "wavfind.h"
#include "wavcmp.h"


constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

Wavfind::Wavfind() = default;

Wavfind::~Wavfind() = default;

void Wavfind::compare(std::string codebookName, double result) {
    if (this -> signalNoiseRatio > result) {
        this -> signalNoiseRatio = result;
        this -> probableCodebook = std::move(codebookName);
    }
}

std::string Wavfind::guessMusic() {
    return this -> probableCodebook;
}

std::vector<std::string> Wavfind::open(const std::string& path = ".") {
    DIR*    dir;
    dirent* pdir;
    std::vector<std::string> files;

    dir = opendir(path.c_str());

    while ((pdir = readdir(dir))) {
        files.emplace_back(pdir->d_name);
    }

    return files;
}

/**
 Used to extract all samples from a sound file
 */
std::vector<short> Wavcmp::allSampleFromFile(SndfileHandle sndFileIn) {
    size_t nFrames;
    std::vector<short> samples(FRAMES_BUFFER_SIZE * sndFileIn.channels());
    std::vector<short> all_samples;

    while((nFrames = sndFileIn.readf(samples.data(), FRAMES_BUFFER_SIZE)))
        // copy samples (each frame might have 2 samples, if we are in stereo)
        for (size_t i = 0; i < nFrames * sndFileIn.channels(); i++)
            all_samples.push_back(samples.at(i));

    return all_samples;
}

/**
 Returns the signal energy
 */
double Wavcmp::signalEnergy(const std::vector<short>& samples){
    double totalEnergy = 0;

    for (short sample : samples)
        totalEnergy += pow(sample,2);

    return totalEnergy;
}


/**
Returns the noise energy
*/
double Wavcmp::noiseEnergy(std::vector<short> originalSamples, std::vector<short> modifiedSamples){
    double noiseEnergy = 0;

    for (size_t i = 0; i < originalSamples.size(); i++)
        noiseEnergy += pow(modifiedSamples.at(i) - originalSamples.at(i),2);

    return noiseEnergy;
}


/**
Returns the signal to noise ration of the signal
*/
double Wavcmp::signalNoiseRatio(double signalEnergy, double noiseEnergy){
    return 10 * log10(signalEnergy/noiseEnergy);
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        std::cerr << "Usage: wavfind <directory with codebooks> <audio sample file>" << std::endl;
        return 1;
    }

    size_t blockSize = 5000;
    float overlappingFactor = 0.5;
    Wavfind wf;
    Wavcmp wc;

    std::vector<std::string> files = wf.open(argv[argc-2]);
    files.erase(files.begin(), files.begin()+2);

    SndfileHandle sampleFile { argv[argc-1] };

    if(sampleFile.error()) {
        std::cerr << "Error: invalid input file" << std::endl;
        return 1;
    }

    if((sampleFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        std::cerr << "Error: file is not in WAV format" << std::endl;
        return 1;
    }

    if((sampleFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        std::cerr << "Error: file is not in PCM_16 format" << std::endl;
        return 1;
    }

    for (const auto & file : files) {
        double result = 0;
        double numBlocks = 0;
        size_t readBlockSize = 0;
        std::string line;
        std::ifstream codebook (argv[argc-2] + file);
        sampleFile.seek(0, SEEK_SET);

        if (codebook.is_open()) {
            while (getline(codebook, line)) {
                std::vector<short> codebookSample (line.begin(), line.end());
                std::vector<short> block(blockSize * sampleFile.channels());

                while((readBlockSize = sampleFile.readf(block.data(), blockSize))) {
                    if(readBlockSize == blockSize) {
                        numBlocks += 1;
                        result += wc.signalNoiseRatio(wc.signalEnergy(block), wc.noiseEnergy(block, codebookSample));
                        sampleFile.seek( -overlappingFactor, SEEK_CUR);
                    }
                }
            }

            codebook.close();
            wf.compare(file, result / numBlocks);
        }
    }

    std::cout << "I think this is your song: " << wf.guessMusic() << std::endl;
    return 0;
}
