#include "Event.h"
#include "Config.h"

namespace Events
{
	REL::Relocation<__int64 (*)(double, char*, ...)> ExecuteCommand{ REL::ID(166307) };  // From Console-Command-Runner-SF

	RE::BSEventNotifyControl EventHandlerForMenu::ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
	{
		Info(format("Menu: name:{}, opening:{}", a_event.menuName.c_str(), a_event.opening));
		//for future release
		if (a_event.menuName == "MainMenu" and !a_event.opening) {
			mainMenuClosed = true;
		}
		if (a_event.menuName == "PauseMenu" and a_event.opening) {
			Config::ReadIni();
		}
		if (a_event.menuName == "LoadingMenu" and !a_event.opening) {
			if (mainMenuClosed) {
				NeedReset = true;
			}
		}
		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl EventHandlerForContainer::ProcessEvent(const RE::TESContainerChangedEvent& a_event, RE::BSTEventSource<RE::TESContainerChangedEvent>*)
	{
		//for future release
		return RE::BSEventNotifyControl::kContinue;
	}
}