#include "config_store.h"
#include "common_utils.h"
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/file_access.hpp>

void ConfigStore::_bind_methods()
{
    DECLARE_PROPERTY(ConfigStore, ActivePlayer, newPlayer, Variant::STRING);
    DECLARE_PROPERTY(ConfigStore, AutoLoad, newState, Variant::BOOL);
    DECLARE_PROPERTY(ConfigStore, AutoSave, newState, Variant::BOOL);
    DECLARE_RESOURCE_PROPERTY(ConfigStore, RuntimeSource, source, FileLocator);
    DECLARE_RESOURCE_PROPERTY(ConfigStore, DefaultSource, source, FileLocator);

    ClassDB::bind_method(D_METHOD("save"), &ConfigStore::save);
    ClassDB::bind_method(D_METHOD("load"), &ConfigStore::load);

    ClassDB::bind_method(D_METHOD("getSystemSettings"), &ConfigStore::getSystemSettings);
    ClassDB::bind_method(D_METHOD("getGameplaySettings"), &ConfigStore::getGameplaySettings);

    ClassDB::bind_method(D_METHOD("onApplySetting", "setting_name", "setting"), &ConfigStore::onApplySetting);

    ADD_SIGNAL(MethodInfo("apply_setting", PropertyInfo(Variant::STRING, "setting_name"),
        PropertyInfo(Variant::OBJECT, "setting", PROPERTY_HINT_OBJECT_ID, "ConfigItem")));

}

ConfigStore::ConfigStore() : activePlayer_(String()), systemSettings_(nullptr), gameplaySettings_(nullptr),
autoLoad_(false), autoSave_(true), runtimeSource_(Ref<FileLocator>()), defaultSource_(Ref<FileLocator>())
{
    systemSettings_ = memnew(ConfigItems);
    gameplaySettings_ = memnew(ConfigItems);
}

ConfigStore::~ConfigStore()
{
    if (autoSave_)
        save();

    if (systemSettings_ != nullptr)
        memdelete(systemSettings_);
    if (gameplaySettings_ != nullptr)
        memdelete(gameplaySettings_);
}

void ConfigStore::onApplySetting(String setting_name, ConfigItem* setting)
{
    emit_signal("apply_setting", setting_name, setting);
}

void ConfigStore::load()
{
    // both sources must be assigned, or we can't proceed.
    if (!runtimeSource_.is_valid() || !defaultSource_.is_valid())
        return;

    auto runtimeFile = runtimeSource_->getResolvedPath();
    auto defaultFile = defaultSource_->getResolvedPath();

    // remmember if we have to write this out afterwards
    bool saveAfter = !FileAccess::file_exists(runtimeFile);
    String fileToUse = saveAfter ? defaultFile : runtimeFile;

    if (!FileAccess::file_exists(fileToUse))
        return;

    auto fileContents = std::string(FileAccess::get_file_as_string(fileToUse).utf8().get_data());
    json j = json::parse(fileContents);

    activePlayer_ = String(std::string(j["player"]).data());
    from_json(j["system"], systemSettings_);
    from_json(j["gameplay"], gameplaySettings_);

    if (saveAfter) {
        saveActual(runtimeFile);
    }
}

void ConfigStore::saveActual(const String filename)
{
    // shouldn't be necessary really, but just in case
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

    try {
        auto file = FileAccess::open(filename, FileAccess::WRITE);
        file->store_string(data);
        file->close();
        DEBUG("save complete");
    }
    catch (...)
    {
        DEBUG("Save failed.");
    }
}

void ConfigStore::save()
{
    // we might not have a source at all, like during creation.
    if (!runtimeSource_.is_valid())
        return;

    // if either settings instance aren't initialized, do nothing
    if ((systemSettings_ == nullptr) || (gameplaySettings_ == nullptr))
        return;

    auto runtimeFile = runtimeSource_->getResolvedPath();
    saveActual(runtimeFile);
}

