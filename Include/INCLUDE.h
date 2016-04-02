
#pragma		once

#ifndef		__INCLUDE_H_
#define		__INCLUDE_H_

//09-08-20 이재준
#include	"DEFAULT_INCLUDE.h"

//	애니메이션 모델 관련 헤더 파일 
#include	"Allocatehierarchy.h"
#include	"Frame.h"
#include	"MeshContainer.h"
#include	"KeyManager.h"

#include	"Background.h"

#include	"Net.h"

#include	"Ball.h"

//09-09-09 이웅재 
#include	"../Network/_NetworkAfx.h"
#include	"../Network/Packet.h"
#include	"../Network/RingBuffer.h"
#include	"../Network/Client.h"
#include   "../Network/NetworkDlg.h"


#include	"CharacterAnimationController.h"
#include	"Character.h"
#include	"Player.h"
#include	"User.h"
#include	"NPC.h"

#include	"CrashManager.h"

//09-08-20 이재준
#include "../Camera/CameraAnimationController.h"
#include "../Camera/CameraAnimationModel.h"

//09-08-04 이재준
#include "../Camera/McUtil.h"
#include "../Camera/McCamera.h"
#include "../Camera/McCamManager.h"
#include "../Input/McInput.h"
#include "../Classes/GameTexture.h"
#include "../Classes/MiMp3.h"

//09-08-11 이재준
#include "../Classes/StartScreen.h"
#include "../Classes/MultiPlay.h"
#include "../Classes/HowtoPlay.h"
#include "../Classes/EndGame.h"
#include "../Classes/CharacterSelect.h"
#include "../Classes/GameSceneManager.h"

//09-09-07 이재준
#include "../Input/CDrawText.h"

#include	"SexySpike.h"

#endif