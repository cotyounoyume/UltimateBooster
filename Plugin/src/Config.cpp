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
		}
		//DumpSettings();
		U::SetLogLevel(GetLogLevel());
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
	std::string GetTestKey() { return SettingsStringMapGeneralMajor["TestKey"]; }

	char GetTestKeyNumber()
	{
		return SettingsStringMapGeneralMajor["TestKey"].c_str()[0];
	}

	bool GetTestOn() { return SettingsBoolMapGeneralMajor["TestOn"]; }

	int GetTestNum() { return SettingsIntMapDebug["TestNum"]; }

	//GeneralMinor
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
}