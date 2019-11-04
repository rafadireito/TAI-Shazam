#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <math.h>


using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading/writing frames


/**
 Used to extract all samples from a sound file
 */
vector<short> allSampleFromFile( SndfileHandle sndFileIn){
    size_t nFrames;
    vector<short> samples(FRAMES_BUFFER_SIZE * sndFileIn.channels());
    vector<short> all_samples;
    
    while((nFrames = sndFileIn.readf(samples.data(), FRAMES_BUFFER_SIZE)))
        // copy samples (each frame might have 2 samples, if we are in stereo)
        for (int i = 0; i < nFrames * sndFileIn.channels(); i++)
            all_samples.push_back(samples.at(i) );
    
    return all_samples;
}


/**
 Writes all the samples to a file, taking into consideration the frame buffer size
 */
void writeDataToFile(vector<short> samples, SndfileHandle sndFileOut, int nChannels){
    vector<short> tmp_samples;
    
    for (int i = 0; i < samples.size(); i++){
        tmp_samples.push_back(samples.at(i));
        //(each frame might have 2 samples, if we are in stereo)
        if(i % (FRAMES_BUFFER_SIZE*nChannels) == 0 && i != 0){
            sndFileOut.writef(tmp_samples.data(), FRAMES_BUFFER_SIZE);
            tmp_samples.clear();
        }
        else if ( i == samples.size()-1)
            sndFileOut.writef(tmp_samples.data(), (i+1)%FRAMES_BUFFER_SIZE/nChannels);
    }
}


/**
 Returns the signal energy
 */
double signalEnergy(vector<short> samples){
    double totalEnergy = 0;
    for (int i = 0; i < samples.size(); i++)
        totalEnergy += pow(samples.at(i),2);
    
    return totalEnergy;
}


/**
Returns the noise energy
*/
double noiseEnergy(vector<short> originalSamples, vector<short> modifiedSamples){
    double noiseEnergy = 0;
    
    for (int i = 0; i < originalSamples.size(); i++)
        noiseEnergy += pow(modifiedSamples.at(i) - originalSamples.at(i),2);
    
    return noiseEnergy;
}


/**
Returns the signal to noise ration of the signal
*/
double signalNoiseRatio(double signalEnergy, double noiseEnergy){
    return 10 * log10(signalEnergy/noiseEnergy);
}




int main(int argc, char *argv[]) {

	if(argc < 3) {
		cerr << "Usage: wavcmp <original file> <modified file>" << endl;
		return 1;
	}

    /*
     Original File
     */
	SndfileHandle originaFileIn { argv[argc-2] };
	if(originaFileIn.error()) {
		cerr << "Error: invalid input file" << endl;
		return 1;
    }

	if((originaFileIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
		cerr << "Error: file is not in WAV format" << endl;
		return 1;
	}

	if((originaFileIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
		cerr << "Error: file is not in PCM_16 format" << endl;
		return 1;
	}

	cout << "Original file has:" << endl;
	cout << '\t' << originaFileIn.frames() << " frames" << endl;
	cout << '\t' << originaFileIn.samplerate() << " samples per second" << endl;
	cout << '\t' << originaFileIn.channels() << " channels" << endl << endl;
        
    /*
     Modified File
     */
    SndfileHandle modifiedFileIn { argv[argc-1] };
    if(modifiedFileIn.error()) {
        cerr << "Error: invalid input file" << endl;
        return 1;
    }

    if((modifiedFileIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        cerr << "Error: file is not in WAV format" << endl;
        return 1;
    }

    if((modifiedFileIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        cerr << "Error: file is not in PCM_16 format" << endl;
        return 1;
    }

    cout << "Modified file has:" << endl;
    cout << '\t' << modifiedFileIn.frames() << " frames" << endl;
    cout << '\t' << modifiedFileIn.samplerate() << " samples per second" << endl;
    cout << '\t' << modifiedFileIn.channels() << " channels" << endl << endl;
    
    
    
    // get all the original samples
    vector<short> originalSamples = allSampleFromFile(originaFileIn);
    
    // get all the modified samples
    vector<short> modifiedSamples = allSampleFromFile(modifiedFileIn);
    
    if(originalSamples.size() != modifiedSamples.size()){
        cerr << "Files have different sizes" << endl;
        return 1;
    }
    
    // get the singals energy
    double signalsEnergy = signalEnergy(originalSamples);
    cout << "Signals Energy: " << signalsEnergy  << endl;
    
    // get the singals noise energy
    double noisesEnergy = noiseEnergy(originalSamples, modifiedSamples);
    cout << "Noise Energy: " << noisesEnergy << endl;
    
    
    // get the singals noise ratio
    double signalsNoiseRation = signalNoiseRatio(signalsEnergy, noisesEnergy);
    cout << "Signal-To-Noise Ration: " << signalsNoiseRation << endl;
    
	

        
	return 0;
}


