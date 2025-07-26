#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
using namespace geode::prelude;

class $modify(SettingsOnPauseMenuPleaseWork, PauseLayer) {
    void customSetup() override {
        PauseLayer::customSetup();
        auto sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        sprite->setScale(0.79f);
        auto button = CCMenuItemSpriteExtra::create(
            sprite, this, menu_selector(SettingsOnPauseMenuPleaseWork::onOptions)
        );
        button->setID("main-options"_spr);
        if (auto leftButtonMenu = this->getChildByID("left-button-menu")) { // left or right hmmmmmmmmmmmm
            if (auto menu = typeinfo_cast<CCMenu*>(leftButtonMenu)) {
                menu->addChild(button);
                menu->updateLayout();
            }
        }
    }
    
    void onOptions(CCObject*) {
        auto optionsLayer = OptionsLayer::create();
        optionsLayer->setZOrder(6969696969); // ZOrder because it need to be above everything
        optionsLayer->showLayer(false);
    }
};
