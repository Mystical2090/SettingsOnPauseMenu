#include <Geode/Geode.hpp>
#include <Geode/modify/OptionsLayer.hpp>

#include "ui.hpp"

using namespace geode::prelude;

class $modify(OptionsLayer) {
	void onOptions(CCObject* sender) {
		SettingsLayer::create()->show();
	}
};
