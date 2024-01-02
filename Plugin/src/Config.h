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
		{ "TestOn", true },
	};
	inline std::unordered_map<std::string, std::string> SettingsStringMapGeneralMajor = {
		{ "TestKey", "T" },
	};
	inline std::unordered_map<std::string, int> SettingsIntMapGeneralMajor = {
		{ "TestNum", 1 },
	};

	inline std::unordered_map<std::string, bool>        SettingsBoolMapGeneralMinor = {};
	inline std::unordered_map<std::string, std::string> SettingsStringMapGeneralMinor = {};
	inline std::unordered_map<std::string, int>         SettingsIntMapGeneralMinor = {};

	inline std::unordered_map<std::string, bool>        SettingsBoolMapDebug = {};
	inline std::unordered_map<std::string, std::string> SettingsStringMapDebug = {};
	inline std::unordered_map<std::string, int>         SettingsIntMapDebug = {
        { "LogLevel", 0 },
	};

	void        ReadIni();
	void        SetTomlPath();
	void        DumpSettings();
	int         GetLogLevel();
	std::string GetTestKey();
	char        GetTestKeyNumber();
	bool        GetTestOn();
	int         GetTestNum();

	std::unordered_map<std::string, bool>*        GetBoolMapByCategory(std::string category);
	std::unordered_map<std::string, std::string>* GetStringMapByCategory(std::string category);
	std::unordered_map<std::string, int>*         GetIntMapByCategory(std::string category);
	void                                          ReadConfigBool(std::string category, std::string name);
	void                                          ReadConfigString(std::string category, std::string name);
	void                                          ReadConfigInt(std::string category, std::string name);
}