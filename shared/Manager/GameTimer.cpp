#include "PlatformPrecomp.h"
#include "GameTimer.h"

#define C_MAXIMUM_DELTA_ALLOWED 100 //phones aren't allowed to be slower than this

//this FPS averaging isn't needed for windows, but the iphone timer seems to be pretty crap...
#define FPS_AVERAGING_HISTORY 8  //average out the last X amount of frames for a visually smoother fps, due to crap timing


GameTimer::GameTimer()
{
	m_lastTimeMS = m_timeMS = 0;
	m_bGameTimerPaused = false;
	m_fps = m_fpsTemp = 0;
	m_fpsTimer = 0;
	m_deltaFloat = 1;
	m_gameTimer = 0;
}

GameTimer::~GameTimer()
{
}

void GameTimer::Update()
{
	m_timeMS = uint32(GetSystemTimeAccurate());
	m_deltaMS =  m_timeMS - m_lastTimeMS;
	//if (m_deltaMS == 0) goto loop;
	if (m_deltaMS > C_MAXIMUM_DELTA_ALLOWED) m_deltaMS = C_MAXIMUM_DELTA_ALLOWED;

	m_tickHistory.push_back(float(m_deltaMS));
	if (m_tickHistory.size() > FPS_AVERAGING_HISTORY)
	{
		m_tickHistory.pop_front();
	}
	float tempDelta = 0;

	for (uint32 i=0; i < m_tickHistory.size(); i++)
	{
		tempDelta += float(m_tickHistory[i]);
	}
	tempDelta /= float(m_tickHistory.size());


	int tickDiff = int32(m_deltaMS)-int32(tempDelta);

	m_deltaMS = (int)tempDelta;
	m_lastTimeMS =  m_timeMS;

	if (tickDiff >0 && tickDiff < 5)
	{
		//attempt to compensate for our tweaks so we don't slow/speed up time in the general scheme of things
		m_lastTimeMS -= tickDiff;
	}

	//we have a maximum delta because above a certain point all collision detection would
	//be broken. 

	if (!m_bGameTimerPaused)
	{
		//advance game timer 
		m_gameTimer += m_deltaMS;
	}
	
	m_deltaFloat = float(m_deltaMS)/ (1000.0f/50.0f); //plan on 50 FPS being about average, which will return 1.0
	
	//LogMsg("Delta: %.5f", m_deltaFloat);
	//if (m_deltaFloat == 0) m_deltaFloat = 0.0000001; //avoid divide by 0 errors later

	if (m_fpsTimer < m_timeMS)
	{
		m_fpsTimer = m_timeMS+1000;
		m_fps = m_fpsTemp;
		m_fpsTemp = 0;
	}

	m_fpsTemp++;
}

void GameTimer::Reset()
{
	m_lastTimeMS = m_timeMS = uint32(GetSystemTimeAccurate()); //make sure this is valid now
	m_bGameTimerPaused = false;
	m_fps = m_fpsTemp = 0;
	m_fpsTimer = 0;
	m_deltaFloat = 1;
}

void GameTimer::SetGameTickPause( bool bNew )
{
 m_bGameTimerPaused = bNew;
}