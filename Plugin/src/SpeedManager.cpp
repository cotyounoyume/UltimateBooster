#include "SpeedManager.h"

namespace SpeedManager
{
	void MainLoop()
	{
		Config::ReadIni();
		bool atOnce = true;
		while (true) {
			Sleep(Config::GetTimePerFrame());
			if (!Utility::InGameScene())
				continue;

			if (SlidingBonusCount > 0) {
				Debug(fmt::format("SlidingBonusCount:{}", SlidingBonusCount));
				SlidingBonusCount--;
			}

			if (atOnce) {
				InitOnce();
				atOnce = false;
			}

			if (Events::NeedReset) {
				StopScript();
				ResetParameter();
				continue;
			}

			ToogleSpeedManagerKey();

			if (!CheckSpeedControllerEnabled()) {
				StopScript();
				continue;
			}

			StartScript();
			SpeedAdjust();
			//DebugPrint();
		}
	}

	void StartScript()
	{
		if (!ReadyForJump && !Jumped) {
			Info("SpeedManagerSFSE:StartScript");
			Utility::ExecuteCommandString("cgf \"zzUltimateBooster.StartManager\"");
			ReadyForJump = true;
			Jumped = false;
			if (Config::GetEffectEnabled()) {
				Utility::ExecuteCommandStringOnFormID(0x14, fmt::format("pms {} 1000", Config::GetEffectFormID()));
			}
			if (Config::GetWwiseEffectEnabled()) {
				Utility::ExecuteCommandString(fmt::format("cgf \"WwiseEvent.PlayMenuSound\" \"{}\"", Config::GetWwiseEffectID()));
			}
		}
	}

	void StopScript()
	{
		if (SpeedUpKeyOn || ReadyForJump) {
			Info("SpeedManagerSFSE:StopScript");
			Utility::ExecuteCommandString("cgf \"zzUltimateBooster.StopManager\"");
			SpeedUpKeyOn = false;
			ReadyForJump = false;
			Jumped = false;
			RestoreJumpBonus();
			SetAV(0x14, "jumpMult", "0");
			CommonAdjuster(&SpeedMultiplierDiff, 0, 0, 0, 0, "SPEEDMULT", 1.0, true);
			CommonAdjuster(&FallspeedMultiplierDiff, 0, 0, 0, 0, "FALLSPEEDMULT", -1.0, true);

			PLAYER->SetActorValue(*(RE::ActorValue::GetSingleton()->fallSpeedMult), BaseFallspeedMultiplier);
			if (Config::GetEffectEnabled()) {
				Utility::ExecuteCommandStringOnFormID(0x14, fmt::format("sms {}", Config::GetEffectFormID()));
			}
		}
	}

	void InitOnce()
	{
		Config::EsmNotLoadCheck();

		Info("InitOnce start.");
		PLAYER = RE::PlayerCharacter::GetSingleton();
		if (PLAYER && PLAYER->formID != 0x14) {
			PLAYER = RE::PlayerCharacter::GetSingleton();
		}
		BaseSpeedMultiplier = PLAYER->GetBaseActorValue(*(RE::ActorValue::GetSingleton()->speedMult));
		BaseFallspeedMultiplier = PLAYER->GetBaseActorValue(*(RE::ActorValue::GetSingleton()->fallSpeedMult));
		SetAV(0x14, "jumpMult", "0");
		//SPEEDMULT = *(RE::ActorValue::GetSingleton()->speedMult);
		//FALLSPEEDMULT = *(RE::ActorValue::GetSingleton()->fallSpeedMult);
		PLAYER->SetActorValue(*(RE::ActorValue::GetSingleton()->speedMult), BaseSpeedMultiplier);
		PLAYER->SetActorValue(*(RE::ActorValue::GetSingleton()->fallSpeedMult), BaseFallspeedMultiplier);
		SPCounter = Utility::GetGlobalFromString("SPCounter");
		SPJumpState = Utility::GetGlobalFromString("SPJumpState");
		SPSwitch = Utility::GetGlobalFromString("SPSwitch");
		SPWalkState = Utility::GetGlobalFromString("SPWalkState");
		SPSneakState = Utility::GetGlobalFromString("SPSneakState");
		SPSlidingState = Utility::GetGlobalFromString("SPSlidingState");
		SPJumpMult = Utility::GetGlobalFromString("SPJumpMult");
		SPJumpMultBase = Utility::GetGlobalFromString("SPJumpMultBase");
		SlidingBonusCountPlus = Config::GetSlidingBonusCount();
		Info("InitOnce done.");
	}

	void ResetParameter()
	{
		Debug(fmt::format("ResetParameter"));
		Events::NeedReset = false;
		ResetFirstJumpMult();
		InitOnce();
	}

	void ToogleSpeedManagerKey()
	{
		if (Utility::IsMenuOthersOpen())
			return;
		if (IsKeyPressed()) {
			SpeedUpKeyOn = !SpeedUpKeyOn;
			if (SpeedUpKeyOn) {
				ReadyForJump = false;
				Jumped = false;
			}
		}
	}

	bool IsKeyPressed()
	{
		if (skipCount > 0) {
			skipCount--;
			return false;
		}
		bool result = Utility::IsKeyPressedMult(Config::GetSpeedManagerKeyNumber1(), Config::GetSpeedManagerKeyNumber2());
		if (result)
			skipCount = skipCountPlus;
		return result;
	}

	bool CheckSpeedControllerEnabled()
	{
		if (SpeedUpKeyOn && !Jumped)
			return true;
		if (SpeedUpKeyOn && ReadyForJump)
			return true;
		if (SpeedMultiplierDiff > 0)
			return true;
		return false;
	}

	void SpeedAdjust()
	{
		if (Jumped) {
			if (Config::GetLogLevel() == 0)
				Info("SpeedAdjuster: active");
			CommonAdjuster(&SpeedMultiplierDiff, Config::GetMultiplierMaxThreshold(), Config::GetMultiplierMinThreshold(),
				Config::GetMultiplierUpRate(), Config::GetMultiplierDownRate(), "SPEEDMULT", 1.0);
			CommonAdjuster(&FallspeedMultiplierDiff, Config::GetFallspeedMultiplierMaxThreshold(), Config::GetFallspeedMultiplierMinThreshold(),
				Config::GetFallspeedMultiplierUpRate(), Config::GetFallspeedMultiplierDownRate(), "FALLSPEEDMULT", -1.0);
		} else {
			if (Config::GetLogLevel() == 0)
				Info("SpeedAdjuster: inactive");

			if (!IsJumping()) {
				SetFirstJumpMult();
				return;
			}
			Jumped = true;
			ReadyForJump = false;
			Debug("Jumped => true");
			ResetFirstJumpMult();
			SpeedAdjust();
		}
	}

	bool IsSliding()
	{
		if (!SPSlidingState) {
			Info("Unexpected Route(IsSliding)");
			InitOnce();
			return false;
		}
		return SPSlidingState->GetValue() == 1;
	}

	bool IsSneaking()
	{
		if (!SPSneakState) {
			Info("Unexpected Route(IsSneaking)");
			InitOnce();
			return false;
		}
		return SPSneakState->GetValue() == 1;
	}

	bool IsJumping()
	{
		if (!SPJumpState) {
			Info("Unexpected Route(IsJumping)");
			InitOnce();
			return false;
		}
		return SPJumpState->GetValue() > 0;
	}

	double GetJumpMult()
	{
		if (!SPJumpMult) {
			Info("Unexpected Route(GetJumpMult)");
			InitOnce();
			return false;
		}
		return SPJumpMult->GetValue();
	}

	bool IsSprinting()
	{
		if (!SPWalkState) {
			Info("Unexpected Route(IsSprinting)");
			InitOnce();
			return false;
		}
		return SPWalkState->GetValue() >= 100;
	}

	void CommonAdjuster(double* var, double max, double min, double upRate, double downRate, std::string type, double sign, bool reset)
	{
		if (reset) {
			*var = 0;
		} else if(SpeedUpKeyOn && IsJumping()) {
			if (*var == 0) {
				*var = max * GetInitialSpeedRatio();
			} else if (*var > max) {
				*var = max;
			} else {
				*var = *var * upRate;
			}
		} else {
			if (*var < min) {
				*var = 0;
			} else {
				*var = *var * downRate;
			}
		}
		if (Config::GetLogLevel() == 0)
			Info(fmt::format("debug:type:{}, var:{}, max:{}, min:{}, upRate:{}, downRate:{}, sign:{}, reset:{}", type, *var, max, min, upRate, downRate, sign, reset));
		if (type == "SPEEDMULT") {
			PLAYER->SetActorValue(*(RE::ActorValue::GetSingleton()->speedMult), BaseSpeedMultiplier + *var * sign);
		} else if (type == "FALLSPEEDMULT") {
			PLAYER->SetActorValue(*(RE::ActorValue::GetSingleton()->fallSpeedMult), BaseFallspeedMultiplier + *var * sign);
		}
	}

	void SetFirstJumpMult()
	{
		auto ratio = Config::GetJumpMultiplier();
		if (GetActorValue_JumpMult() < 0)
			RestoreJumpBonus();
		if (IsSliding() || IsSprinting()) {
			if (IsSliding()) {
				SlidingBonusCount = SlidingBonusCountPlus;
				Info(fmt::format("AddBonus: SlidingBonusCount: {}", SlidingBonusCount));
			}
			DisableJumpBonus();
			SetAV(0x14, "jumpMult", "0");
			return;
		}
		if (IsSneaking()) {
			double extraRatio = (SPSneakState->GetValue() == 1.0) ? Config::GetCrouchPowerRatio() : 1.0;
			ratio *= extraRatio;
		}
		std::string sRatio = std::to_string(ratio);
		SetAV(0x14, "jumpMult", sRatio);
		if (Config::GetLogLevel() == 0)
			Info(fmt::format("jumpMult: {}", sRatio));
	}

	void DisableJumpBonus()
	{
		DamageAV(0x14, "jumpMult", std::to_string(GetBaseActorValue_JumpMult()));
	}

	void RestoreJumpBonus()
	{
		RestoreAV(0x14, "jumpMult", "100");
	}

	double GetBaseActorValue_JumpMult()
	{
		if (!SPJumpMultBase) {
			Info("Unexpected Route(GetBaseActorValue_JumpMult)");
			InitOnce();
			return 0;
		}
		return SPJumpMultBase->GetValue();
	}

	double GetActorValue_JumpMult()
	{
		if (!SPJumpMult) {
			Info("Unexpected Route(GetActorValue_JumpMult)");
			InitOnce();
			return 0;
		}
		return SPJumpMult->GetValue();
	}

	void ResetFirstJumpMult()
	{
		RestoreJumpBonus();
		SetAV(0x14, "jumpMult", "0");
	}

	double GetInitialSpeedRatio()
	{
		double ratio = InitialSpeedRatioMin;
		if (SPWalkState) {
			double extraRatio = ratio;
			if (SlidingBonusCount > 0) {
				extraRatio = 1.0;
				Debug("Ratio Max");
			} else if (SPWalkState->GetValue() >= 10) {
				extraRatio = (ratio + Config::GetSprintPowerRatio()) / 2;
			} else if (SPWalkState->GetValue() >= 100) {
				extraRatio = Config::GetSprintPowerRatio();
			}
			ratio = (extraRatio < InitialSpeedRatioMin) ? InitialSpeedRatioMin : extraRatio;
			ratio = (extraRatio > 1) ? 1.0 : extraRatio;
		}
		if (Config::GetLogLevel() == 0)
			Info(fmt::format("GetInitialSpeedRatio:{}", ratio));
		return ratio;
	}

	void SetAV(uint32_t formID, std::string name, std::string value)
	{
		Utility::ExecuteCommandStringOnFormID(formID, fmt::format("setav {} {}", name, value));
	}

	void DamageAV(uint32_t formID, std::string name, std::string value)
	{
		Utility::ExecuteCommandStringOnFormID(formID, fmt::format("damageav {} {}", name, value));
	}

	void RestoreAV(uint32_t formID, std::string name, std::string value)
	{
		Utility::ExecuteCommandStringOnFormID(formID, fmt::format("restoreav {} {}", name, value));
	}

	void DebugPrint()
	{
		//auto actionValue = PLAYER->actionValue;
		auto actorState1 = PLAYER->actorState1;
		auto actorState2 = PLAYER->actorState2;

		//Info(fmt::format("debug: actionValue={}", actionValue));
		//Info(fmt::format("debug: actorState1={}", actorState1));
		std::bitset<32> binary1 = actorState1;
		std::bitset<32> binary2 = actorState2;
		Info(fmt::format("actorState1:bin1:{} ,bin2:{}", binary1.to_string(), binary2.to_string()));
		//Info(fmt::format("debug: actorState2={}", actorState2));
		uint32_t Mask = 0x11111111;
		uint32_t maskedState1 = actorState1 & Mask;
		if ((maskedState1 & 0x1111) == 0x0110) {
			Info(fmt::format("actorState1: front?"));
		} else if ((maskedState1 & 0x1111) == 0x1001) {
			Info(fmt::format("actorState1: left?"));
		} else if ((maskedState1 & 0x1111) == 0x0101) {
			Info(fmt::format("actorState1: back?rignt?"));
		} else if ((maskedState1 & 0x1111) == 0x0000) {
			Info(fmt::format("actorState1: stop?"));
		}
	}

}