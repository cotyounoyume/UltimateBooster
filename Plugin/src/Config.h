#pragma once
#include "Common.h"
#include "winuser.h"

namespace Config
{
	inline toml::v3::ex::parse_result TomlConfig;
	inline std::vector<std::string>   Categories = {
        "GeneralMajor",
        "GeneralMinor",
        "Debug",
	};
	inline bool tomlParseError = false;

	inline std::unordered_map<std::string, bool> SettingsBoolMapGeneralMajor = {
		{ "EffectShaderForSpeedupOn", true },
	};
	inline std::unordered_map<std::string, std::string> SettingsStringMapGeneralMajor = {
		{ "SpeedManagerKey", "T" },
		{ "EffectShaderFormIDForSpeedUp", "000ef400" },
	};
	inline std::unordered_map<std::string, int> SettingsIntMapGeneralMajor = {
		{ "SlidingBonusCount", 20 },
	};

	inline std::unordered_map<std::string, double> SettingsFloatMapGeneralMajor = {
		{ "MultiplierMinThreshold", 0.1 },
		{ "MultiplierMaxThreshold", 400.0 },
		{ "MultiplierDownRate", 0.8 },
		{ "MultiplierUpRate", 1.05 },
		{ "FallspeedMultiplierMinThreshold", 0.1 },
		{ "FallspeedMultiplierMaxThreshold", 75.0 },
		{ "FallspeedMultiplierDownRate", 0.9 },
		{ "FallspeedMultiplierUpRate", 1.05 },
		{ "CrouchPowerRatio", 2.0 },
		{ "SprintPowerRatio", 0.7 },
		{ "FirstJumpMultiplier", 3.0 },
	};

	inline std::unordered_map<std::string, bool>        SettingsBoolMapGeneralMinor = {};
	inline std::unordered_map<std::string, std::string> SettingsStringMapGeneralMinor = {};
	inline std::unordered_map<std::string, double>      SettingsFloatMapGeneralMinor = {};

	inline std::unordered_map<std::string, int> SettingsIntMapGeneralMinor = {
		{ "TimePerFrame", 50 },
	};

	inline std::unordered_map<std::string, bool>        SettingsBoolMapDebug = {};
	inline std::unordered_map<std::string, std::string> SettingsStringMapDebug = {};
	inline std::unordered_map<std::string, double>      SettingsFloatMapDebug = {};
	inline std::unordered_map<std::string, int>         SettingsIntMapDebug = {
        { "LogLevel", 0 },
	};

	void        ReadIni();
	void        SetTomlPath();
	void        DumpSettings();
	int         GetLogLevel();
	std::string GetSpeedManagerKey();
	std::string GetEffectFormID();
	bool        GetEffectEnabled();
	char        GetSpeedManagerKeyNumber();
	int         GetTimePerFrame();
	int         GetSlidingBonusCount();
	double      GetCrouchPowerRatio();
	double      GetSprintPowerRatio();
	double      GetJumpMultiplier();
	double      GetMultiplierMinThreshold();
	double      GetMultiplierMaxThreshold();
	double      GetMultiplierDownRate();
	double      GetMultiplierUpRate();
	double      GetFallspeedMultiplierMinThreshold();
	double      GetFallspeedMultiplierMaxThreshold();
	double      GetFallspeedMultiplierDownRate();
	double      GetFallspeedMultiplierUpRate();

	std::unordered_map<std::string, bool>*        GetBoolMapByCategory(std::string category);
	std::unordered_map<std::string, std::string>* GetStringMapByCategory(std::string category);
	std::unordered_map<std::string, int>*         GetIntMapByCategory(std::string category);
	std::unordered_map<std::string, double>*      GetFloatMapByCategory(std::string category);
	void                                          ReadConfigBool(std::string category, std::string name);
	void                                          ReadConfigString(std::string category, std::string name);
	void                                          ReadConfigInt(std::string category, std::string name);
	void                                          ReadConfigFloat(std::string category, std::string name);
}