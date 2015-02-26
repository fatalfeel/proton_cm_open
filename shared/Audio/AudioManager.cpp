#include "PlatformPrecomp.h"
#include "AudioManager.h"

AudioManager::AudioManager()
{
	m_bMusicEnabled = true;
	m_bSoundEnabled = true;
	m_bLastMusicLooping = true;
	m_lastMusicID = AUDIO_HANDLE_BLANK;
	m_bVibrationDisabled = false;
	m_musicVol = 1.0f;
}

AudioManager::~AudioManager()
{
}

void AudioManager::StopMusic()
{
	if (m_lastMusicID != AUDIO_HANDLE_BLANK)
	{
		Stop(m_lastMusicID);
		m_lastMusicID = AUDIO_HANDLE_BLANK;
		m_lastMusicFileName = "";
	}
}


void AudioManager::SetDLS(string fName)
{
	m_midiSoundBankFile = fName;
}


AudioHandle AudioManager::Play( string fName, bool bLooping , bool bIsMusic , bool bAddBasePath , bool bForceStreaming )
{
//	LogMsg("AudioManager::Play activated, not handled");
	return AUDIO_HANDLE_BLANK;
}

