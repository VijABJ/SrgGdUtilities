#pragma once
#ifndef __CONFIG_SETTINGS_HEADER__
#define __CONFIG_SETTINGS_HEADER__

#include "../../SrgGdHelpers/include/__templates.hpp"
#include <godot_cpp/variant/dictionary.hpp>
#include "undoable.hpp"
#include <string>

#include "../../SrgGdHelpers/include/nlohmann/json.hpp"
using json = nlohmann::json;

class ConfigSetting GDX_SUBCLASS(Node)
{
    GDX_CLASS_PREFIX(ConfigSetting, Node);

public:
    ConfigSetting();
    virtual ~ConfigSetting() {}

    enum ConfigValueType { T_EMPTY = 0, T_BOOL, T_INT, T_FLOAT, T_STRING };

    using u_bool_t = undoable_t<bool>;
    using u_int64_t = undoable_t<int64_t>;
    using u_float_t = undoable_t<double>;
    using u_string_t = undoable_t<std::string>;

    void mark();
    void restore();
    void resetChange();
    bool hasChanged() const { return changed_;  }

    bool getBool() const { return bool_; }
    void setBool(const bool value);

    int64_t getInt() const { return int_; }
    void setInt(const int64_t value);

    double getFloat() const { return float_; }
    void setFloat(const double value);

    std::string getStdString() const { return string_; }
    void setStdString(const std::string& value);

    String getString() const { return String(getStdString().data()); }
    void setString(const String value);

    void copyFrom(ConfigSetting* source);

    static ConfigSetting* create();
    static ConfigSetting* create(bool initial_value);
    static ConfigSetting* create(int64_t initial_value);
    static ConfigSetting* create(double initial_value);
    static ConfigSetting* create(String initial_value);
    static ConfigSetting* create_(const std::string& initial_value);

    //int64_t getType() const { return static_cast<int64_t>(type_); }
    //void setType(int64_t type) { type_ = static_cast<ConfigValueType>(type); }

    ConfigValueType getType() const { return type_; }
    void setType(ConfigValueType type) { type_ = type; }

    operator bool() { return bool_; }
    operator int64_t() { return int_; }
    operator double() { return float_; }
    operator std::string() { return string_; }

private:
    ConfigValueType type_;
    bool changed_;
    u_bool_t bool_;
    u_int64_t int_;
    u_float_t float_;
    u_string_t string_;
};


class ConfigSettings GDX_SUBCLASS(Node)
{
    GDX_CLASS_PREFIX(ConfigSettings, Node);

public:
    ConfigSettings() : settings_() {}
    virtual ~ConfigSettings();

    using settings_list_t = std::map<std::string, ConfigSetting*>;

    // this will add the setting to the dictionary if it wasn't there yet
    ConfigSetting* getSetting(String name);
    ConfigSetting* getSetting_(const std::string& name, bool autoCreate = true);

    void markAll();
    bool hasChanges() const;

    ConfigSetting* add(const std::string& name, ConfigSetting* setting);
    ConfigSetting* add(const std::string& name, bool value);
    ConfigSetting* add(const std::string& name, int64_t value);
    ConfigSetting* add(const std::string& name, double value);
    ConfigSetting* add(const std::string& name, const std::string& value);

    template<typename T, ConfigSetting::ConfigValueType U>
    T get(const std::string& name, T defaultValue);

    ConfigSetting* addSetting(String name, ConfigSetting* setting);
    ConfigSetting* addBoolSetting(String name, bool value);
    ConfigSetting* addIntSetting(String name, int64_t value);
    ConfigSetting* addFloatSetting(String name, double value);
    ConfigSetting* addStringSetting(String name, String value);

    Dictionary getSettings() const;

    bool readBoolSetting(String name, bool defaultValue);
    int64_t readIntSetting(String name, int64_t defaultValue);
    double readFloatSetting(String name, double defaultValue);
    String readStringSetting(String name, String defaultValue);

    // this will iterate on changed values ONLY, and call the godot event.
    // after this, changed flags will be reset
    void applyChanges(); 
    // this will reset all values that have changed to their previous values
    void undoPendingChanges();
    // copy settings from source.  will set changed flag if necessary.
    // will also create items that didn't exist in our list.
    void updateFrom(ConfigSettings* source);

    const settings_list_t& getSettings_() const { return settings_; }
    void clear() { settings_.clear(); }

private:
    settings_list_t settings_;

    void remove(const std::string& settingName);
};


class ConfigStore GDX_SUBCLASS(Node)
{
    GDX_CLASS_PREFIX(ConfigStore, Node);
   
public:
    ConfigStore();
    virtual ~ConfigStore();

    String getActivePlayer() const { return activePlayer_; }
    void setActivePlayer(const String newPlayer) { activePlayer_ = newPlayer; }

    ConfigSettings* getSystemSettings() const { return systemSettings_; }
    ConfigSettings* getGameplaySettings() const { return gameplaySettings_; }

    String getMainSource() const { return mainSource_; }
    void setMainSource(const String source) { mainSource_ = source; }
    String getBackupSource() const { return backupSource_; }
    void setBackupSource(const String source) { backupSource_ = source; }

    bool getAutoLoad() const { return autoLoad_; }
    void setAutoLoad(const bool newState) { autoLoad_ = newState; }

    // mainSource should be where the configuration file is expected to be.
    // backupSource will be loaded if mainSource can't be found.  afterwards,
    //     the configuration will write this to mainSource for future use.
    ConfigStore* fromFile(const String mainSource, const String backupSource);
    bool toFile(const String filename);

    void load() { fromFile(mainSource_, backupSource_); }
    void save() { toFile(mainSource_); }

private:
    String activePlayer_;
    ConfigSettings* systemSettings_;
    ConfigSettings* gameplaySettings_;

    bool autoLoad_;
    String mainSource_;
    String backupSource_;

    void allocateThings();
};

void to_json(json& j, ConfigSettings* source);
void from_json(json& j, ConfigSettings* settings);

VARIANT_ENUM_CAST(ConfigSetting::ConfigValueType);

#endif /// __CONFIG_SETTINGS_HEADER__
