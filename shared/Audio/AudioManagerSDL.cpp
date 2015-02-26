#include "PlatformPrecomp.h"

//NOTE:  Much of this code is from the tiltodemo sample from the Palm WebBOS PDK

#if defined RT_WEBOS || defined RT_USE_SDL_AUDIO

#include "SDL.h"

#define NUM_CHANNELS 16

#include "AudioManagerSDL.h"
#include "util/MiscUtils.h"

//0 is a valid channel in SDL audio, but 0 is a bad audio handle the rest of Proton, this hack gets around it
#define C_CHANNEL_OFFSET_SO_ZERO_ISNT_USED 1


class SoundObject
{
public:
	SoundObject() :
	    m_pSound(NULL),
	    m_bIsLooping(false),
	    m_pLastChannelToUse(AUDIO_HANDLE_BLANK)
	{
	}

	~SoundObject()
	{
		Mix_FreeChunk(m_pSound);
	}

	Mix_Chunk *m_pSound;
	string m_fileName;
	bool m_bIsLooping;
	int m_pLastChannelToUse;
};

static bool g_MusicHasFinished = false;

void musicFinishedCallback()
{
	g_MusicHasFinished = true;
}

//by jesse stone
static AudioManagerSDL* audio_sdl_manager_instance = NULL;

AudioManagerSDL* AudioManagerSDL::GetAudioManager()
{
	if (!audio_sdl_manager_instance)
		audio_sdl_manager_instance = new AudioManagerSDL;
	return audio_sdl_manager_instance;
}

void AudioManagerSDL::Free()
{
	if( audio_sdl_manager_instance )
	{
		delete audio_sdl_manager_instance;
		audio_sdl_manager_instance = NULL;
	}
}

AudioManagerSDL::AudioManagerSDL()
{
	m_pMusicChannel = NULL;
	
}

AudioManagerSDL::~AudioManagerSDL()
{
	Kill();
}

AudioHandle AudioManagerSDL::Play( const string fileName )
{
	LogMsg("Playing %s", fileName.c_str());
	assert(!"huh?");
	return AUDIO_HANDLE_BLANK;
}

bool AudioManagerSDL::Init()
{
	// we'll use SDL_Mixer to do all our sound playback. 
	// It's a simple system that's easy to use. The most 
	// complicated part about using SLD_Mixer is initting it. So 
	// let's talk about that. 
	// this is the function definition:
	// Mix_OpenAudio(int frequency, Uint16 format, int channels, int chunksize);
	// here's what it wants:
	// 
	// frequency: this is the sample rate you want the audio to
	// play at. On the Palm, things are optimized for 44100 samples per
	// second (CD quality). Though you can send in whatever sample rate you like.
	// 
	// format: For the Palm, you should use AUDIO_S16
	// 
	// channels: 1 for mono. 2 for stereo
	// 
	// chunksize: How big one audio buffer is, in bytes.
	// 
	// this example has the recommended settings for initting the mixer:

	int rate = 44100;
	Uint16 format = AUDIO_S16;
	int channels = 2;
	int bufferSize = 1024;
	
	if ( Mix_OpenAudio(rate, format, channels, bufferSize) )
	{
		// we had an error opening the audio
		LogMsg("unable to open Audio! Reason: %s\n", Mix_GetError());
		return false;

	}

	// SDL mixer requires that you specify the maximum number of simultaneous
	// sounds you will have. This is done through the function Mix_AllocateChannels. 
	// We'll need an answer for that. A channel has very little overhead,
	// so we'll arbitrarily allocate 16 channels (even though we're only using
	// 1 actual sound channel). Among other things, we need a channel per unique
	// simultaneous sound playback. So if we play the same sound 4 times, and have
	// 4 copies of the sound playing from different start times, we need four channels.
	// Note - streamed music does not consume a channel. Only sound playback does.
	// So why allocate more than we need? Because it's such a small overhead, and
	// later we don't need to come and revisit this every time we add a new sound. 
	Mix_AllocateChannels(NUM_CHANNELS);

	Mix_HookMusicFinished(musicFinishedCallback);

	return true;
}

void AudioManagerSDL::KillCachedSounds(bool bKillMusic, bool bKillLooping, int ignoreSoundsUsedInLastMS, int killSoundsLowerPriorityThanThis, bool bKillSoundsPlaying)
{
	LogMsg("Killing sound cache");
	list<SoundObject*>::iterator itor = m_soundList.begin();

	while (itor != m_soundList.end())
	{
		if (!bKillLooping && (*itor)->m_bIsLooping) 
		{
			itor++;
			continue;
		}

		if (!bKillSoundsPlaying)
		{
			//are any channels currently using this sound?
			if ((*itor)->m_pLastChannelToUse != AUDIO_HANDLE_BLANK && IsPlaying((AudioHandle)(*itor)->m_pLastChannelToUse))
			{
				itor++;
				continue;
			}
		}

		if (!bKillMusic && (*itor)->m_fileName == m_lastMusicFileName)
		{
			itor++;
			continue; //skip this one
		}

		delete (*itor);
		list<SoundObject*>::iterator itorTemp = itor;
		itor++;
		m_soundList.erase(itorTemp);
	}

	if (bKillMusic)
	{
		m_pMusicChannel = NULL;
	}
}

void AudioManagerSDL::Kill()
{
	Mix_HaltMusic();
	if ( m_pMusicChannel != NULL )
	{
		// free up any memore in use by the music track
		Mix_FreeMusic(m_pMusicChannel);

		// set it to null for safety (ensuring that 
		// if we acidentally refer to this variable after
		// deletion, at least it will be null)
		m_pMusicChannel = NULL; 
	}

	// close out the audio
	Mix_CloseAudio();
}

bool AudioManagerSDL::DeleteSoundObjectByFileName(string fName)
{
	list<SoundObject*>::iterator itor = m_soundList.begin();

	while (itor != m_soundList.end())
	{
		if ( (*itor)->m_fileName == fName)
		{
			delete (*itor);
			m_soundList.erase(itor);
			return true; //deleted
		}
		itor++;
	}

	return false; //failed
}

SoundObject * AudioManagerSDL::GetSoundObjectByFileName(string fName)
{
	list<SoundObject*>::iterator itor = m_soundList.begin();

	while (itor != m_soundList.end())
	{
		if ( (*itor)->m_fileName == fName)
		{
			return (*itor); //found a match
		}
		itor++;
	}

	return 0; //failed
}

void AudioManagerSDL::Preload( string fName, bool bLooping /*= false*/, bool bIsMusic /*= false*/, bool bAddBasePath /*= true*/, bool bForceStreaming )
{

	if (bIsMusic) return;//we don't preload music that way

	SoundObject *pObject = GetSoundObjectByFileName( fName.c_str());

	if (!pObject)
	{
		//create it
		pObject = new SoundObject;
		pObject->m_fileName = fName;

		assert(! (GetFileExtension(fName) == "mp3") && "SDL mixer doesn't support mp3 for non music playback though");

		string basePath;
		if (bAddBasePath)
		{
			basePath = GetBaseAppPath();
		}

		pObject->m_pSound = Mix_LoadWAV( (basePath+fName).c_str());
		if (!pObject->m_pSound)
		{
			LogMsg("Error loading %s (%s)", (basePath+fName).c_str(), Mix_GetError());
			delete pObject;
			return;
		}
#ifdef _DEBUG
		LogMsg("Caching out %s", fName.c_str());
#endif
		
		pObject->m_bIsLooping = bLooping;

		m_soundList.push_back(pObject);
	}
}


AudioHandle AudioManagerSDL::Play( string fName, bool bLooping /*= false*/, bool bIsMusic, bool bAddBasePath, bool bForceStreaming)
{

#ifdef _DEBUG
	//LogMsg("********** AudioSDL: Thinking of playing %s, music=%d", fName.c_str(), int(bIsMusic));
#endif

	if (!GetSoundEnabled() && !bIsMusic) return AUDIO_HANDLE_BLANK;
	if (!GetMusicEnabled() && bIsMusic)
	{
		m_bLastMusicLooping = bLooping;
		m_lastMusicFileName = fName;

		return AUDIO_HANDLE_BLANK;
	}

	if (bIsMusic && m_bLastMusicLooping == bLooping && m_lastMusicFileName == fName && m_bLastMusicLooping && IsPlaying((AudioHandle) m_pMusicChannel))
	{
		return (AudioHandle) m_pMusicChannel;
	}

	int loops = 0;
	if (bLooping) loops = -1;

	if (bIsMusic)
	{
		string basePath;

		if (bAddBasePath)
		{
			basePath = GetBaseAppPath();
		}

		m_lastMusicFileName = fName;

		StopMusic();
		m_pMusicChannel = Mix_LoadMUS( (basePath+fName).c_str());

		if (!m_pMusicChannel && !bAddBasePath)
		{
			LogError("Couldn't load %s, trying again with full path", (basePath+fName).c_str());

			basePath = GetBaseAppPath();
			//try again with the basepath added.. the SDL sound system on webos seems to require it
			m_pMusicChannel = Mix_LoadMUS((basePath+fName).c_str());
		}

		if (!m_pMusicChannel)
		{
			LogError("Unable to load music file %s. Missing?", (basePath+fName).c_str());
			return AUDIO_HANDLE_BLANK;
		}
		m_lastMusicID = (AudioHandle) m_pMusicChannel;
		m_bLastMusicLooping = bLooping;
		
		SetMusicVol(m_musicVol);

		int ret = Mix_PlayMusic(m_pMusicChannel, loops);
		if (ret == -1)
		{
			LogError("Unable to play music file %s.", (GetBaseAppPath()+fName).c_str());
		}

		return (AudioHandle) m_pMusicChannel;
	}

	//non music

	SoundObject *pObject = GetSoundObjectByFileName(fName);

	if (!pObject)
	{
		//create it
		Preload(fName, bLooping, bIsMusic, bAddBasePath, bForceStreaming);
		pObject = GetSoundObjectByFileName(fName);
		if (!pObject)
		{
			LogError("Unable to cache sound %s", fName.c_str());
			return AUDIO_HANDLE_BLANK;

		}
	}

#ifdef _DEBUG
	//LogMsg("AudioSDL: Playing sfx %s", fName.c_str());
#endif

	//play it
	
	int channel = Mix_PlayChannel(-1, pObject->m_pSound, loops);
	if (channel == -1)
	{
		pObject->m_pLastChannelToUse = AUDIO_HANDLE_BLANK;
		return AUDIO_HANDLE_BLANK;
	}
	pObject->m_pLastChannelToUse = channel + C_CHANNEL_OFFSET_SO_ZERO_ISNT_USED;

	//need this because sometimes it's set to nothing by default??
	SetVol(pObject->m_pLastChannelToUse, 1.0f);

	return (AudioHandle)pObject->m_pLastChannelToUse ;
}

AudioHandle AudioManagerSDL::Play( string fName, int vol, int pan /*= 0*/ )
{	
	
	assert(!"We don't support this");
	return AUDIO_HANDLE_BLANK;
}

void AudioManagerSDL::Update()
{
	if (g_MusicHasFinished)
	{
		g_MusicHasFinished = false;
		StopMusic();
	}
}

void AudioManagerSDL::Stop( AudioHandle soundID )
{
	
	if (!soundID) return;
	
	if (soundID == (AudioHandle)m_pMusicChannel)
	{
		StopMusic();
		return;
	}
	//pChannel->stop();

	Mix_HaltChannel(soundID-C_CHANNEL_OFFSET_SO_ZERO_ISNT_USED);

}

AudioHandle AudioManagerSDL::GetMusicChannel()
{
	return (AudioHandle)m_pMusicChannel;

}

bool AudioManagerSDL::IsPlaying( AudioHandle soundID )
{
	if (soundID == 0) return false;


	if (soundID == (AudioHandle) m_pMusicChannel)
	{
		return Mix_PlayingMusic() != 0;
	}

	return Mix_Playing(soundID-C_CHANNEL_OFFSET_SO_ZERO_ISNT_USED) != 0;
}


void AudioManagerSDL::SetMusicEnabled( bool bNew )
{
	if (bNew != m_bMusicEnabled)
	{
		AudioManager::SetMusicEnabled(bNew);
		if (bNew)
		{
			if (!m_lastMusicFileName.empty())
			{
				Play(m_lastMusicFileName, GetLastMusicLooping(), true);

			}
		} else
		{
			//kill the music
			StopMusic();
		}
	}
}

void AudioManagerSDL::StopMusic()
{
	DeleteSoundObjectByFileName(m_lastMusicFileName);
	Mix_HaltMusic();
	Mix_FreeMusic(m_pMusicChannel);
	m_pMusicChannel = NULL;
	m_lastMusicID = AUDIO_HANDLE_BLANK;
}

void AudioManagerSDL::FadeOutMusic(unsigned int duration)
{
	Mix_FadeOutMusic(duration);
}

int AudioManagerSDL::GetMemoryUsed()
{
	return 0;
}

void AudioManagerSDL::SetFrequency( AudioHandle soundID, int freq )
{
	assert(soundID);
	//SDL::Channel *pChannel = (SDL::Channel*) soundID;
	//pChannel->setFrequency(float(freq));
}

void AudioManagerSDL::SetPan( AudioHandle soundID, float pan )
{
	assert(soundID);
	
	//SDL::Channel *pChannel = (SDL::Channel*) soundID;
	//pChannel->setPan(pan);
}

uint32 AudioManagerSDL::GetPos( AudioHandle soundID )
{
	assert(soundID);
	//unsigned int pos;
//	pChannel->getPosition(&pos, SDL_TIMEUNIT_MS);
	return 0;
}

void AudioManagerSDL::SetPos( AudioHandle soundID, uint32 posMS )
{
	assert(soundID);
	
	//pChannel->setPosition(posMS, SDL_TIMEUNIT_MS);
}


void AudioManagerSDL::SetVol( AudioHandle soundID, float vol )
{
	int ivol =  int(vol*128.0f);

	if (soundID == AUDIO_HANDLE_BLANK) return;
#ifdef _DEBUG
	//ivol = 128;
	//LogMsg("Setting audio handle %d to %d", soundID, ivol);
#endif

	Mix_Volume(soundID-C_CHANNEL_OFFSET_SO_ZERO_ISNT_USED,ivol);
	//assert(soundID);
	//pChannel->setVolume(vol);
}

void AudioManagerSDL::SetMusicVol(float vol )
{
	int ivol =  int(vol*128.0f);
	if (m_pMusicChannel)
	{
		Mix_VolumeMusic(ivol);
	}
	m_musicVol = vol;
}


void AudioManagerSDL::SetPriority( AudioHandle soundID, int priority )
{

}
#endif
