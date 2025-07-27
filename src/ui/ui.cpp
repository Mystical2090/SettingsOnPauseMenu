#include "ui.hpp"

SearchPopup* SearchPopup::create(SearchCB callback) {
    auto ret = new SearchPopup();
    if (ret && ret->initAnchored(230.f, 140.f, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool SearchPopup::setup(SearchCB callback) {
    m_callback = callback;
    this->setTitle("Search Settings");
    this->setID("SearchPopup"_spr);
    this->m_bgSprite->setID("background");
    this->m_title->setID("title");
    this->m_buttonMenu->setID("button-menu");
    this->m_mainLayer->setID("main-layer");
    this->m_closeBtn->setID("close-button");

    m_input = TextInput::create(170.f, "Enter search term...");
    m_input->setID("search-input");
    m_input->setMaxCharCount(50);
    m_mainLayer->addChildAtPosition(m_input, Anchor::Center, {0, 10.f});

    auto searchBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Search", "goldFont.fnt", "GJ_button_01.png", 0.8f), 
        this, 
        menu_selector(SearchPopup::onSearch)
    );
    searchBtn->setID("search-button");
    
    auto clearBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Clear", "goldFont.fnt", "GJ_button_02.png", 0.8f), 
        this, 
        menu_selector(SearchPopup::onClear)
    );
    clearBtn->setID("clear-button");

    auto buttonMenu = CCMenu::create();
    buttonMenu->addChild(searchBtn);
    buttonMenu->addChild(clearBtn);
    buttonMenu->setLayout(RowLayout::create()->setGap(10.f));
    buttonMenu->updateLayout();
    buttonMenu->setID("search-buttons");
    
    m_mainLayer->addChildAtPosition(buttonMenu, Anchor::Center, {0, -25.f});

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
    nameLabel->limitLabelWidth(180.f, 0.9f, 0.5f);
    
    auto menu = CCMenu::create();
    menu->setID("button-menu");

    auto gameManager = GameManager::sharedState();

    switch (type) {
        case Default: {
            m_toggler = CCMenuItemToggler::createWithStandardSprites(
                this,
                menu_selector(SettingCell::onCheckboxToggled),
                0.75f
            );
            m_toggler->setID("toggler");
            m_toggler->toggle(gameManager->getGameVariable(gv.c_str()));

            auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
            infoSpr->setScale(0.65f);
            auto infoBtn = CCMenuItemSpriteExtra::create(
                infoSpr, this, menu_selector(SettingCell::onInfo)
            );
            infoBtn->setID("info-button");
            
            menu->addChild(infoBtn);
            menu->addChild(m_toggler);
            menu->setLayout(RowLayout::create()->setGap(15.f)->setAxisAlignment(AxisAlignment::End));
            menu->updateLayout();
            break;
        }
        case FMODDebug: {
            auto debugBtn = CCMenuItemSpriteExtra::create(
                ButtonSprite::create("Debug", "goldFont.fnt", "GJ_button_05.png", 0.6f),
                this, 
                menu_selector(SettingCell::onFMODDebug)
            );
            debugBtn->setID("debug-button");
            menu->addChild(debugBtn);
            break;
        }
        case SongSelect: {
            auto songSpr = CCSprite::createWithSpriteFrameName("GJ_savedSongsBtn_001.png");
            songSpr->setScale(0.5f);
            auto songBtn = CCMenuItemSpriteExtra::create(
                songSpr, this, menu_selector(SettingCell::onSongSelect)
            );
            songBtn->setID("song-button");
            menu->addChild(songBtn);
            break;
        }
        case SongOffset: {
            auto offsetInput = TextInput::create(80.f, "0");
            offsetInput->setCommonFilter(CommonFilter::Int);
            offsetInput->setMaxCharCount(6);
            offsetInput->setID("offset-input");
            offsetInput->setScale(0.8f);

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
            
            menu->addChild(offsetInput);
            break;
        }
        case Separator: {
            nameLabel->setOpacity(0.f);
            auto separatorText = CCLabelBMFont::create(name.c_str(), "goldFont.fnt");
            separatorText->setID("separator-label");
            separatorText->limitLabelWidth(320.f, 0.8f, 0.1f);
            separatorText->setColor({255, 215, 0}); // Gold color
            this->addChildAtPosition(separatorText, Anchor::Center);
            break;
        }
    }

    if (type != Separator) {
        this->addChildAtPosition(nameLabel, Anchor::Left, {15.f, 0.f});
        this->addChildAtPosition(menu, Anchor::Right, {-20.f, 0.f});
        nameLabel->setAnchorPoint({0.f, 0.5f});
    }

    this->setContentSize({380.f, 32.f});
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

    auto particle = CCParticleSystemQuad::create("explodeEffect.plist");
    if (particle) {
        particle->setPosition(m_toggler->getPosition());
        particle->setScale(0.3f);
        particle->setAutoRemoveOnFinish(true);
        this->addChild(particle);
    }
}

std::unordered_map<std::string, std::string> getSettingDescriptions() {
    return {
        {"0026", "Automatically restarts the level when you die, saving you a click."},
        {"0052", "Reduces restart delay from 1.0s to 0.5s for faster gameplay."},
        {"0128", "Locks and hides your cursor during gameplay for better focus."},
        {"0010", "Swaps player 1 and player 2 controls in dual mode."},
        {"0011", "Forces player 1 controls to one side even when dual mode is off."},
        {"0028", "Prevents mouse movement when using controller thumbstick."},
        {"0163", "Enables temporary keybinds: R for reset, Ctrl+R for full reset, P for hitboxes."},
        {"0024", "Shows cursor and pause button while playing levels."},
        {"0135", "Hides the attempt counter to reduce UI clutter."},
        {"0015", "Moves the pause button to the opposite side of the screen."},
        {"0129", "Removes extra visual indicators on portal objects."},
        {"0130", "Adds helpful indicators to orb objects."},
        {"0140", "Disables the scaling animation effect on all orbs."},
        {"0141", "Disables scaling effect only on trigger orbs."},
        {"0172", "Removes screen shake effects throughout the game."},
        {"0014", "Prevents screen shake when your player dies."},
        {"0072", "Removes the visual effect when gravity changes."},
        {"0060", "Uses default icon appearance in mini mode."},
        {"0061", "Changes spider teleport effect color between main and secondary."},
        {"0062", "Changes dash orb fire effect color between main and secondary."},
        {"0096", "Changes wave mode trail color between main and secondary."},
        {"0174", "Hides debug text when using start positions or ignore damage."},
        {"0071", "Hides checkpoint placement buttons in practice mode."},
        {"0134", "Hides attempt counter specifically in practice mode."},
        {"0027", "Automatically places checkpoints as you progress in practice."},
        {"0068", "Places checkpoints more frequently in practice mode."},
        {"0100", "Shows death effects even in practice mode."},
        {"0125", "Plays normal level music in sync with editor levels."},
        {"0166", "Displays hitboxes while in practice mode for debugging."},
        {"0171", "Disables player hitbox when other hitboxes are shown."},
        {"0066", "Increases rendering capacity for complex levels (may affect performance)."},
        {"0108", "Automatically enables Low Detail Mode on supported levels."},
        {"0082", "Removes warnings about levels with high object counts."},
        {"0136", "Enhanced LDM that removes glow and enter effects."},
        {"0042", "Increases local level storage from 10 to 100 levels."},
        {"0119", "Faster saving but requires re-downloading levels each session."},
        {"0127", "Saves gauntlet levels locally to avoid re-downloading."},
        {"0155", "Disables anti-aliasing on shader effects for performance."},
        {"0033", "Changes custom song storage location (may fix song issues)."},
        {"0083", "Removes alerts when starting levels without downloaded songs."},
        {"0018", "Prevents automatic deletion of custom songs."},
        {"0142", "Reduces audio quality from 44.1kHz to 24kHz (restart required)."},
        {"0159", "Increases audio buffer size to fix audio issues (restart required)."},
    };
}

void SettingCell::onInfo(CCObject* sender) {
    auto descriptions = getSettingDescriptions();
    std::string description = "No description available for this setting.";
    
    if (descriptions.find(m_gameVariable) != descriptions.end()) {
        description = descriptions[m_gameVariable];
    }
    
    auto alert = FLAlertLayer::create(
        m_name.c_str(),
        description.c_str(),
        "OK"
    );
    alert->m_scene = CCScene::get();
    alert->show();
}

SettingsLayer* SettingsLayer::create() {
    auto ret = new SettingsLayer();
    if (ret && ret->initAnchored(520.f, 300.f)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCSprite* createCategoryBtnSprite(std::string name, bool isSelected = false) {
    auto sprite = CCSprite::createWithSpriteFrameName(
        isSelected ? "GJ_longBtn02_001.png" : "GJ_longBtn01_001.png"
    );
    auto text = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
    text->limitLabelWidth(85.f, 0.75f, 0.1f);
    sprite->setScale(0.9f);
    sprite->addChildAtPosition(text, Anchor::Center, {0, 1.5f});

    if (isSelected) {
        text->setColor({255, 255, 150});
    }
    
    return sprite;
}

CCMenuItemSpriteExtra* createCategoryBtn(std::string name, CCObject* target, SettingPage page, SEL_MenuHandler callback) {
    auto btn = CCMenuItemSpriteExtra::create(
        createCategoryBtnSprite(name), target, callback
    );
    btn->setUserObject(CCInteger::create(page));
    btn->setID(name);
    return btn;
}

bool SettingsLayer::setup() {
    this->setID("SettingsLayer"_spr);
    this->setTitle("Advanced Settings");
    this->m_bgSprite->setID("background");
    this->m_buttonMenu->setID("button-menu");
    this->m_mainLayer->setID("main-layer");
    this->m_closeBtn->setID("close-button");
    m_noElasticity = true;

    auto tabBg = CCScale9Sprite::create("square02b_001.png");
    tabBg->setID("tab-background");
    tabBg->setContentSize({110.f, 260.f});
    tabBg->setColor({20, 20, 25});
    tabBg->setOpacity(180);

    auto tabMenu = CCMenu::create();
    tabMenu->setID("tab-menu");
    #define CATEGORY_BTN(name, page) tabMenu->addChild( \
        createCategoryBtn(name, this, page, menu_selector(SettingsLayer::onCategoryBtn)) \
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
            ->setGap(5.f)
    );
    tabMenu->setContentSize(tabBg->getContentSize());
    tabMenu->updateLayout();
    tabBg->addChildAtPosition(tabMenu, Anchor::Center);

    m_mainLayer->addChildAtPosition(tabBg, Anchor::Left, {75.f, 0.f});

    switchPage(SettingPage::Gameplay, true, 
        static_cast<CCMenuItemSpriteExtra*>(this->getChildByIDRecursive("Gameplay")));

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
    searchMenu->setLayout(RowLayout::create()->setGap(5.f));
    searchMenu->updateLayout();
    searchMenu->setID("search-menu");
    
    m_mainLayer->addChildAtPosition(searchMenu, Anchor::TopRight, {-15.f, -15.f});

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
    
    // Temporarily switch to each page to collect all settings
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
            SEPARATOR("Basic Controls")
            SETTING("Auto Retry", "0026")
            SETTING("Fast Reset", "0052")
            SETTING("Show Cursor In-Game", "0024")
            SETTING("Show Percent", "0040")
            SETTING("Restart Button", "0074")
            SETTING("Confirm Exit", "0167")
            
            SEPARATOR("Advanced Controls")
            SETTING("Flip 2P Controls", "0010")
            SETTING("Always Limit Controls", "0011")
            SETTING("Disable Thumbstick", "0028")
            SETTING("Flip Plat. Controls", "0113")
            SETTING("Quick Keys", "0163")
            SETTING("Flip Pause Button", "0015")
            
            SEPARATOR("Visual Options")
            SETTING("Decimal Percent", "0126")
            SETTING("Hide Attempts", "0135")
            SETTING("Extra Info", "0109")
            SETTING("Orb Labels", "0130")
            SETTING("Hide Playtest Text", "0174")
            
            SEPARATOR("Player Appearance")
            SETTING("Explode Player on Death", "0153")
            SETTING("Default Mini Icon", "0060")
            SETTING("Switch Spider Teleport Color", "0061")
            SETTING("Switch Dash Fire Color", "0062")
            SETTING("Switch Wave Trail Color", "0096")
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
            SETTING("Normal Music in Editor", "0125")
            
            SEPARATOR("Audio Tools")
            SETTING_WITH_TYPE("FMOD Debug", SettingCellType::FMODDebug)
            SETTING_WITH_TYPE("Local Songs", SettingCellType::SongSelect)
            SETTING_WITH_TYPE("Song Offset (MS)", SettingCellType::SongOffset)
            break;

        default:
            SEPARATOR("Coming Soon!")
            break;
    }
    
    if (!isFirstRun && m_border) {
        m_border->removeFromParent();
    }
    
    refreshList();

    if (m_currentBtn) {
        m_currentBtn->setSprite(createCategoryBtnSprite(m_currentBtn->getID(), false));
    }
    if (btn) {
        btn->setSprite(createCategoryBtnSprite(btn->getID(), true));
        m_currentBtn = btn;
    }
}

void SettingsLayer::refreshList() {
    auto listView = ListView::create(m_listItems, 32.f, 380.f, 260.f);
    listView->setID("list-view");
    
    m_border = Border::create(listView, {30, 30, 35, 200}, {380.f, 260.f});
    m_border->setID("list-border");
   
    if (auto borderSprite = typeinfo_cast<CCScale9Sprite*>(
        m_border->getChildByID("geode.loader/border_sprite"))) {
        borderSprite->setColor({40, 40, 50});
        borderSprite->setOpacity(150);
    }
    
    m_border->ignoreAnchorPointForPosition(false);
    m_mainLayer->addChildAtPosition(m_border, Anchor::Right, {-190.f, 0.f});
}
