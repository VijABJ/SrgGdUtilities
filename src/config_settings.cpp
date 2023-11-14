
#include "config_settings.h"
#include "common_utils.h"
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/file_access.hpp>

/// 
/// ConfigItem
/// The setting object for one entry in the settings collection
///
//////////////////////////////////////////////////////////////////////////////////////

void ConfigItem::_bind_methods()
{
    DECLARE_ENUM_PROPERTY_READONLY(ConfigItem, Type, "T_BLANK,T_BOOL,T_INT,T_FLOAT,T_STRING");

    ClassDB::bind_method(D_METHOD("mark"), &ConfigItem::mark);
    ClassDB::bind_method(D_METHOD("restore"), &ConfigItem::restore);
    ClassDB::bind_method(D_METHOD("touch"), &ConfigItem::touch);
    ClassDB::bind_method(D_METHOD("hasChanged"), &ConfigItem::hasChanged);

    BIND_ENUM_CONSTANT(T_BLANK);
    BIND_ENUM_CONSTANT(T_BOOL);
    BIND_ENUM_CONSTANT(T_INT);
    BIND_ENUM_CONSTANT(T_FLOAT);
    BIND_ENUM_CONSTANT(T_STRING);
}

ConfigItem::ConfigItem() : type_(ConfigValueType::T_BLANK), changed_(false)
{
}

//////////////////////////////////////////////////////////////////////////////////////

void BoolConfigItem::_bind_methods()
{
    DECLARE_PROPERTY(BoolConfigItem, Value, value, Variant::BOOL);
}

ConfigItem* BoolConfigItem::create(const bool value)
{
    BoolConfigItem* item = memnew(BoolConfigItem);
    item->setValue(value);
    return item;
}

void IntConfigItem::_bind_methods()
{
    DECLARE_PROPERTY(IntConfigItem, Value, value, Variant::INT);
}

ConfigItem* IntConfigItem::create(const int64_t value)
{
    IntConfigItem* item = memnew(IntConfigItem);
    item->setValue(value);
    return item;
}

void FloatConfigItem::_bind_methods()
{
    DECLARE_PROPERTY(FloatConfigItem, Value, value, Variant::FLOAT);
}

ConfigItem* FloatConfigItem::create(const double value)
{
    FloatConfigItem* item = memnew(FloatConfigItem);
    item->setValue(value);
    return item;
}

void StringConfigItem::_bind_methods()
{
    DECLARE_PROPERTY(StringConfigItem, Value, value, Variant::STRING);
}

ConfigItem* StringConfigItem::create(const std::string& value)
{
    StringConfigItem* item = memnew(StringConfigItem);
    item->setValue(translate(value));
    return item;
}

/// 
/// ConfigItems
/// A collection of setting objects
///
//////////////////////////////////////////////////////////////////////////////////////

void ConfigItems::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("mark"), &ConfigItems::mark);
    ClassDB::bind_method(D_METHOD("restore"), &ConfigItems::restore);
    ClassDB::bind_method(D_METHOD("touch"), &ConfigItems::touch);
    ClassDB::bind_method(D_METHOD("hasChanges"), &ConfigItems::hasChanges);

    ClassDB::bind_method(D_METHOD("applyChanges"), &ConfigItems::applyChanges);
    ClassDB::bind_method(D_METHOD("undoPendingChanges"), &ConfigItems::undoPendingChanges);

    ClassDB::bind_method(D_METHOD("addSetting", "name", "setting"), &ConfigItems::addSetting);
    ClassDB::bind_method(D_METHOD("updateFrom", "source"), &ConfigItems::updateFrom);

    ClassDB::bind_method(D_METHOD("addBoolSetting", "name", "value"), &ConfigItems::addBoolSetting);
    ClassDB::bind_method(D_METHOD("addIntSetting", "name", "value"), &ConfigItems::addIntSetting);
    ClassDB::bind_method(D_METHOD("addFloatSetting", "name", "value"), &ConfigItems::addFloatSetting);
    ClassDB::bind_method(D_METHOD("addStringSetting", "name", "value"), &ConfigItems::addStringSetting);

    ClassDB::bind_method(D_METHOD("readBoolSetting", "defaultValue"), &ConfigItems::readBoolSetting);
    ClassDB::bind_method(D_METHOD("readIntSetting", "defaultValue"), &ConfigItems::readIntSetting);
    ClassDB::bind_method(D_METHOD("readFloatSetting", "defaultValue"), &ConfigItems::readFloatSetting);
    ClassDB::bind_method(D_METHOD("readStringSetting", "defaultValue"), &ConfigItems::readStringSetting);

    ClassDB::bind_method(D_METHOD("getSettings"), &ConfigItems::getSettings);

    ADD_SIGNAL(MethodInfo("apply_setting", PropertyInfo(Variant::STRING, "setting_name"),
        PropertyInfo(Variant::OBJECT, "setting", PROPERTY_HINT_OBJECT_ID, "ConfigItem")));
}

ConfigItem* ConfigItems::getSetting(String name)
{
    return getSetting_(translate(name));
}

ConfigItems::~ConfigItems()
{
    for (auto& item : settings_) {
        memdelete(item.second);
    }
}

ConfigItem* ConfigItems::getSetting_(const std::string& name, bool autoCreate)
{
    auto iter = settings_.find(name);
    if (iter != settings_.end())
        return iter->second;

    return nullptr;
}

Dictionary ConfigItems::getSettings() const
{
    Dictionary result;

    for (auto& item : settings_) {
        result[String(item.first.data())] = item.second;
    }

    return result;
}

void ConfigItems::mark()
{
    for (auto& item : settings_) {
        (item.second)->mark();
    }
}

void ConfigItems::restore()
{
    for (auto& item : settings_) {
        (item.second)->restore();
    }
}

void ConfigItems::touch()
{
    for (auto& item : settings_) {
        (item.second)->touch();
    }
}

bool ConfigItems::hasChanges() const
{
    for (auto& item : settings_) {
        if ((item.second)->hasChanged()) {
            return true;
        }
    }

    return false;
}

ConfigItem* ConfigItems::add(const std::string& name, ConfigItem* setting)
{
    // note that the item passed in WILL NOT be added if the
    // 'name' entry is already occupied!  instead, it will
    // be deallocated!  watch out!
    DEBUG(vformat("Setting::add(%s)", translate(name)));
    auto it = settings_.find(name);
    if (it == settings_.end()) {
        DEBUG("Adding...");
        settings_[name] = setting;
    }
    else {
        // same type. update the value of the existing one
        if (it->second->isSameType(*setting)) {
            DEBUG("Copying...");
            it->second->copyFrom(*setting);
        }
        // we're not going to overwrite an existing item
        // @TODO: maybe add an option to allow this?
        memdelete(setting);
    }

    return settings_[name];
}
ConfigItem* ConfigItems::add(const std::string& name, bool value)
{
    return add(name, BoolConfigItem::create(value));
}
ConfigItem* ConfigItems::add(const std::string& name, int64_t value)
{
    return add(name, IntConfigItem::create(value));
}
ConfigItem* ConfigItems::add(const std::string& name, double value)
{
    return add(name, FloatConfigItem::create(value));
}
ConfigItem* ConfigItems::add(const std::string& name, const std::string& value)
{
    return add(name, StringConfigItem::create(value));
}

ConfigItem* ConfigItems::add(const std::string& name, const String value)
{
    return add(name, StringConfigItem::create(translate(value)));
}

template<typename T, ConfigItem::ConfigValueType U, typename V>
T ConfigItems::get(const std::string& name, T defaultValue)
{
    ConfigItem* setting = getSetting_(name, false);
    if (setting != nullptr) {
        auto dataType = setting->getType();
        if (dataType == U) {
            return (static_cast<V*>(setting))->getValue();
        }
    }

    if (setting != nullptr) {
        remove(name);
    }
    add(name, defaultValue);
    return defaultValue;
}

ConfigItem* ConfigItems::addSetting(String name, ConfigItem* setting)
{
    return add(translate(name), setting);
}
ConfigItem* ConfigItems::addBoolSetting(String name, bool value)
{
    return add(translate(name), value);
}
ConfigItem* ConfigItems::addIntSetting(String name, int64_t value)
{
    return add(translate(name), value);
}
ConfigItem* ConfigItems::addFloatSetting(String name, double value)
{
    return add(translate(name), value);
}
ConfigItem* ConfigItems::addStringSetting(String name, String value)
{
    return add(translate(name), translate(value));
}

bool ConfigItems::readBoolSetting(String name, bool defaultValue)
{
    return get<bool, ConfigItem::T_BOOL, BoolConfigItem>(translate(name), defaultValue);
}
int64_t ConfigItems::readIntSetting(String name, int64_t defaultValue)
{
    return get<int64_t, ConfigItem::T_INT, IntConfigItem>(translate(name), defaultValue);
}
double ConfigItems::readFloatSetting(String name, double defaultValue)
{
    return get<double, ConfigItem::T_FLOAT, FloatConfigItem>(translate(name), defaultValue);
}
String ConfigItems::readStringSetting(String name, String defaultValue)
{
    return get<String, ConfigItem::T_STRING, StringConfigItem>(translate(name), defaultValue);
}

// this should seldom happen!
void ConfigItems::remove(const std::string& settingName)
{
    auto iter = settings_.find(settingName);
    if (iter != settings_.end()) {
        auto item = iter->second;
        memdelete(item);
        settings_.erase(iter);
    }
}

void ConfigItems::applyChanges()
{
    if (!hasChanges())
        return;

    for (auto& item : settings_) {
        auto setting = item.second;
        if (setting->hasChanged()) {
            String item_name(item.first.data());
            emit_signal("apply_setting", item_name, setting);
            setting->touch();
        }
    }
}

void ConfigItems::undoPendingChanges()
{
    for (auto& item : settings_) {
        (item.second)->restore();
    }
}

void ConfigItems::updateFrom(ConfigItems* source)
{
    DEBUG("updateFrom");
    for (auto& item : source->settings_) {
        auto setting = item.second;
        auto& name = item.first;
        auto item = settings_.find(name);
        if (item == settings_.end()) {
            ConfigItem* new_setting = memnew(ConfigItem);
            new_setting->copyFrom(*setting);
            settings_[name] = new_setting;
        }
        else {
            (item->second)->copyFrom(*setting);
        }
    }
}

