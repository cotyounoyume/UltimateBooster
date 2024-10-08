#include "Config.h"

namespace Config
{
	void SetTomlPath()
	{
		std::string           name = Plugin::NAME.data();
		std::string           pathString = "Data/SFSE/Plugins/" + name + ".ini";
		std::filesystem::path path = std::filesystem::path(pathString);
		if (!std::filesystem::exists(path)) {
			Error(fmt::format("path:{} not found", path.string()));
			tomlParseError = true;
			return;
		}
		tomlParseError = false;

		Info(fmt::format("{}: path={}", name, path.string()));
		try {
			TomlConfig = toml::parse_file(path.string());
			tomlParseError = false;
		} catch (const toml::parse_error& err) {
			tomlParseError = true;
			auto        tomlerror = std::string(err.description());
			std::string prefix = "There is an error in editing " + name + ".ini. _ ";
			std::string message = prefix +
			                      "Loading of ini file has been stopped and the contents of this file will not be reflected. _ \
Please correct ini file according to the error message. _ \
If you are unsure, please re-download the file for this mod or otherwise restore it to its original state.";

			Info(fmt::format("{}: Exception: {}\n{}", name, tomlerror, message));
			//if (Utility::InGameScene(true)) {
			//	Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.PrintMessage\" \"{}\"", message));
			//}
		} catch (...) {
			tomlParseError = true;
			std::string prefix = "There is an error in editing " + name + ".ini. _ ";
			std::string message = prefix +
			                      "Loading of ini file has been stopped and the contents of this file will not be reflected. _ \
Please correct ini file according to the error message. _ \
If you are unsure, please re-download the file for this mod or otherwise restore it to its original state.";
			Info(fmt::format("{}: Exception: Unknown Error.\n{}", name, message));
			//if (Utility::InGameScene(true)) {
			//	Utility::ExecuteCommandString(fmt::format("cgf \"zzStrippingArmor.PrintMessage\" \"{}\"", message));
			//}
		}
	}

	void ReadIni()
	{
		SetTomlPath();

		for (auto category : Categories) {
			auto boolMap = GetBoolMapByCategory(category);
			for (auto itr = (*boolMap).begin(); itr != (*boolMap).end(); ++itr) {
				ReadConfigBool(category, itr->first);
			}

			auto stringMap = GetStringMapByCategory(category);
			for (auto itr = (*stringMap).begin(); itr != (*stringMap).end(); ++itr) {
				ReadConfigString(category, itr->first);
			}

			auto intMap = GetIntMapByCategory(category);
			for (auto itr = (*intMap).begin(); itr != (*intMap).end(); ++itr) {
				ReadConfigInt(category, itr->first);
			}

			auto floatMap = GetFloatMapByCategory(category);
			for (auto itr = (*floatMap).begin(); itr != (*floatMap).end(); ++itr) {
				ReadConfigFloat(category, itr->first);
			}
		}
		//DumpSettings();
		U::SetLogLevel(GetLogLevel());
	}

	void EsmNotLoadCheck()
	{
		Info(fmt::format("Debug EsmNotLoadCheck start"));
		if (!EsmNotLoadCheckEnabled())
			return;
		if (!EsmCheck)
			return;
		auto form = RE::TESForm::LookupByEditorID(UniqueEditorID);
		std::string name = Plugin::NAME.data();
		std::string msg1 = "ERROR::There is an error about the " + name + "._ ";
		std::string msg2 = name + ".esm cannot be found._ Perhaps you have failed to install the Plugin.txt Enabler or have made a mistake in its configuration._ Please review the Plugin.txt Enabler and its settings.";
		std::string msg = msg1 + msg2;
		if (!form)
			Utility::ExecuteCommandString(fmt::format("cgf \"debug.messagebox\" \"{}\"", msg));
		Info(fmt::format("Debug EsmNotLoadCheck finish"));
	}

	void DumpSettings()
	{
		for (auto category : Categories) {
			auto boolMap = GetBoolMapByCategory(category);
			for (auto itr = (*boolMap).begin(); itr != (*boolMap).end(); ++itr) {
				Info(fmt::format("Bool: category:{}, name:{}, value:{}", category, itr->first, itr->second));
			}

			auto stringMap = GetStringMapByCategory(category);
			for (auto itr = (*stringMap).begin(); itr != (*stringMap).end(); ++itr) {
				Info(fmt::format("String: category:{}, name:{}, value:{}", category, itr->first, itr->second));
			}

			auto intMap = GetIntMapByCategory(category);
			for (auto itr = (*intMap).begin(); itr != (*intMap).end(); ++itr) {
				Info(fmt::format("Int: category:{}, name:{}, value:{}", category, itr->first, itr->second));
			}
		}
	}

	//GeneralMajor
	std::string GetSpeedManagerKey1() { return SettingsStringMapGeneralMajor["SpeedManagerKey1"]; }
	std::string GetSpeedManagerKey2() { return SettingsStringMapGeneralMajor["SpeedManagerKey2"]; }
	std::string GetEffectFormID() { return SettingsStringMapGeneralMajor["EffectShaderFormIDForSpeedUp"]; }
	std::string GetWwiseEffectID() { return SettingsStringMapGeneralMinor["FxWwiseEffectID"]; }
	std::string GetControllerButton() { return SettingsStringMapGeneralMajor["ControllerButton"]; }

	uint32_t GetControllerButtonCode() {
	    std::string button = GetControllerButton();
	    if (button == "A") return SFSE::InputMap::kGamepadButtonOffset_A;
	    if (button == "B") return SFSE::InputMap::kGamepadButtonOffset_B;
	    if (button == "X") return SFSE::InputMap::kGamepadButtonOffset_X;
	    if (button == "Y") return SFSE::InputMap::kGamepadButtonOffset_Y;
	    if (button == "LB") return SFSE::InputMap::kGamepadButtonOffset_LEFT_SHOULDER;
	    if (button == "RB") return SFSE::InputMap::kGamepadButtonOffset_RIGHT_SHOULDER;
	    if (button == "LT") return SFSE::InputMap::kGamepadButtonOffset_LT;
	    if (button == "RT") return SFSE::InputMap::kGamepadButtonOffset_RT;
	    // 他のボタンも必要に応じて追加
	    return 0;  // 未知のボタン
	}

	char GetSpeedManagerKeyNumber1()
	{
		if (SettingsStringMapGeneralMajor["SpeedManagerKey1"] == "None")
			return 0;
		return SettingsStringMapGeneralMajor["SpeedManagerKey1"].c_str()[0];
	}
	char GetSpeedManagerKeyNumber2()
	{
		if (SettingsStringMapGeneralMajor["SpeedManagerKey2"] == "SHIFT")
			return 0x10;
		if (SettingsStringMapGeneralMajor["SpeedManagerKey2"] == "CTRL")
			return 0x11;
		if (SettingsStringMapGeneralMajor["SpeedManagerKey2"] == "ALT")
			return 0x12;
		return 0;
	}

	bool GetEffectEnabled() { return SettingsBoolMapGeneralMajor["EffectShaderForSpeedupOn"]; }
	bool EsmNotLoadCheckEnabled() { return SettingsBoolMapGeneralMajor["EsmNotLoadCheckOn"]; };
	bool GetWwiseEffectEnabled() { return SettingsBoolMapGeneralMinor["FxWwiseEffectOn"]; }

	double GetMultiplierMinThreshold() { return SettingsFloatMapGeneralMajor["MultiplierMinThreshold"]; }
	double GetMultiplierMaxThreshold() { return SettingsFloatMapGeneralMajor["MultiplierMaxThreshold"]; }
	double GetMultiplierDownRate() { return SettingsFloatMapGeneralMajor["MultiplierDownRate"]; }
	double GetMultiplierUpRate() { return SettingsFloatMapGeneralMajor["MultiplierUpRate"]; }
	double GetCrouchPowerRatio() { return SettingsFloatMapGeneralMajor["CrouchPowerRatio"]; }
	double GetFallspeedMultiplierMinThreshold() { return SettingsFloatMapGeneralMajor["FallspeedMultiplierMinThreshold"]; }
	double GetFallspeedMultiplierMaxThreshold() { return SettingsFloatMapGeneralMajor["FallspeedMultiplierMaxThreshold"]; }
	double GetFallspeedMultiplierDownRate() { return SettingsFloatMapGeneralMajor["FallspeedMultiplierDownRate"]; }
	double GetFallspeedMultiplierUpRate() { return SettingsFloatMapGeneralMajor["FallspeedMultiplierUpRate"]; }
	double GetJumpMultiplier() { return SettingsFloatMapGeneralMajor["FirstJumpMultiplier"]; }
	double GetSprintPowerRatio() { return SettingsFloatMapGeneralMajor["SprintPowerRatio"]; }
	int    GetSlidingBonusCount() { return SettingsIntMapGeneralMajor["SlidingBonusCount"]; }

	//GeneralMinor
	int GetTimePerFrame() { return SettingsIntMapGeneralMinor["TimePerFrame"]; }
	//Debug
	int GetLogLevel() { return SettingsIntMapDebug["LogLevel"]; }

	std::unordered_map<std::string, bool>* GetBoolMapByCategory(std::string category)
	{
		if (category == "GeneralMajor")
			return &SettingsBoolMapGeneralMajor;
		else if (category == "GeneralMinor")
			return &SettingsBoolMapGeneralMinor;
		else if (category == "Debug")
			return &SettingsBoolMapDebug;

		std::unordered_map<std::string, bool> result;
		return &result;
	}

	std::unordered_map<std::string, std::string>* GetStringMapByCategory(std::string category)
	{
		if (category == "GeneralMajor")
			return &SettingsStringMapGeneralMajor;
		else if (category == "GeneralMinor")
			return &SettingsStringMapGeneralMinor;
		else if (category == "Debug")
			return &SettingsStringMapDebug;

		std::unordered_map<std::string, std::string> result;
		return &result;
	}

	std::unordered_map<std::string, int>* GetIntMapByCategory(std::string category)
	{
		if (category == "GeneralMajor")
			return &SettingsIntMapGeneralMajor;
		else if (category == "GeneralMinor")
			return &SettingsIntMapGeneralMinor;
		else if (category == "Debug")
			return &SettingsIntMapDebug;

		std::unordered_map<std::string, int> result;
		return &result;
	}

	std::unordered_map<std::string, double>* GetFloatMapByCategory(std::string category)
	{
		if (category == "GeneralMajor")
			return &SettingsFloatMapGeneralMajor;
		else if (category == "GeneralMinor")
			return &SettingsFloatMapGeneralMinor;
		else if (category == "Debug")
			return &SettingsFloatMapDebug;

		std::unordered_map<std::string, double> result;
		return &result;
	}

	void ReadConfigBool(std::string category, std::string name)
	{
		auto map = GetBoolMapByCategory(category);
		if (tomlParseError)
			return;
		std::string value = TomlConfig[category][name].value_or("");

		if (value != "") {
			(*map)[name] = (value == "True" || value == "true");
			Debug(fmt::format("{}: Category:{}, SettingName:{}, ConfigFileValue:{}, Settings:{}", Plugin::NAME.data(), category, name, value, (*map)[name]));
		}
	}

	void ReadConfigString(std::string category, std::string name)
	{
		auto map = GetStringMapByCategory(category);
		if (tomlParseError)
			return;
		std::string value = TomlConfig[category][name].value_or("");

		if (value != "") {
			(*map)[name] = value;
			Debug(fmt::format("{}: Category:{}, SettingName:{}, ConfigFileValue:{}, Settings:{}", Plugin::NAME.data(), category, name, value, (*map)[name]));
		}
	}

	void ReadConfigInt(std::string category, std::string name)
	{
		auto map = GetIntMapByCategory(category);
		if (tomlParseError)
			return;
		std::string value = TomlConfig[category][name].value_or("");
		if (value != "") {
			try {
				(*map)[name] = stoi(value);
				Debug(fmt::format("{}: Category:{}, SettingName:{}, ConfigFileValue:{}, Settings:{}", Plugin::NAME.data(), category, name, value, (*map)[name]));
			} catch (std::exception const& e) {
				Error(fmt::format("can't parse value:{} to int in Category:{}, SettingName:{}", value, category, name));
			}
		}
	}

	void ReadConfigFloat(std::string category, std::string name)
	{
		auto map = GetFloatMapByCategory(category);
		if (tomlParseError)
			return;
		std::string value = TomlConfig[category][name].value_or("");
		if (value != "") {
			try {
				(*map)[name] = stof(value);
				Debug(fmt::format("{}: Category:{}, SettingName:{}, ConfigFileValue:{}, Settings:{}", Plugin::NAME.data(), category, name, value, (*map)[name]));
			} catch (std::exception const& e) {
				Error(fmt::format("can't parse value:{} to float in Category:{}, SettingName:{}", value, category, name));
			}
		}
	}

}
