//George Brandlin 
//2/22/18
#include<iostream>
#include<stdio.h>
#include<cmath>
#include"fonts.h"
#include<GL/glx.h>
#include<ctime>
#include<cstdlib>
#include<sys/stat.h>
#include</usr/include/AL/alut.h>
#include<math.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>




using namespace std;


int  bulletsound(){

   alutInit(0, NULL);
    if (alGetError() != AL_NO_ERROR) {
	printf("ERROR: alutInit()\n");
    }

    alGetError();
    float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, vec);
    alListenerf(AL_GAIN, 1.0f);




    ALuint alBuffer;
    alBuffer = alutCreateBufferFromFile("./test.wav");
    ALuint alSource;
    alGenSources(1, &alSource);
    alSourcei(alSource, AL_BUFFER, alBuffer);
    alSourcef(alSource, AL_GAIN, 1.0f);
    alSourcef(alSource, AL_PITCH, 1.0f);
    alSourcei(alSource, AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR) {
	printf("ERROR: setting source\n");

    }
    for (int i=0; i<1; i++) {
	alSourcePlay(alSource);
	usleep(25);
    }
    // alDeleteSources(1, &alSource);
    //alDeleteBuffers(1, &alBuffer);
    //ALCcontext *Context = alcGetCurrentContext();
    //ALCdevice *Device = alcGetContextsDevice(Context);
    //alcMakeContextCurrent(NULL);

    //alcDestroyContext(Context);

    //alcCloseDevice(Device);




    return(0);

}



