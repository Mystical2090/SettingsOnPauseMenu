#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/CCMenuItemSpriteExtra.hpp>

using namespace geode::prelude;

class $modify(SettingsOnPauseMenuPleaseWork, PauseLayer) {
    void customSetup() override {
        PauseLayer::customSetup();

        auto sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn02_001.png");
        sprite->setScale(0.775f);

        auto button = CCMenuItemSpriteExtra::create(
            sprite, this, menu_selector(SettingsOnPauseMenuPleaseWork::onOptions)
        );
        button->setID("main-options"_spr);

        if (auto rightButtonMenu = this->getChildByID("right-button-menu")) {
            if (auto menu = typeinfo_cast<CCMenu*>(rightButtonMenu)) {
                menu->addChild(button);
                menu->updateLayout();
            }
        }
    }

    void onOptions(CCObject*) {
        PauseLayer::showOptions();
    }
};
