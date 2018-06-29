#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    int blockSize = 1024;  // desired block length
    int fs        = 44100; // target sampling rate
    this->setupAudio(fs, blockSize);
}

//--------------------------------------------------------------
void ofApp::update(){
    if (this->getAudioRecordingState() == true) {
        if (this->isNewBlockAvailable() == true) {
            // do processing
            for (int kk=0; kk<this->audioSettings.bufferSize; kk++) {
                std::cout << this->recordingBlock[kk] << " ";
            }
            std::cout << std::endl;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'r') {
        if (this->getAudioRecordingState() == true) {
            this->stopRecording();
        }
        else {
            this->startRecording();
        }
    }
}

void ofApp::setupAudio(int targetSamplingFrequency, int blockSize) {
    // get all available devices
    vector<ofSoundDevice> deviceList = this->inStream.getDeviceList();
    if (deviceList.empty() == false) {
        for (int kk=0; kk<deviceList.size(); kk++) {
            // if device has input channels
            if (deviceList[kk].inputChannels > 0) {
                if (deviceList.size() < 3) {
                    ofLog(ofLogLevel::OF_LOG_NOTICE, "no external devices");
                }
                this->inputDevice = deviceList[kk];
                vector<unsigned int> fsList = this->inputDevice.sampleRates;
                this->audioSampleRate = -1;
                for (int nn=0; nn<fsList.size(); nn++) {
                    if (fsList[nn] == targetSamplingFrequency) {
                        this->audioSampleRate = fsList[nn];
                    }
                }
            }
        }
    }
    if (this->audioSampleRate == -1) {
        ofLog(ofLogLevel::OF_LOG_WARNING, "unsupported sampling rate");
        this->audioSampleRate = targetSamplingFrequency;
    }
    this->audioSettings.setInListener(this);
    this->audioSettings.setInDevice(this->inputDevice);
    this->audioSettings.numInputChannels  = 1; // we need 1 microphone
    this->audioSettings.numOutputChannels = 0; // we don't need a speaker
    this->audioSettings.numBuffers = 4; // we will have a total buffer of size bufferSize * numBuffers
    this->audioSettings.bufferSize = blockSize;
    this->audioSettings.sampleRate = this->audioSampleRate;
    
    ofLog(ofLogLevel::OF_LOG_NOTICE, "setup " + this->inputDevice.name + ":\n\tfs   " + ofToString(this->audioSettings.sampleRate) + "\n\tchan " + ofToString(this->audioSettings.numInputChannels) + "\n\tlen  " + ofToString(this->audioSettings.bufferSize));

    this->recordingBlock = new float[this->audioSettings.bufferSize];
    this->inStream.setup(this->audioSettings);
    this->stopRecording();
}

void ofApp::audioIn(ofSoundBuffer &input) {
    if (this->getAudioRecordingState()) {
        // 0 out channels, false filling buffer (if it is longer than available data)
        input.copyTo(this->recordingBlock, input.size(), input.getNumChannels(), 0, false);
        this->setNewBlockAvailable(true);
    }
}

void ofApp::startRecording() {
    this->inStream.start();
    this->setAudioRecordingState(true);
    ofLog(ofLogLevel::OF_LOG_NOTICE, "start recording");
}
void ofApp::stopRecording() {
    this->setAudioRecordingState(false);
    this->inStream.stop();
    ofLog(ofLogLevel::OF_LOG_NOTICE, "stop  recording");
}

void ofApp::setAudioRecordingState(bool state) {
    this->isAudioRecording = state;
}
bool ofApp::getAudioRecordingState() {
    return this->isAudioRecording;
}

// the following setter and getter uses a mutex because the audio recording runs in a separate thread
void ofApp::setNewBlockAvailable(bool state) {
    this->audioMutex.lock();
    this->newBlockAvailable = state;
    this->audioMutex.unlock();
}
bool ofApp::isNewBlockAvailable() {
    bool state;
    this->audioMutex.lock();
    state = this->newBlockAvailable;
    this->audioMutex.unlock();
    return state;
}
