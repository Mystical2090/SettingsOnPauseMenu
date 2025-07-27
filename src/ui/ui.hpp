#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;
enum SettingPage {
    Gameplay,
    Practice,
    Performance,
    Audio,
    Misc,
    Keybinds,
    Credits // i forgor
};
enum SettingCellType {
    Default,
    FMODDebug,
    SongSelect,
    SongOffset,
    Separator
};
using SearchCB = std::function<void(std::string)>;
class SearchPopup : public geode::Popup<SearchCB> {
protected:
    TextInput* m_input;
    SearchCB m_callback;
    bool setup(SearchCB) override;
    void onSearch(CCObject*);
public:
    static SearchPopup* create(SearchCB callback);
};
class SettingCell : public CCNode {
protected:
    CCMenuItemToggler* m_toggler;
    bool init(std::string name, std::string gv, SettingCellType type);
    void onCheckboxToggled(CCObject* sender);
    void onSongSelect(CCObject*);
    void onFMODDebug(CCObject*);  // Added missing declaration
    void onInfo(CCObject*);
public:
    std::string m_name;
    std::string m_gameVariable;
    SettingCellType m_type;
    static SettingCell* create(std::string name, std::string gv, SettingCellType type = SettingCellType::Default);
};
