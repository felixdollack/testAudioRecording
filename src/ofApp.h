#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();

		void keyPressed(int key);
		
        // audio settings
        void setupAudio(int, int);
        void setNewBlockAvailable(bool);
        bool isNewBlockAvailable();
        void startRecording();
        void stopRecording();
        void setAudioRecordingState(bool);
        bool getAudioRecordingState();
        void audioIn(ofSoundBuffer &input); // audio in callback
        
        ofSoundDevice inputDevice; // audio device for recording
        ofSoundStreamSettings audioSettings;
        int audioSampleRate;
        ofSoundStream inStream; // stream to audio hardware
        float *recordingBlock;  // audio data buffer
        ofMutex audioMutex;     // mutex to get exclusive access to buffer
        bool newBlockAvailable; // flag if a new block is available for processing
        
        bool isAudioRecording;  // flag if hardware is running
};
