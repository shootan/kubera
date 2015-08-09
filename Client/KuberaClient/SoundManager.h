#pragma once
#include "header.h"
#include "fmod.hpp"
#include "fmod_errors.h"

#pragma comment(lib, "fmodex_vc.lib")

enum
{
	SOUND_SELECT_BGM,
	SOUND_MAIN_BGM,
	SOUND_WARR_MOVE,
	SOUND_WARR_SELECT,
	SOUND_WARR_ATTACK,
	SOUND_WARR_SKILL,
	SOUND_SKEL_MOVE,
	SOUND_SKEL_SELECT,
	SOUND_SKEL_ATTACK,
	SOUND_SKEL_SKILL,
	SOUND_SKEL_DEATH,
	SOUND_WARR_DEATH
};

class SoundManager{
private:
	SoundManager()
	{
		;
	}
public:
	~SoundManager()
	{
		;
	}

	static SoundManager* instance;
	static SoundManager* sharedManager()
	{
		if(instance == NULL) instance = new SoundManager;
		return instance;
	}

	FMOD::System*		fsystem; //FMOD 엔진

	FMOD::Channel*		bgaCH; // 채널
	FMOD::Channel*		effCh;
	FMOD::Channel*		heroEffCh;

	FMOD::Sound*		SelectBGM;
	FMOD::Sound*		MainBGM;

	FMOD::Sound*		WarriorSelect;
	FMOD::Sound*		WarriorMove;
	FMOD::Sound*		WarriorAttack;
	FMOD::Sound*		WarriorSkill;
	FMOD::Sound*		SkelSelect;
	FMOD::Sound*		SkelMove;
	FMOD::Sound*		SkelAttack;
	FMOD::Sound*		SkelSkill;
	FMOD::Sound*		WarriorDeath;
	FMOD::Sound*		SkelDeath;



	FMOD_RESULT			result;


public:
	void init()
	{
		result = FMOD::System_Create(&fsystem);
		ERRCHECK(result);

		fsystem->init(100, FMOD_INIT_NORMAL, NULL);
		ERRCHECK(result);

	}

	void sndLoad()
	{
		result = fsystem->createSound("Sound/배경_1.wav", FMOD_LOOP_NORMAL, 0, &SelectBGM);
		ERRCHECK(result);

		result = fsystem->createSound("Sound/배경_3.wav", FMOD_LOOP_NORMAL, 0, &MainBGM);
		ERRCHECK(result);

 		result = fsystem->createSound("Sound/전사무브.mp3", FMOD_DEFAULT, 0, &WarriorMove);
 		ERRCHECK(result);

		result = fsystem->createSound("Sound/전사선택.mp3", FMOD_DEFAULT, 0, &WarriorSelect);
		ERRCHECK(result);

		result = fsystem->createSound("Sound/전사공격.mp3", FMOD_DEFAULT, 0, &WarriorAttack);
		ERRCHECK(result);

		result = fsystem->createSound("Sound/전사스킬.mp3", FMOD_DEFAULT, 0, &WarriorSkill);
		ERRCHECK(result);

		result = fsystem->createSound("Sound/해골무브.mp3", FMOD_DEFAULT, 0, &SkelMove);
		ERRCHECK(result);

		result = fsystem->createSound("Sound/해골선택.mp3", FMOD_DEFAULT, 0, &SkelSelect);
		ERRCHECK(result);
		result = fsystem->createSound("Sound/해골공격.wav", FMOD_DEFAULT, 0, &SkelAttack);
		ERRCHECK(result);
		result = fsystem->createSound("Sound/해골스킬.wav", FMOD_DEFAULT, 0, &SkelSkill);
		ERRCHECK(result);

		result = fsystem->createSound("Sound/전사죽음.wav", FMOD_DEFAULT, 0, &WarriorDeath);
		ERRCHECK(result);

		result = fsystem->createSound("Sound/해골죽음.mp3", FMOD_DEFAULT, 0, &SkelDeath);
		ERRCHECK(result);

	}
	void play(int _val)
	{
		fsystem->update();
		switch(_val)
		{
		case SOUND_SELECT_BGM:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, SelectBGM, false, &bgaCH);
				ERRCHECK(result);
				break;
			}
		case SOUND_MAIN_BGM:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, MainBGM, false, &bgaCH);
				ERRCHECK(result);
				break;
			}
		case SOUND_WARR_MOVE:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, WarriorMove, false, &heroEffCh);
				ERRCHECK(result);
				break;
			}

		case SOUND_WARR_SELECT:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, WarriorSelect, false, &heroEffCh);
				ERRCHECK(result);
				break;
			}
		case SOUND_WARR_ATTACK:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, WarriorAttack, false, &heroEffCh);
				ERRCHECK(result);
				break;
			}
		case SOUND_WARR_SKILL:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, WarriorSkill, false, &heroEffCh);
				ERRCHECK(result);
				break;
			}
		case SOUND_SKEL_MOVE:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, SkelMove, false, &heroEffCh);
				ERRCHECK(result);
				break;
			}
		case SOUND_SKEL_SELECT:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, SkelSelect, false, &heroEffCh);
				ERRCHECK(result);
				break;
			}
		case SOUND_SKEL_ATTACK:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, SkelAttack, false, &heroEffCh);
				ERRCHECK(result);
				break;
			}
		case SOUND_SKEL_SKILL:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, SkelSkill, false, &heroEffCh);
				ERRCHECK(result);
				break;
			}
		case SOUND_SKEL_DEATH:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, SkelDeath, false, &heroEffCh);
				ERRCHECK(result);
				break;
			}
		case SOUND_WARR_DEATH:
			{
				fsystem->playSound(FMOD_CHANNEL_FREE, WarriorDeath, false, &heroEffCh);
				ERRCHECK(result);
				break;
			}
		}
	}

	void stop(int _val)
	{
		switch(_val)
		{
		case 0:
			bgaCH->stop();
			break;

		case 1:
			effCh->stop();
			break;

		case 2:
			heroEffCh->stop();
			break;
		}
	}


	void ERRCHECK(FMOD_RESULT result)
	{
		TCHAR error[256];
		if(result!=FMOD_OK)
		{
			_stprintf_s(error, L"%s", FMOD_ErrorString(result));
			printf("%s", error);
			//TextOut(ST::sharedClass()->m_hBackBufferDC, 500, 500, error, lstrlen(error));
		}
	}

};