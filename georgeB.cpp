//George Brandlin 
//5/7/18
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
//#ifdef USE_SOUND
//#endif // USE_SOUND


void initSound(){

    alutInit(0, NULL);
    if (alGetError() != AL_NO_ERROR) {
	printf("ERROR: alutInit()\n");
    }

    alGetError();
    float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
    alListenerfv(AL_ORIENTATION, vec);
    alListenerf(AL_GAIN, 1.0f);

}


int  bulletsound(){

//#ifdef USE_SOUND
    initSound();



    ALuint bullBuff;
    bullBuff = alutCreateBufferFromFile("./laser.wav");
    ALuint bullSrc;
    alGenSources(1, &bullSrc);
    alSourcei(bullSrc, AL_BUFFER, bullBuff);
    alSourcef(bullSrc, AL_GAIN, 1.0f);
    alSourcef(bullSrc, AL_PITCH, 1.0f);
    alSourcei(bullSrc, AL_LOOPING, AL_FALSE);
    if (alGetError() != AL_NO_ERROR) {
	printf("ERROR: setting source\n");

    }
    alSourcePlay(bullSrc);
    // alDeleteSources(1, &alSource);
    //alDeleteBuffers(1, &alBuffer);
    //ALCcontext *Context = alcGetCurrentContext();
    //ALCdevice *Device = alcGetContextsDevice(Context);
    //alcMakeContextCurrent(NULL);
    //alcDestroyContext(Context);
    //alcCloseDevice(Device);
//#endif
    return(0);

}
int startMenuSound(){
    //found in astroids.cpp main
    //plays once becuase its called once?
    //not sure might have to make more functions?
    //when this is put in a render function it plays continuesly
    //maybe the trick is not to call it in render...
//#ifdef USE_SOUND
    initSound();

    ALuint menuSoundBuff;
    menuSoundBuff = alutCreateBufferFromFile("./intro2.wav");
    ALuint menuSoundSrc;
    alGenSources(1, &menuSoundSrc);
    alSourcei(menuSoundSrc, AL_BUFFER, menuSoundBuff);
    alSourcef(menuSoundSrc, AL_GAIN, 0.6f);
    alSourcef(menuSoundSrc, AL_PITCH, 1.0f);
    //WHEN SET TO TRUE plays for the entire game...
    alSourcei(menuSoundSrc, AL_LOOPING, AL_TRUE);
    if (alGetError() != AL_NO_ERROR) {
	printf("ERROR: setting source in gameStart\n");

    }

    //for(int i=0; i<3; i++){

    alSourcePlay(menuSoundSrc);
    printf("startsoundplayed\n");
    //usleep(20000);
    // }
    //alDeleteSources(1, &menuSoundSrc);
    //alDeleteBuffers(1, &menuSoundBuff);
    //ALCcontext *Context = alcGetCurrentContext();
    //ALCdevice *Device = alcGetContextsDevice(Context);
    //alcMakeContextCurrent(NULL);
    //alcDestroyContext(Context);
    //alcCloseDevice(Device);
//#endif
    return(0);
}


/*
   void endSound(){

   alDeleteSources(1, &menuSoundSrc);
   alDeleteBuffers(1, &menuBuffer);
   ALCcontext *Context = alcGetCurrentContext();
   ALCdevice *Device = alcGetContextsDevice(Context);
   alcMakeContextCurrent(NULL);
   alcDestroyContext(Context);
   alcCloseDevice(Device);

   }
   */

int explosion(){
    //found in astriods .cpp
    //void deleteAstriod line 808
//#ifdef USE_SOUND
    initSound();

    ALuint exploBuff;
    exploBuff = alutCreateBufferFromFile("./explosion.wav");
    ALuint exploSrc;
    alGenSources(1, &exploSrc);
    alSourcei(exploSrc, AL_BUFFER, exploBuff);
    alSourcef(exploSrc, AL_GAIN, 1.0f);
    alSourcef(exploSrc, AL_PITCH, 0.5f);
    //WHEN SET TO TRUE plays for the entire game...
        alSourcei(exploSrc, AL_LOOPING, AL_FALSE);
            if (alGetError() != AL_NO_ERROR) {
                    printf("ERROR: setting source in gameStart\n");
    
                        }
    
 alSourcePlay(exploSrc);
     printf("explosionplayed\n");
//#endif
return(0);


}
