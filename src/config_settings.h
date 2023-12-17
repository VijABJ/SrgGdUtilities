#pragma once
#ifndef __CONFIG_SETTINGS_HEADER__
#define __CONFIG_SETTINGS_HEADER__

#include "../../SrgGdHelpers/include/__templates.hpp"
#include <godot_cpp/variant/dictionary.hpp>
#include "undoable.hpp"
#include <string>

class ConfigItem GDX_SUBCLASS(Node)
{
    GDX_CLASS_PREFIX(ConfigItem, Node);

public:
    ConfigItem() = default;
    virtual ~ConfigItem() = default;

    enum ConfigValueType { T_BLANK = 0, T_BOOL, T_INT, T_FLOAT, T_STRING };

    void mark() {
        changed_ = false;
        internalMark();
    }
    void restore() {
        if (changed_) {
            internalRestore();
            changed_ = false;
        }
    }
    void touch() {
        if (changed_) {
            internalTouch();
            changed_ = false;
        }
    }
    bool hasChanged() const { return changed_; }

    bool isSameType(const ConfigItem & other) { return type_ == other.type_; }
    void copyFrom(const ConfigItem & other) {
        if (isSameType(other))
            performCopy(other);
    }

    ConfigValueType getType() const { return type_; }

protected:
    ConfigValueType type_{ ConfigValueType::T_BLANK };
    bool changed_{};

    virtual void internalMark() {}
    virtual void internalRestore() {}
    virtual void internalTouch() {}
    virtual void performCopy(const ConfigItem& other) {}
};

VARIANT_ENUM_CAST(ConfigItem::ConfigValueType);

class BoolConfigItem GDX_SUBCLASS(ConfigItem)
{
    GDX_CLASS_PREFIX(BoolConfigItem, ConfigItem);

public:
    BoolConfigItem() { type_ = T_BOOL; }
    virtual ~BoolConfigItem() = default;

    bool getValue() const { return value_; }
    void setValue(const bool value) {
        value_ = value;
        changed_ = value_.hasChanged();
    }

    operator bool() { return value_; }
    static ConfigItem* create(const bool value);

protected:
    virtual void internalMark() { value_.mark(); }
    virtual void internalRestore() { value_.restore(); }
    virtual void internalTouch() { value_.touch(); }
    virtual void performCopy(const ConfigItem & other) {
        auto otherValue = (dynamic_cast<const BoolConfigItem&>(other)).value_;
        if (value_ != otherValue) {
            setValue(otherValue);
        }
    }

private:
    undoable_t<bool> value_{};
};

class IntConfigItem GDX_SUBCLASS(ConfigItem)
{
    GDX_CLASS_PREFIX(IntConfigItem, ConfigItem);

public:
    IntConfigItem() { type_ = T_INT; }
    virtual ~IntConfigItem() = default;

    int64_t getValue() const { return value_; }
    void setValue(const int64_t value) {
        value_ = value;
        changed_ = value_.hasChanged();
    }

    operator int64_t() { return value_; }
    static ConfigItem* create(const int64_t value);

protected:
    virtual void internalMark() { value_.mark(); }
    virtual void internalRestore() { value_.restore(); }
    virtual void internalTouch() { value_.touch(); }
    virtual void performCopy(const ConfigItem & other) {
        auto otherValue = (dynamic_cast<const IntConfigItem&>(other)).value_;
        if (value_ != otherValue) {
            setValue(otherValue);
        }
    }

private:
    undoable_t<int64_t> value_{};
};

class FloatConfigItem GDX_SUBCLASS(ConfigItem)
{
    GDX_CLASS_PREFIX(FloatConfigItem, ConfigItem);

public:
    FloatConfigItem() { type_ = T_FLOAT; }
    virtual ~FloatConfigItem() = default;

    double getValue() const { return value_; }
    void setValue(const double value) {
        value_ = value;
        changed_ = value_.hasChanged();
    }

    operator double() { return value_; }
    static ConfigItem* create(const double value);

protected:
    virtual void internalMark() { value_.mark(); }
    virtual void internalRestore() { value_.restore(); }
    virtual void internalTouch() { value_.touch(); }
    virtual void performCopy(const ConfigItem & other) {
        auto otherValue = (dynamic_cast<const FloatConfigItem&>(other)).value_;
        if (value_ != otherValue) {
            setValue(otherValue);
        }
    }

private:
    undoable_t<double> value_{};
};

class StringConfigItem GDX_SUBCLASS(ConfigItem)
{
    GDX_CLASS_PREFIX(StringConfigItem, ConfigItem);

public:
    StringConfigItem() { type_ = T_STRING; }
    virtual ~StringConfigItem() = default;

    String getValue() const { return translate(value_); }
    void setValue(const String value) {
        value_ = translate(value);
        changed_ = value_.hasChanged();
    }
    void setStdStringValue(const std::string& value) {
        value_ = value;
        changed_ = value_.hasChanged();
    }

    operator std::string() { return value_; }
    static ConfigItem* create(const std::string& value);

protected:
    virtual void internalMark() { value_.mark(); }
    virtual void internalRestore() { value_.restore(); }
    virtual void internalTouch() { value_.touch(); }
    virtual void performCopy(const ConfigItem & other) {
        setStdStringValue((dynamic_cast<const StringConfigItem&>(other)).value_);
    }

private:
    undoable_t<std::string> value_{};
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConfigItems GDX_SUBCLASS(Node)
{
    GDX_CLASS_PREFIX(ConfigItems, Node);

public:
    ConfigItems() = default;
    virtual ~ConfigItems();

    using settings_list_t = std::map<std::string, ConfigItem*>;

    void mark();
    void restore();
    void touch();
    bool hasChanges() const;

    ConfigItem* add(const std::string & name, ConfigItem * setting);
    ConfigItem* add(const std::string & name, bool value);
    ConfigItem* add(const std::string & name, int64_t value);
    ConfigItem* add(const std::string & name, double value);
    ConfigItem* add(const std::string & name, const std::string & value);
    ConfigItem* add(const std::string& name, const String value);

    template<typename T, ConfigItem::ConfigValueType U, typename V>
    T get(const std::string & name, T defaultValue);

    ConfigItem* addSetting(String name, ConfigItem * setting);
    ConfigItem* addBoolSetting(String name, bool value);
    ConfigItem* addIntSetting(String name, int64_t value);
    ConfigItem* addFloatSetting(String name, double value);
    ConfigItem* addStringSetting(String name, String value);

    Dictionary getSettings() const;
    // this will add the setting to the dictionary if it wasn't there yet
    ConfigItem* getSetting(String name);
    ConfigItem* getSetting_(const std::string& name, bool autoCreate = true);


    bool readBoolSetting(String name, bool defaultValue);
    int64_t readIntSetting(String name, int64_t defaultValue);
    double readFloatSetting(String name, double defaultValue);
    String readStringSetting(String name, String defaultValue);

    // this will iterate on changed values ONLY, and call the godot event.
    // after this, changed flags will be reset
    void applyChanges();
    // for when things get reset mid program. only C++ side for now
    void forceApplyChanges();
    // this will reset all values that have changed to their previous values
    void undoPendingChanges();
    // copy settings from source.  will set changed flag if necessary.
    // will also create items that didn't exist in our list.
    void updateFrom(ConfigItems * source);

    const settings_list_t& getSettings_() const { return settings_; }
    void clear() { settings_.clear(); }

private:
    settings_list_t settings_{};

    void remove(const std::string & settingName);
};

#endif /// __CONFIG_SETTINGS_HEADER__
