
#include "config_settings.h"
#include "common_utils.h"
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/file_access.hpp>

/// 
/// ConfigSetting
/// The setting object for one entry in the settings collection
///
//////////////////////////////////////////////////////////////////////////////////////

void ConfigSetting::_bind_methods()
{
    DECLARE_PROPERTY(ConfigSetting, Type, type, Variant::INT);
    DECLARE_PROPERTY(ConfigSetting, Bool, value, Variant::BOOL);
    DECLARE_PROPERTY(ConfigSetting, Int, value, Variant::INT);
    DECLARE_PROPERTY(ConfigSetting, Float, value, Variant::FLOAT);
    DECLARE_PROPERTY(ConfigSetting, String, value, Variant::STRING);

    ClassDB::bind_method(D_METHOD("mark"), &ConfigSetting::mark);
    ClassDB::bind_method(D_METHOD("restore"), &ConfigSetting::restore);
    ClassDB::bind_method(D_METHOD("resetChange"), &ConfigSetting::resetChange);
    ClassDB::bind_method(D_METHOD("hasChanged"), &ConfigSetting::hasChanged);

    BIND_ENUM_CONSTANT(T_EMPTY);
    BIND_ENUM_CONSTANT(T_BOOL);
    BIND_ENUM_CONSTANT(T_INT);
    BIND_ENUM_CONSTANT(T_FLOAT);
    BIND_ENUM_CONSTANT(T_STRING);
}

ConfigSetting::ConfigSetting() : type_(ConfigValueType::T_EMPTY), changed_(false),
    bool_(false), int_(0), float_(0.0), string_()
{
}

void ConfigSetting::mark()
{
    // depends on type
    switch (type_) {
    case ConfigValueType::T_BOOL:
        bool_.mark();
        break;
    case ConfigValueType::T_INT:
        int_.mark();
        break;
    case ConfigValueType::T_FLOAT:
        float_.mark();
        break;
    case ConfigValueType::T_STRING:
        string_.mark();
        break;
    }
}

void ConfigSetting::restore()
{
    // if we didn't change anything, then don't do this
    if (!changed_)
        return;
    // depends on type
    changed_ = false;
    switch (type_) {
    case ConfigValueType::T_BOOL:
        bool_.restore();
        break;
    case ConfigValueType::T_INT:
        int_.restore();
        break;
    case ConfigValueType::T_FLOAT:
        float_.restore();
        break;
    case ConfigValueType::T_STRING:
        string_.restore();
        break;
    }
}

void ConfigSetting::resetChange()
{
    changed_ = false;
    switch (type_) {
    case ConfigValueType::T_BOOL:
        bool_.resetChange();
        break;
    case ConfigValueType::T_INT:
        int_.resetChange();
        break;
    case ConfigValueType::T_FLOAT:
        float_.resetChange();
        break;
    case ConfigValueType::T_STRING:
        string_.resetChange();
        break;
    }
}

void ConfigSetting::setBool(const bool value)
{
    bool_ = value;
    changed_ = bool_.hasChanged();
}

void ConfigSetting::setInt(const int64_t value)
{
    int_ = value;
    changed_ = int_.hasChanged();
}

void ConfigSetting::setFloat(const double value)
{
    float_ = value;
    changed_ = float_.hasChanged();
}

void ConfigSetting::setStdString(const std::string& value)
{
    string_ = value;
    changed_ = string_.hasChanged();
}

void ConfigSetting::setString(String value)
{
    setStdString(translate(value));
}

void ConfigSetting::copyFrom(ConfigSetting* source)
{
    if (source == nullptr)
        return;

    // enable undo if it's the same type
    if(type_ == source->type_) {
        mark();
        switch (type_)
        {
        case ConfigValueType::T_BOOL:
            bool_ = source->bool_;
            changed_ = bool_.hasChanged();
            break;
        case ConfigValueType::T_INT:
            int_ = source->int_;
            changed_ = int_.hasChanged();
            break;
        case ConfigValueType::T_FLOAT:
            float_ = source->float_;
            changed_ = float_.hasChanged();
            break;
        case ConfigValueType::T_STRING:
            string_ = source->string_;
            changed_ = string_.hasChanged();
            break;
        }
    } else {
        type_ = source->type_;
        changed_ = true;
        switch (type_)
        {
        case ConfigValueType::T_BOOL:
            bool_ = source->bool_;
            break;
        case ConfigValueType::T_INT:
            int_ = source->int_;
            break;
        case ConfigValueType::T_FLOAT:
            float_ = source->float_;
            break;
        case ConfigValueType::T_STRING:
            string_ = source->string_;
            break;
        }
    }
}

ConfigSetting* ConfigSetting::create()
{
    ConfigSetting* item = memnew(ConfigSetting);
    return item;
}

ConfigSetting* ConfigSetting::create(bool initial_value)
{
    auto item = create();
    item->type_ = ConfigValueType::T_BOOL;
    item->bool_ = initial_value;
    return item;
}   

ConfigSetting* ConfigSetting::create(int64_t initial_value)
{
    auto item = create();
    item->type_ = ConfigValueType::T_INT;
    item->int_ = initial_value;
    return item;
}   

ConfigSetting* ConfigSetting::create(double initial_value)
{
    auto item = create();
    item->type_ = ConfigValueType::T_FLOAT;
    item->float_ = initial_value;
    return item;
}   

ConfigSetting* ConfigSetting::create_(const std::string& initial_value)
{
    auto item = create();
    item->type_ = ConfigValueType::T_STRING;
    item->string_ = initial_value;
    return item;
}   

ConfigSetting* ConfigSetting::create(String initial_value)
{
    std::string s(initial_value.utf8().get_data());
    return create_(s);
}   


/// 
/// ConfigSettings
/// A collection of setting objects
///
//////////////////////////////////////////////////////////////////////////////////////

void ConfigSettings::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("markAll"), &ConfigSettings::markAll);
    ClassDB::bind_method(D_METHOD("hasChanges"), &ConfigSettings::hasChanges);

    ClassDB::bind_method(D_METHOD("applyChanges"), &ConfigSettings::applyChanges);
    ClassDB::bind_method(D_METHOD("undoPendingChanges"), &ConfigSettings::undoPendingChanges);

    ClassDB::bind_method(D_METHOD("addSetting", "name", "setting"), &ConfigSettings::addSetting);
    ClassDB::bind_method(D_METHOD("updateFrom", "source"), &ConfigSettings::updateFrom);

    ClassDB::bind_method(D_METHOD("addBoolSetting", "name", "value"), &ConfigSettings::addBoolSetting);
    ClassDB::bind_method(D_METHOD("addIntSetting", "name", "value"), &ConfigSettings::addIntSetting);
    ClassDB::bind_method(D_METHOD("addFloatSetting", "name", "value"), &ConfigSettings::addFloatSetting);
    ClassDB::bind_method(D_METHOD("addStringSetting", "name", "value"), &ConfigSettings::addStringSetting);

    ClassDB::bind_method(D_METHOD("readBoolSetting", "defaultValue"), &ConfigSettings::readBoolSetting);
    ClassDB::bind_method(D_METHOD("readIntSetting", "defaultValue"), &ConfigSettings::readIntSetting);
    ClassDB::bind_method(D_METHOD("readFloatSetting", "defaultValue"), &ConfigSettings::readFloatSetting);
    ClassDB::bind_method(D_METHOD("readStringSetting", "defaultValue"), &ConfigSettings::readStringSetting);

    ClassDB::bind_method(D_METHOD("getSettings"), &ConfigSettings::getSettings);

    ADD_SIGNAL(MethodInfo("apply_setting", PropertyInfo(Variant::STRING, "setting_name"), PropertyInfo(Variant::OBJECT, "setting")));
}

ConfigSetting* ConfigSettings::getSetting(String name)
{
    return getSetting_(translate(name));
}

ConfigSettings::~ConfigSettings()
{
    for (auto& item : settings_) {
        memdelete(item.second);
    }
}

ConfigSetting* ConfigSettings::getSetting_(const std::string& name, bool autoCreate)
{
    auto iter = settings_.find(name);
    if (iter != settings_.end())
        return iter->second;

    if(!autoCreate) {
        return nullptr;
    }

    ConfigSetting* item = memnew(ConfigSetting);
    settings_[name] = item;
    return item;
}

Dictionary ConfigSettings::getSettings() const
{
    Dictionary result;

    for (auto& item : settings_) {
        result[String(item.first.data())] = item.second;
    }

    return result;
}

void ConfigSettings::markAll()
{
    for (auto& item : settings_) {
        (item.second)->mark();
    }
}

bool ConfigSettings::hasChanges() const
{
    for (auto& item : settings_) {
        if ((item.second)->hasChanged()) {
            return true;
        }
    }

    return false;
}

template<typename T, ConfigSetting::ConfigValueType U>
T ConfigSettings::get(const std::string& name, T defaultValue)
{
    ConfigSetting* setting = getSetting_(name, false);
    if(setting != nullptr) {
        auto dataType = static_cast<ConfigSetting::ConfigValueType>(setting->getType());
        if(dataType == U) {
            return static_cast<T>(*setting);
        }
    }
    
    if(setting != nullptr) {
        remove(name);
    }
    add(name, defaultValue);
    return defaultValue;
}


ConfigSetting* ConfigSettings::addSetting(String name, ConfigSetting* setting)
{
    return add(translate(name), setting);
}
ConfigSetting* ConfigSettings::addBoolSetting(String name, bool value)
{
    return add(translate(name), ConfigSetting::create(value));
}
ConfigSetting* ConfigSettings::addIntSetting(String name, int64_t value)
{
    return add(translate(name), ConfigSetting::create(value));
}
ConfigSetting* ConfigSettings::addFloatSetting(String name, double value)
{
    return add(translate(name), ConfigSetting::create(value));
}
ConfigSetting* ConfigSettings::addStringSetting(String name, String value)
{
    return add(translate(name), ConfigSetting::create(value));
}

bool ConfigSettings::readBoolSetting(String name, bool defaultValue)
{
    return get<bool, ConfigSetting::T_BOOL>(translate(name), defaultValue);
}
int64_t ConfigSettings::readIntSetting(String name, int64_t defaultValue)
{
    return get<int64_t, ConfigSetting::T_INT>(translate(name), defaultValue);
}
double ConfigSettings::readFloatSetting(String name, double defaultValue)
{
    return get<double, ConfigSetting::T_FLOAT>(translate(name), defaultValue);
}
String ConfigSettings::readStringSetting(String name, String defaultValue)
{
    auto result = get<std::string, ConfigSetting::T_STRING>(translate(name), translate(defaultValue));
    return result.data();
}

ConfigSetting* ConfigSettings::add(const std::string& name, ConfigSetting* setting)
{
    // note that the item passed in WILL NOT be added if the
    // 'name' entry is already occupied!  instead, it will
    // be deallocated!  watch out!
    auto it = settings_.find(name);
    if(it == settings_.end()) {
        settings_[name] = setting;
    } else {
        // same type. update the value of the existing one
        if(it->second->getType() == setting->getType()) {
            switch(static_cast<ConfigSetting::ConfigValueType>(setting->getType())) {
            case ConfigSetting::T_BOOL:
                it->second->setBool(setting->getBool());
                break;
            case ConfigSetting::T_INT:
                it->second->setInt(setting->getInt());
                break;
            case ConfigSetting::T_FLOAT:
                it->second->setFloat(setting->getFloat());
                break;
            case ConfigSetting::T_STRING:
                it->second->setStdString(setting->getStdString());
                break;
            }
        }
        memdelete(setting);
    }

    return settings_[name];
}
ConfigSetting* ConfigSettings::add(const std::string& name, bool value)
{
    return add(name, ConfigSetting::create(value));
}
ConfigSetting* ConfigSettings::add(const std::string& name, int64_t value)
{
    return add(name, ConfigSetting::create(value));
}
ConfigSetting* ConfigSettings::add(const std::string& name, double value)
{
    return add(name, ConfigSetting::create(value));
}
ConfigSetting* ConfigSettings::add(const std::string& name, const std::string& value)
{
    return add(name, ConfigSetting::create_(value));
}

// this should seldom happen!
void ConfigSettings::remove(const std::string& settingName)
{
    auto iter = settings_.find(settingName);
    if(iter != settings_.end()) {
        auto item = iter->second;
        memdelete(item);
        settings_.erase(iter);
    }
}

void ConfigSettings::applyChanges()
{
    if (!hasChanges())
        return;

    for (auto& item : settings_) {
        auto setting = item.second;
        if (setting->hasChanged()) {
            String item_name(item.first.data());
            emit_signal("apply_setting", item_name, setting);
            setting->resetChange();
        }
    }
}

void ConfigSettings::undoPendingChanges()
{
    for (auto& item : settings_) {
        (item.second)->restore();
    }
}

void ConfigSettings::updateFrom(ConfigSettings* source)
{
    DEBUG("updateFrom");
    for (auto& item : source->settings_) {
        auto setting = item.second;
        auto& name = item.first;
        auto item = settings_.find(name);
        if (item == settings_.end()) {
            ConfigSetting* new_setting = memnew(ConfigSetting);
            new_setting->copyFrom(setting);
            settings_[name] = new_setting;
        }
        else {
            (item->second)->copyFrom(setting);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void to_json(json& j, ConfigSettings* source)
{
    auto& settings = source->getSettings_();
    for(auto& item : settings) {
        json entry;
        entry["name"] = item.first;
        switch(item.second->getType()) {
            case ConfigSetting::T_BOOL:
                entry["value"] = item.second->getBool();
                break;
            case ConfigSetting::T_INT:
                entry["value"] = item.second->getInt();
                break;
            case ConfigSetting::T_FLOAT:
                entry["value"] = item.second->getFloat();
                break;
            case ConfigSetting::T_STRING:
                entry["value"] = item.second->getStdString();
                break;
        }
        j.push_back(entry);
    }
}

void from_json(json& j, ConfigSettings* settings)
{
    settings->clear();
    for(auto& [key, item] : j.items()) {
        std::string configKey = item["name"];
        auto value = item["value"];

        if(value.is_number_float()) {
            double floatValue = static_cast<double>(value);
            settings->add(configKey, floatValue);
        }
        if(value.is_number_integer()) {
            int64_t intValue = static_cast<int64_t>(value);
            settings->add(configKey, intValue);
        }
        else if(value.is_boolean()) {
            bool boolValue = static_cast<bool>(value);
            settings->add(configKey, boolValue);
        }
        else {
            // treat as string
            std::string stringValue = static_cast<std::string>(value);
            settings->add(configKey, stringValue);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

void ConfigStore::_bind_methods()
{
    DECLARE_PROPERTY(ConfigStore, ActivePlayer, newPlayer, Variant::STRING, getActivePlayer, setActivePlayer);
    DECLARE_PROPERTY_READONLY(ConfigStore, SystemSettings, Variant::OBJECT, getSystemSettings);
    DECLARE_PROPERTY_READONLY(ConfigStore, GameplaySettings, Variant::OBJECT, getGameplaySettings);
    DECLARE_PROPERTY(ConfigStore, MainSource, source, Variant::STRING, getMainSource, setMainSource);
    DECLARE_PROPERTY(ConfigStore, BackupSource, source, Variant::STRING, getBackupSource, setBackupSource);
    DECLARE_PROPERTY(ConfigStore, AutoLoad, newState, Variant::BOOL, getAutoLoad, setAutoLoad);

    ClassDB::bind_method(D_METHOD("fromFile", "mainSource", "backupSource"), &ConfigStore::fromFile);
    ClassDB::bind_method(D_METHOD("toFile", "filename"), &ConfigStore::toFile);

    ClassDB::bind_method(D_METHOD("save"), &ConfigStore::save);
    ClassDB::bind_method(D_METHOD("load"), &ConfigStore::load);
}

ConfigStore::ConfigStore() : activePlayer_(String()), systemSettings_(nullptr), gameplaySettings_(nullptr),
    mainSource_(String()), backupSource_(String()), autoLoad_(false)
{
    systemSettings_ = memnew(ConfigSettings);
    gameplaySettings_ = memnew(ConfigSettings);
}

ConfigStore::~ConfigStore()
{
    if(!mainSource_.is_empty()) {
        save();
    }
    if(systemSettings_ != nullptr)
        memdelete(systemSettings_);
    if(gameplaySettings_ != nullptr)
        memdelete(gameplaySettings_);
}

ConfigStore* ConfigStore::fromFile(const String mainSource, const String backupSource)
{
    // for now, the backupSource is mandatory because it's the fallback plan
    if(backupSource.is_empty())
         return nullptr;

    // create folders if necessary
    ensureFolderExists(mainSource.get_base_dir());
    ensureFolderExists(backupSource.get_base_dir());

    // remmember if we have to write this out afterwards
    bool saveAfter = !FileAccess::file_exists(mainSource);
    String fileToUse = saveAfter ? backupSource : mainSource;

    auto fileContents = std::string(FileAccess::get_file_as_string(fileToUse).utf8().get_data());
    json j = json::parse(fileContents);

    activePlayer_ = String(std::string(j["player"]).data());
    from_json(j["system"], systemSettings_);
    from_json(j["gameplay"], gameplaySettings_);

    if(saveAfter) {
        toFile(mainSource);
    }

    return this;
}

bool ConfigStore::toFile(const String filename)
{
    ensureFolderExists(filename.get_base_dir());

    json j;
    j["player"] = std::string(activePlayer_.utf8().get_data());

    json system;
    to_json(system, systemSettings_);

    json gameplay;
    to_json(gameplay, gameplaySettings_);

    j["system"] = system;
    j["gameplay"] = gameplay;

    auto data = String(j.dump(4).data());
    auto file = FileAccess::open(filename, FileAccess::WRITE);
    file->store_string(data);
    file->close();

    return true;
}

