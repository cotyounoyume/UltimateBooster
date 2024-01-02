#pragma once
#include "Common.h"
#include "Config.h"
#include "Event.h"

namespace SpeedManager
{
	void        MainLoop();
	void        InitOnce();
	void        ToogleSpeedManagerKey();
	bool        IsKeyPressed();
	bool        CheckSpeedControllerEnabled();
	void        SpeedAdjust();
	void        CommonAdjuster(double* var, double max, double min, double upRate, double downRate, std::string type, double sign);
	void        DebugPrint();
	void        StartScript();
	void        StopScript();
	bool        IsJumping();
	bool        IsSliding();
	bool        IsSprinting();
	double      GetJumpMult();
	void        SetAV(uint32_t formID, std::string name, std::string value);
	void        DamageAV(uint32_t formID, std::string name, std::string value);
	void        RestoreAV(uint32_t formID, std::string name, std::string value);
	void        SetFirstJumpMult();
	void        ResetFirstJumpMult();
	double      GetInitialSpeedRatio();
	double      GetBaseActorValue_JumpMult();
	double      GetActorValue_JumpMult();
	void        DisableJumpBonus();
	void        RestoreJumpBonus();
	inline bool ReadyForJump = false;
	inline bool Jumped = false;
	inline bool SpeedUpKeyOn = false;
	inline int  skipCount = 0;
	inline int  skipCountPlus = 5;
	inline int  SlidingBonusCount = 0;
	inline int  SlidingBonusCountPlus = 20;
	//inline RE::ActorValueInfo SPEEDMULT;
	//inline RE::ActorValueInfo FALLSPEEDMULT;

	inline double               BaseSpeedMultiplier = 0;
	inline double               BaseFallspeedMultiplier = 0;
	inline double               SpeedMultiplierDiff = 0;
	inline double               FallspeedMultiplierDiff = 0;
	inline double               InitialSpeedRatioMin = 0.1;
	inline RE::PlayerCharacter* PLAYER;
	inline RE::TESGlobal*       SPSneakState;
	inline RE::TESGlobal*       SPCounter;
	inline RE::TESGlobal*       SPJumpState;
	inline RE::TESGlobal*       SPSwitch;
	inline RE::TESGlobal*       SPWalkState;
	inline RE::TESGlobal*       SPSlidingState;
	inline RE::TESGlobal*       SPJumpMult;
	inline RE::TESGlobal*       SPJumpMultBase;

}