#include "wavfind.h"
#include "wavcmp.h"


constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

Wavfind::Wavfind() = default;

Wavfind::~Wavfind() = default;

/**
 * Function to compare the given result with the result of, so far, the most probable codebook.
 * @param codebookName is the name of the codebook which represents a music.
 * @param result is the signal-to-energy ratio used as factor of comparison.
 */
void Wavfind::compare(std::string codebookName, double result) {
    if (this -> signalNoiseRatio < result) {
        this -> signalNoiseRatio = result;
        this -> probableCodebook = std::move(codebookName);
    }
}

/**
 * Function to retrieve the music name with the highest probability.
 * @return the name of the most probable music.
 */
std::string Wavfind::guessMusic() {
    return this -> probableCodebook;
}

/**
 * Function to open a directory and retrieve all the files inside.
 * @param path is the location of the directory with the collection of codebooks.
 * @return all the files, that should be codebooks, inside of the directory.
 */
std::vector<std::string> Wavfind::open(const std::string& path = ".") {
    DIR*    dir;
    dirent* pdir;
    std::vector<std::string> files;

    if (path.back() != '/' and path != ".") {
        std::cerr << "Directory is missing the / at the end!" << std::endl;
        exit(EXIT_FAILURE);
    }

    dir = opendir(path.c_str());

    if (dir == nullptr) {
        std::cerr << "Directory doesn't exist!" << std::endl;
        exit(EXIT_FAILURE);
    }

    while ((pdir = readdir(dir)))
        files.emplace_back(pdir->d_name);

    return files;
}

 /**
  * Function to compute the signal energy of samples.
  * @param samples represent a set of values of a audio sample block.
  * @return the signal energy of the samples.
  */
double Wavcmp::signalEnergy(const std::vector<short>& samples){
    double totalEnergy = 0;

    for (short sample : samples)
        totalEnergy += pow(sample,2);

    return totalEnergy;
}

/**
 * Function to compute the noise energy between a audio sample block and a codebook block.
 * @param originalSamples represent a set of values of a audio sample block.
 * @param modifiedSamples represent a set of values of a codebook block.
 * @return the noise energy between the originalSamples and the modifiedSamples.
 */
double Wavcmp::noiseEnergy(std::vector<short> originalSamples, std::vector<short> modifiedSamples){
    double noiseEnergy = 0;

    for (size_t i = 0; i < originalSamples.size(); i++)
        noiseEnergy += pow(modifiedSamples.at(i) - originalSamples.at(i),2);

    return noiseEnergy;
}

/**
 * Function to compute the signal-to-noise ratio of a signal.
 * @param signalEnergy of a signal.
 * @param noiseEnergy of a signal.
 * @return the signal-to-noise ratio of a signal.
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
    Wavcmp wcmp;

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
        size_t readBlockSize;
        std::string line;
        std::ifstream codebook (argv[argc-2] + file);

        if (codebook.is_open()) {
            while (getline(codebook, line)) {
                std::vector<short> codebookBlock (line.begin(), line.end());
                std::vector<short> block(blockSize * sampleFile.channels());
                sampleFile.seek(0, SEEK_SET);

                while((readBlockSize = sampleFile.readf(block.data(), blockSize))) {
                    if(readBlockSize == blockSize) {
                        result = wcmp.signalNoiseRatio(wcmp.signalEnergy(block),
                                wcmp.noiseEnergy(block, codebookBlock));
                        wf.compare(file, result);
                        sampleFile.seek(-overlappingFactor, SEEK_CUR);
                    }
                }
            }

            codebook.close();
        }
    }

    std::cout << "I think this is your song: " << wf.guessMusic() << std::endl;
    return 0;
}
