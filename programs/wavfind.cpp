#include "wavfind.h"
#include "wavcmp.h"

Wavfind::Wavfind() = default;

Wavfind::~Wavfind() = default;

/**
 * Function to compare the given result with the result of, so far, the most probable codebook.
 * @param codebookName is the name of the codebook which represents a music.
 * @param result is the signal-to-energy ratio used as factor of comparison.
 */
void Wavfind::compare(std::string codebookName, double result) {
    if (result > this -> signalNoiseRatio) {
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
        if (strncmp(pdir->d_name, ".", 1) != 0 and strncmp(pdir->d_name, "..", 2) != 0)
            files.emplace_back(pdir->d_name);

    return files;
}

/**
 * Function to retrieve all the blocks from the codebook file.
 * @param codebook is the file which contains the blocks to be compared with the sample blocks.
 * @param blockSize is the size of each block inside of the codebook file.
 * @return all the blocks inside the codebook file.
 */
std::vector<std::vector<short>> Wavfind::getCodebookBlocks(std::ifstream & codebook, size_t blockSize) {
    short val;
    std::string value;
    std::vector<short> codebookBlock;
    std::vector<std::vector<short>> codebookBlocks;

    if (codebook.is_open()) {
        while (getline(codebook, value, ' ')) {
            std::stringstream sstream(value);
            sstream >> val;
            codebookBlock.push_back(val);

            if (codebookBlock.size() == blockSize) {
                codebookBlocks.push_back(codebookBlock);
                codebookBlock.clear();
            }
        }

        codebook.close();
    }

    return codebookBlocks;
}

/**
 * Function to retrieve all the blocks from the audio sample file.
 * @param sampleFile is a sample of an audio file with the .wav extension.
 * @param blockSize is the size of each block inside of the sample file.
 * @return all the blocks inside the audio sample file.
 */
std::vector<std::vector<short>> Wavfind::getSampleBlocks(SndfileHandle sampleFile, size_t blockSize) {
    size_t readBlockSize;
    std::vector<std::vector<short>> blocks;
    std::vector<short> block(blockSize * sampleFile.channels());

    while((readBlockSize = sampleFile.readf(block.data(), blockSize)))
        if (readBlockSize == blockSize)
            blocks.push_back(block);

    return blocks;
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
    if(argc != 4) {
        std::cerr << "Usage: wavfind <directory with codebooks> <audio sample file> <blockSize>" << std::endl;
        return 1;
    }

    std::stringstream sstream(argv[argc-1]);
    size_t blockSize;
    sstream >> blockSize;
    Wavfind wf;
    Wavcmp wcmp;
    SndfileHandle sampleFile { argv[argc-2] };

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

    std::vector<std::vector<short>> sampleBlocks = wf.getSampleBlocks(sampleFile, blockSize);
    std::vector<std::string> files = wf.open(argv[argc-3]);

    for (const auto & file : files) {
        double result = 0.0;
        std::ifstream codebook (argv[argc-3] + file);
        std::vector<std::vector<short>> codebookBlocks = wf.getCodebookBlocks(codebook, blockSize);

        for (const auto & sampleBlock : sampleBlocks) {
            double min_error = -std::numeric_limits<double>::infinity();

            for (const auto & codebookBlock : codebookBlocks) {
                double error = wcmp.signalNoiseRatio(wcmp.signalEnergy(sampleBlock),
                        wcmp.noiseEnergy(codebookBlock, sampleBlock));

                if (error > min_error)
                    min_error = error;
            }

            result += min_error;
            codebook.clear();
            codebook.seekg(0, std::ios::beg);
        }

        wf.compare(file, result);
    }

    std::cout << "I think this is your song: " << wf.guessMusic() << std::endl;
    return 0;
}
