//  ***************************************************************
//  AudioManagerSDL - Creation date: 09/14/2010
//  -------------------------------------------------------------
//  Robinson Technologies Copyright (C) 2010 - All Rights Reserved
//
//  ***************************************************************
//  Programmer(s):  Seth A. Robinson (seth@rtsoft.com)
//  ***************************************************************

#ifndef AudioManagerSDL_h__
#define AudioManagerSDL_h__

#include "AudioManager.h"

#if defined RT_WEBOS || defined RT_USE_SDL_AUDIO

#include "SDL_mixer.h"

class SoundObject;

class AudioManagerSDL: public AudioManager
{
public:
	//by jesse stone
	static AudioManagerSDL* GetAudioManager();
    static void Free();

	AudioManagerSDL();
	virtual ~AudioManagerSDL();

	virtual AudioHandle Play(const string fileName);

	virtual bool Init();
	virtual void Kill();

	virtual AudioHandle Play(string fName, bool bLooping = false, bool bIsMusic = false, bool bAddBasePath = true, bool bForceStreaming = false);
	virtual AudioHandle Play(string fName, int vol, int pan = 0);

	virtual void Preload(string fName, bool bLooping = false, bool bIsMusic = false, bool bAddBasePath = true, bool bForceStreaming = false);

	virtual void KillCachedSounds(bool bKillMusic, bool bKillLooping, int ignoreSoundsUsedInLastMS, int killSoundsLowerPriorityThanThis, bool bKillSoundsPlaying);
	virtual void Update();
	virtual void Stop(AudioHandle soundID);
	virtual AudioHandle GetMusicChannel();
	virtual bool IsPlaying(AudioHandle soundID);
	virtual void SetMusicEnabled(bool bNew);
	virtual void StopMusic();
	virtual void FadeOutMusic(unsigned int duration = 1000);
	virtual int GetMemoryUsed();
	virtual void SetFrequency(AudioHandle soundID, int freq);
	virtual void SetPan(AudioHandle soundID, float pan); //0 is normal stereo, -1 is all left, +1 is all right
	virtual void SetVol(AudioHandle soundID, float vol);
	virtual void SetPriority(AudioHandle soundID, int priority);
	virtual uint32 GetPos( AudioHandle soundID );
	virtual void SetPos( AudioHandle soundID, uint32 posMS );
	virtual void SetMusicVol(float vol);

private:
	SoundObject * GetSoundObjectByFileName(string fName);
	bool DeleteSoundObjectByFileName(string fName);

	list<SoundObject*> m_soundList;
	Mix_Music *m_pMusicChannel;

};


#endif // defined RT_WEBOS || defined RT_USE_SDL_AUDIO
#endif // AudioManagerSDL_h__
