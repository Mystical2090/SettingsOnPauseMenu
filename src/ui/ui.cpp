#include "ui.hpp"
#include <unordered_set>

SearchPopup* SearchPopup::create(SearchCB callback) {
    auto ret = new SearchPopup();
    if (ret && ret->initAnchored(320.f, 200.f, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SearchPopup::setup(SearchCB callback) {
    m_callback = std::move(callback);
    
    this->setTitle("Search Settings");
    this->setID("SearchPopup"_spr);
    
    if (m_bgSprite) {
        m_bgSprite->setID("background");
        auto gradient = CCLayerGradient::create(ccc4(40, 125, 255, 255), ccc4(15, 75, 200, 255));
        gradient->setContentSize(m_bgSprite->getContentSize());
        gradient->setPosition(m_bgSprite->getPosition());
        gradient->setZOrder(-1);
        this->addChild(gradient);
    }
    
    if (m_title) {
        m_title->setID("title");
        m_title->setColor({255, 255, 255});
        m_title->setScale(0.9f);
    }
    
    if (m_buttonMenu) m_buttonMenu->setID("button-menu");
    if (m_mainLayer) m_mainLayer->setID("main-layer");
    if (m_closeBtn) {
        m_closeBtn->setID("close-button");
        m_closeBtn->setScale(0.8f);
    }

    auto inputContainer = CCNode::create();
    inputContainer->setID("input-container");
    
    auto inputBg = CCScale9Sprite::create("GJ_square01.png");
    inputBg->setContentSize({240.f, 35.f});
    inputBg->setColor({255, 255, 255});
    inputBg->setOpacity(220);
    inputContainer->addChild(inputBg);

    m_input = TextInput::create(230.f, "Type to search...");
    m_input->setID("search-input");
    m_input->setMaxCharCount(64);
    m_input->getInputNode()->setColor({50, 50, 50});
    inputContainer->addChild(m_input);
    
    m_mainLayer->addChildAtPosition(inputContainer, Anchor::Center, {0, 15.f});

    auto buttonMenu = CCMenu::create();
    buttonMenu->setID("action-buttons");

    auto searchBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Search", "bigFont.fnt", "GJ_button_01.png", 0.8f),
        this, 
        menu_selector(SearchPopup::onSearch)
    );
    searchBtn->setID("search-button");

    auto clearBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Clear", "bigFont.fnt", "GJ_button_06.png", 0.8f),
        this, 
        menu_selector(SearchPopup::onClear)
    );
    clearBtn->setID("clear-button");

    buttonMenu->addChild(searchBtn);
    buttonMenu->addChild(clearBtn);
    buttonMenu->setLayout(RowLayout::create()->setGap(20.f));
    buttonMenu->updateLayout();
    
    m_mainLayer->addChildAtPosition(buttonMenu, Anchor::Center, {0, -35.f});

    return true;
}

void SearchPopup::onSearch(CCObject* sender) {
    if (m_callback) {
        m_callback(m_input->getString());
    }
    m_closeBtn->activate();
}

void SearchPopup::onClear(CCObject* sender) {
    if (m_callback) {
        m_callback("");
    }
    m_closeBtn->activate();
}

SettingCell* SettingCell::create(std::string name, std::string gv, SettingCellType type) {
    auto ret = new SettingCell();
    if (ret && ret->init(name, gv, type)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SettingCell::init(std::string name, std::string gv, SettingCellType type) {
    if (!CCNode::init()) return false;
    
    setID(name);
    m_name = name;
    m_gameVariable = gv;
    m_type = type;

    auto nameLabel = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
    nameLabel->setID("name-label");
    nameLabel->limitLabelWidth(300.f, 0.7f, 0.4f);
    nameLabel->setColor({255, 255, 255});
    
    auto menu = CCMenu::create();
    menu->setID("button-menu");

    auto gameManager = GameManager::sharedState();

    switch (type) {
        case Separator: {
            auto separatorBg = CCScale9Sprite::create("GJ_square05.png");
            separatorBg->setContentSize({480.f, 35.f});
            separatorBg->setColor({100, 150, 255});
            separatorBg->setOpacity(180);
            this->addChild(separatorBg);
            
            auto separatorText = CCLabelBMFont::create(name.c_str(), "goldFont.fnt");
            separatorText->setID("separator-label");
            separatorText->limitLabelWidth(420.f, 0.8f, 0.1f);
            separatorText->setColor({255, 255, 255});
            this->addChildAtPosition(separatorText, Anchor::Center);
            
            this->setContentSize({480.f, 35.f});
            return true;
        }
        case Default: {
            m_toggler = CCMenuItemToggler::createWithStandardSprites(
                this,
                menu_selector(SettingCell::onCheckboxToggled),
                0.7f
            );
            m_toggler->setID("toggler");
            m_toggler->toggle(gameManager->getGameVariable(gv.c_str()));

            auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
            infoSpr->setScale(0.6f);
            auto infoBtn = CCMenuItemSpriteExtra::create(
                infoSpr, this, menu_selector(SettingCell::onInfo)
            );
            infoBtn->setID("info-button");
            
            menu->addChild(infoBtn);
            menu->addChild(m_toggler);
            menu->setLayout(RowLayout::create()->setGap(12.f)->setAxisAlignment(AxisAlignment::End));
            menu->updateLayout();
            break;
        }
        case FMODDebug: {
            auto debugBtn = CCMenuItemSpriteExtra::create(
                ButtonSprite::create("Debug", "bigFont.fnt", "GJ_button_04.png", 0.6f),
                this, 
                menu_selector(SettingCell::onFMODDebug)
            );
            debugBtn->setID("debug-button");
            menu->addChild(debugBtn);
            break;
        }
        case SongSelect: {
            auto songSpr = CCSprite::createWithSpriteFrameName("GJ_audioOnBtn_001.png");
            songSpr->setScale(0.7f);
            auto songBtn = CCMenuItemSpriteExtra::create(
                songSpr, this, menu_selector(SettingCell::onSongSelect)
            );
            songBtn->setID("song-button");
            menu->addChild(songBtn);
            break;
        }
        case SongOffset: {
            auto offsetBg = CCScale9Sprite::create("GJ_square01.png");
            offsetBg->setContentSize({80.f, 30.f});
            offsetBg->setColor({255, 255, 255});
            offsetBg->setOpacity(200);
            
            auto offsetInput = TextInput::create(75.f, "0");
            offsetInput->setCommonFilter(CommonFilter::Int);
            offsetInput->setMaxCharCount(6);
            offsetInput->setID("offset-input");
            offsetInput->setScale(0.8f);
            offsetInput->getInputNode()->setColor({50, 50, 50});

            if (gameManager->m_timeOffset != 0) {
                offsetInput->setString(fmt::format("{}", gameManager->m_timeOffset));
            }
            
            offsetInput->setCallback([this, gameManager](std::string offset) {
                auto result = geode::utils::numFromString<int>(offset);
                if (result.isOk()) {
                    gameManager->m_timeOffset = result.unwrap();
                } else {
                    gameManager->m_timeOffset = 0;
                    log::warn("Invalid offset value: {}", offset);
                }
            });
            
            auto offsetContainer = CCNode::create();
            offsetContainer->addChild(offsetBg);
            offsetContainer->addChild(offsetInput);
            menu->addChild(offsetContainer);
            break;
        }
    }

    if (type != Separator) {
        auto cellBg = CCScale9Sprite::create("GJ_square02.png");
        cellBg->setContentSize({480.f, 45.f});
        cellBg->setColor({255, 255, 255});
        cellBg->setOpacity(120);
        this->addChild(cellBg);
        
        this->addChildAtPosition(nameLabel, Anchor::Left, {20.f, 0.f});
        this->addChildAtPosition(menu, Anchor::Right, {-20.f, 0.f});
        nameLabel->setAnchorPoint({0.f, 0.5f});
        this->setContentSize({480.f, 45.f});
    }

    return true;
}

void SettingCell::onFMODDebug(CCObject* sender) {
    auto mol = MoreOptionsLayer::create();
    mol->onFMODDebug(sender);
}

void SettingCell::onSongSelect(CCObject* sender) {
    auto mol = MoreOptionsLayer::create();
    mol->onSongBrowser(sender);
    if (auto songBrowser = CCScene::get()->getChildByType<GJSongBrowser>(0)) {
        if (songBrowser->m_delegate == mol) {
            songBrowser->m_delegate = nullptr;
        }
    }
}

void SettingCell::onCheckboxToggled(CCObject* sender) {
    bool newValue = !m_toggler->isOn();
    GameManager::get()->setGameVariable(m_gameVariable.c_str(), newValue);
    log::debug("Setting gv_{} to {}", m_gameVariable, newValue);
    
    if (m_toggler) {
        m_toggler->stopAllActions();
        auto bounce = CCEaseElasticOut::create(CCScaleTo::create(0.25f, 0.7f));
        m_toggler->runAction(bounce);
    }
}

std::unordered_map<std::string, std::string> getModernSettingDescriptions() {
    return {
        {"0026", "Instantly restarts levels on death without manual input"},
        {"0052", "Reduces restart delay to 0.5 seconds for fluid gameplay"},
        {"0128", "Hides cursor during gameplay for immersive experience"},
        {"0010", "Reverses dual mode controls for alternative playstyle"},
        {"0011", "Restricts player 1 to single-side controls permanently"},
        {"0028", "Disables mouse input when using controller thumbstick"},
        {"0163", "Enables developer hotkeys: R=reset, Ctrl+R=full reset, P=hitboxes"},
        {"0024", "Displays cursor and pause controls during level play"},
        {"0135", "Removes attempt counter from level interface"},
        {"0015", "Relocates pause button to opposite screen position"},
        {"0129", "Minimizes portal visual indicators for cleaner UI"},
        {"0130", "Enhances orb visibility with additional indicators"},
        {"0140", "Disables orb scaling animations for consistent visuals"},
        {"0141", "Removes scaling effects from trigger orbs only"},
        {"0172", "Eliminates all screen shake effects globally"},
        {"0014", "Prevents death-triggered screen shake specifically"},
        {"0072", "Removes gravity change visual feedback"},
        {"0060", "Forces default icon appearance in mini gamemode"},
        {"0061", "Alternates spider teleport colors between primary/secondary"},
        {"0062", "Switches dash orb flame colors between primary/secondary"},
        {"0096", "Changes wave trail colors between primary/secondary"},
        {"0174", "Hides debug overlays during start position testing"},
        {"0071", "Removes checkpoint UI elements in practice mode"},
        {"0134", "Hides attempt tracking in practice sessions"},
        {"0027", "Auto-generates checkpoints during practice gameplay"},
        {"0068", "Increases checkpoint frequency in practice mode"},
        {"0100", "Enables death animations in practice sessions"},
        {"0125", "Synchronizes editor music with practice mode"},
        {"0166", "Visualizes collision boundaries in practice mode"},
        {"0171", "Disables player collision when showing hitboxes"},
        {"0066", "Optimizes rendering for high-object-count levels"},
        {"0108", "Automatically applies low detail mode when available"},
        {"0082", "Suppresses high object count warning dialogs"},
        {"0136", "Enhanced LDM removing glow and entrance effects"},
        {"0042", "Expands local level storage from 10 to 100 slots"},
        {"0119", "Accelerates save/load by clearing level cache"},
        {"0127", "Caches gauntlet levels to prevent re-downloads"},
        {"0155", "Disables shader anti-aliasing for performance"},
        {"0033", "Relocates custom song directory for compatibility"},
        {"0083", "Bypasses missing song download notifications"},
        {"0018", "Prevents automatic custom song cleanup"},
        {"0142", "Reduces audio sample rate to 24kHz (restart required)"},
        {"0159", "Increases audio buffer for stability (restart required)"},
        {"0094", "Expands comment display capacity per page"},
        {"0090", "Preloads comments without manual refresh"},
        {"0073", "Uses 2.1 completion criteria for level filtering"},
        {"0093", "Doubles level list capacity from 10 to 20 items"},
        {"0084", "Positions new levels at list bottom by default"},
        {"0126", "Shows precise decimal percentages instead of integers"},
        {"0099", "Displays personal leaderboard rankings on levels"},
        {"0095", "Legacy option with no functional effect"},
        {"0167", "Adds confirmation dialog before level exit"},
        {"0168", "Accelerates menu transition animations"},
        {"0040", "Toggles percentage display during gameplay"},
        {"0074", "Controls restart button visibility in pause menu"},
        {"0109", "Shows additional debug information overlay"},
        {"0113", "Inverts platformer movement controls"},
        {"0153", "Enables player explosion effects on death"},
        {"0019", "Preloads audio files into system memory"},
        {"0022", "Increases audio quality above standard settings"},
        {"0075", "Parental control: disables all comment systems"},
        {"0076", "Parental control: blocks account post features"},
        {"0077", "Parental control: removes creator search functionality"},
        {"0023", "Applies frame rate smoothing algorithms"},
        {"0065", "Optimizes object movement calculations"},
        {"0101", "Forces smooth fix activation regardless of settings"},
        {"0102", "Enables editor-specific smooth fix processing"},
        {"0056", "Suppresses high object count alerts"},
        {"0081", "Disables physics-based screen shake effects"},
        {"0067", "Improves start position placement accuracy"},
    };
}

void SettingCell::onInfo(CCObject* sender) {
    auto descriptions = getModernSettingDescriptions();
    std::string description = "This setting modifies game behavior. No detailed description available.";
    
    if (descriptions.find(m_gameVariable) != descriptions.end()) {
        description = descriptions[m_gameVariable];
    }
    
    auto alert = FLAlertLayer::create(
        m_name.c_str(),
        description.c_str(),
        "OK"
    );
    if (alert) {
        alert->m_scene = CCScene::get();
        alert->show();
    }
}

SettingsLayer* SettingsLayer::create() {
    auto ret = new SettingsLayer();
    if (ret && ret->initAnchored(720.f, 450.f)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCSprite* createModernCategorySprite(const std::string& name, bool isSelected = false) {
    auto sprite = CCScale9Sprite::create(
        isSelected ? "GJ_button_01.png" : "GJ_button_04.png"
    );
    sprite->setContentSize({140.f, 40.f});
    
    auto text = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
    text->limitLabelWidth(120.f, 0.7f, 0.1f);
    text->setColor(isSelected ? ccc3(255, 255, 255) : ccc3(200, 200, 200));
    
    sprite->addChildAtPosition(text, Anchor::Center);
    
    return sprite;
}

CCMenuItemSpriteExtra* createModernCategoryBtn(const std::string& name, CCObject* target, SettingPage page, SEL_MenuHandler callback) {
    auto btn = CCMenuItemSpriteExtra::create(
        createModernCategorySprite(name), target, callback
    );
    btn->setUserObject(CCInteger::create(page));
    btn->setID(name);
    return btn;
}

bool SettingsLayer::setup() {
    this->setID("SettingsLayer"_spr);
    this->setTitle("Advanced Settings");
    
    if (m_bgSprite) {
        m_bgSprite->setID("background");
        auto gradient = CCLayerGradient::create(ccc4(0, 100, 255, 255), ccc4(0, 50, 150, 255));
        gradient->setContentSize(m_bgSprite->getContentSize());
        gradient->setPosition(m_bgSprite->getPosition());
        gradient->setZOrder(-1);
        this->addChild(gradient);
    }
    
    if (m_buttonMenu) m_buttonMenu->setID("button-menu");
    if (m_mainLayer) m_mainLayer->setID("main-layer");
    if (m_closeBtn) {
        m_closeBtn->setID("close-button");
        m_closeBtn->setScale(0.8f);
    }
    if (m_title) {
        m_title->setColor({255, 255, 255});
        m_title->setScale(1.0f);
    }
    
    m_noElasticity = true;

    auto tabBg = CCScale9Sprite::create("GJ_square01.png");
    tabBg->setID("tab-background");
    tabBg->setContentSize({160.f, 380.f});
    tabBg->setColor({255, 255, 255});
    tabBg->setOpacity(150);

    auto tabMenu = CCMenu::create();
    tabMenu->setID("tab-menu");

    #define CATEGORY_BTN(name, page) tabMenu->addChild( \
        createModernCategoryBtn(name, this, page, menu_selector(SettingsLayer::onCategoryBtn)) \
    );
    
    CATEGORY_BTN("Gameplay", SettingPage::Gameplay)
    CATEGORY_BTN("Practice", SettingPage::Practice)
    CATEGORY_BTN("Performance", SettingPage::Performance)
    CATEGORY_BTN("Audio", SettingPage::Audio)
    CATEGORY_BTN("Misc", SettingPage::Misc)
    CATEGORY_BTN("Keys", SettingPage::Keybinds)

    tabMenu->setLayout(
        ColumnLayout::create()
            ->setAxisAlignment(AxisAlignment::Even)
            ->setGap(10.f)
    );
    tabMenu->setContentSize({140.f, 360.f});
    tabMenu->updateLayout();
    tabBg->addChildAtPosition(tabMenu, Anchor::Center);

    m_mainLayer->addChildAtPosition(tabBg, Anchor::Left, {110.f, 0.f});

    switchPage(SettingPage::Gameplay, true, 
        static_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("Gameplay")));

    auto searchContainer = CCNode::create();
    searchContainer->setID("search-container");
    
    auto searchBtnSpr = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
    searchBtnSpr->setScale(0.9f);
    auto searchBtn = CCMenuItemSpriteExtra::create(
        searchBtnSpr, this, menu_selector(SettingsLayer::onSearchBtn)
    );
    searchBtn->setID("search-button");

    auto clearBtnSpr = CCSprite::createWithSpriteFrameName("gj_findBtnOff_001.png");
    clearBtnSpr->setScale(0.9f);
    m_searchClearBtn = CCMenuItemSpriteExtra::create(
        clearBtnSpr, this, menu_selector(SettingsLayer::onClearSearch)
    );
    m_searchClearBtn->setID("clear-search-button");
    m_searchClearBtn->setVisible(false);

    auto searchMenu = CCMenu::create();
    searchMenu->addChild(searchBtn);
    searchMenu->addChild(m_searchClearBtn);
    searchMenu->setLayout(RowLayout::create()->setGap(10.f));
    searchMenu->updateLayout();
    searchContainer->addChild(searchMenu);
    
    m_mainLayer->addChildAtPosition(searchContainer, Anchor::TopRight, {-90.f, -40.f});

    return true;
}

void SettingsLayer::onClearSearch(CCObject* sender) {
    auto page = static_cast<SettingPage>(
        static_cast<CCInteger*>(m_currentBtn->getUserObject())->getValue()
    );
    switchPage(page, false, m_currentBtn);
    m_searchClearBtn->setVisible(false);
}

void SettingsLayer::onSearchBtn(CCObject* sender) {
    SearchPopup::create([this](std::string query) {
        performSearch(query);
    })->show();
}

void SettingsLayer::performSearch(std::string query) {
    using namespace geode::utils::string;
    
    if (query.empty()) {
        onClearSearch(nullptr);
        return;
    }

    auto currentPage = static_cast<SettingPage>(
        static_cast<CCInteger*>(m_currentBtn->getUserObject())->getValue()
    );
    
    auto allSettings = CCArray::create();
    
    for (int i = 0; i < 6; i++) {
        switchPage(static_cast<SettingPage>(i), false, m_currentBtn);
        for (auto cell : CCArrayExt<SettingCell*>(m_listItems)) {
            if (cell->m_type != SettingCellType::Separator) {
                allSettings->addObject(cell);
            }
        }
    }
    
    auto filteredSettings = CCArray::create();
    std::string lowerQuery = toLower(query);
    
    for (auto cell : CCArrayExt<SettingCell*>(allSettings)) {
        if (toLower(cell->m_name).find(lowerQuery) != std::string::npos) {
            filteredSettings->addObject(
                SettingCell::create(cell->m_name, cell->m_gameVariable, cell->m_type)
            );
        }
    }
    
    m_listItems = filteredSettings;
    m_searchClearBtn->setVisible(true);
    
    if (m_border) {
        m_border->removeFromParent();
    }
    this->refreshList();
}

void SettingsLayer::onCategoryBtn(CCObject* sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
    auto page = static_cast<SettingPage>(
        static_cast<CCInteger*>(btn->getUserObject())->getValue()
    );
    switchPage(page, false, btn);
}

void SettingsLayer::switchPage(SettingPage page, bool isFirstRun, CCMenuItemSpriteExtra* btn) {
    m_listItems = CCArray::create();
    
    if (m_searchClearBtn) {
        m_searchClearBtn->setVisible(false);
    }
    
    #define SETTING(name, gv) m_listItems->addObject( \
        SettingCell::create(name, gv, SettingCellType::Default) \
    );
    #define SETTING_WITH_TYPE(name, type) m_listItems->addObject( \
        SettingCell::create(name, "", type) \
    );
    #define SEPARATOR(text) m_listItems->addObject( \
        SettingCell::create(text, "", SettingCellType::Separator) \
    );

    switch (page) {
        case Gameplay:
            SEPARATOR("Core Mechanics")
            SETTING("Auto Retry", "0026")
            SETTING("Fast Reset", "0052")
            SETTING("Show Percent", "0040")
            SETTING("Restart Button", "0074")
            SETTING("Confirm Exit", "0167")
            SETTING("Extra Info", "0109")
            
            SEPARATOR("Control Systems")
            SETTING("Show Cursor In-Game", "0024")
            SETTING("Flip 2P Controls", "0010")
            SETTING("Always Limit Controls", "0011")
            SETTING("Disable Thumbstick", "0028")
            SETTING("Flip Plat. Controls", "0113")
            SETTING("Quick Keys", "0163")
            SETTING("Flip Pause Button", "0015")
            
            SEPARATOR("Visual Interface")
            SETTING("Decimal Percent", "0126")
            SETTING("Hide Attempts", "0135")
            SETTING("Orb Labels", "0130")
            SETTING("Hide Playtest Text", "0174")
            
            SEPARATOR("Player Visuals")
            SETTING("Explode Player on Death", "0153")
            SETTING("Default Mini Icon", "0060")
            SETTING("Switch Spider Teleport Color", "0061")
            SETTING("Switch Dash Fire Color", "0062")
            SETTING("Switch Wave Trail Color", "0096")
            break;
            
        case Practice:
            SEPARATOR("Checkpoint System")
            SETTING("Auto Checkpoints", "0027")
            SETTING("Quick Checkpoint Mode", "0068")
            SETTING("High Start Position Accuracy", "0067")
            
            SEPARATOR("Practice Interface")
            SETTING("Hide Practice Button", "0071")
            SETTING("Hide Attempts in Practice", "0134")
            SETTING("Practice Death Effect", "0100")
            SETTING("Show Hitboxes", "0166")
            SETTING("Disable Player Hitbox", "0171")
            break;
            
        case Performance:
            SEPARATOR("Rendering Optimization")
            SETTING("Smooth Fix", "0023")
            SETTING("Move Optimization", "0065")
            SETTING("Force Smooth Fix", "0101")
            SETTING("Smooth Fix in Editor", "0102")
            SETTING("High Capacity Mode", "0066")
            
            SEPARATOR("Low Detail Mode")
            SETTING("Auto LDM", "0108")
            SETTING("Extra LDM", "0136")
            SETTING("Disable Shader Anti-Aliasing", "0155")
            
            SEPARATOR("System Enhancement")
            SETTING("Increase Max Levels", "0042")
            SETTING("Save Gauntlet Levels", "0127")
            SETTING("Increase Local Levels Per Page", "0093")
            SETTING("Lock Cursor In-Game", "0128")
            
            SEPARATOR("Effect Toggles")
            SETTING("Disable Explosion Shake", "0014")
            SETTING("Disable Orb Scale", "0140")
            SETTING("Disable Trigger Orb Scale", "0141")
            SETTING("Disable Shake Effect", "0081")
            SETTING("Disable High Object Alert", "0082")
            SETTING("Disable Object Alert", "0056")
            break;
            
        case Audio:
            SEPARATOR("Audio Quality")
            SETTING("Higher Audio Quality", "0022")
            SETTING("Reduce Audio Quality", "0142")
            SETTING("Audio Fix 01", "0159")
            
            SEPARATOR("Song Management")
            SETTING("Load Songs into Memory", "0019")
            SETTING("Change Song Path", "0033")
            SETTING("No Song Limit", "0018")
            SETTING("Disable Song Alert", "0083")
            SETTING("Normal Music in Editor", "0125")
            
            SEPARATOR("Audio Tools")
            SETTING_WITH_TYPE("FMOD Debug", SettingCellType::FMODDebug)
            SETTING_WITH_TYPE("Local Songs", SettingCellType::SongSelect)
            SETTING_WITH_TYPE("Song Offset (MS)", SettingCellType::SongOffset)
            break;
            
        case Misc:
            SEPARATOR("User Interface")
            SETTING("Fast Menu", "0168")
            SETTING("Show Leaderboard Percent", "0099")
            SETTING("Manual Level Order", "0084")
            SETTING("New Completed Filter", "0073")
            SETTING("Do Not...", "0095")
            
            SEPARATOR("Comments System")
            SETTING("Autoload Comments", "0090")
            SETTING("More Comments Mode", "0094")
            
            SEPARATOR("Parental Controls")
            SETTING("Disable Comments", "0075")
            SETTING("Disable Account Comments", "0076")
            SETTING("Featured Levels Only", "0077")
            
            SEPARATOR("Visual Effects")
            SETTING("Disable Gravity Effect", "0072")
            break;
            
        case Keybinds:
            #ifndef GEODE_IS_IOS
            auto mol = MoreOptionsLayer::create();
            mol->onKeybindings(btn);
            #endif
            SEPARATOR("Keybind Configuration")
            SEPARATOR("Use the Options menu for full keybind customization")
            break;
    }
    
    if (!isFirstRun && m_border) {
        m_border->removeFromParent();
    }
    
    refreshList();
    
    if (m_currentBtn) {
        m_currentBtn->setSprite(createModernCategorySprite(m_currentBtn->getID(), false));
    }
    if (btn) {
        btn->setSprite(createModernCategorySprite(btn->getID(), true));
        m_currentBtn = btn;
    }
}

void SettingsLayer::refreshList() {
    auto listView = ListView::create(m_listItems, 45.f, 520.f, 360.f);
    listView->setID("list-view");
    
    if (listView->m_tableView) {
        listView->m_tableView->setID("table-view");
        if (listView->m_tableView->m_contentLayer) {
            listView->m_tableView->m_contentLayer->setID("content-layer");
        }
    }
    
    auto listBg = CCScale9Sprite::create("GJ_square01.png");
    listBg->setContentSize({520.f, 360.f});
    listBg->setColor({255, 255, 255});
    listBg->setOpacity(140);
    
    m_border = CCNode::create();
    m_border->setID("list-container");
    m_border->addChild(listBg);
    m_border->addChild(listView);
    m_border->setContentSize({520.f, 360.f});
    
    listView->setPosition({260.f, 180.f});
    listBg->setPosition({260.f, 180.f});
    
    m_border->ignoreAnchorPointForPosition(false);
    m_mainLayer->addChildAtPosition(m_border, Anchor::Right, {-180.f, 0.f});
}
