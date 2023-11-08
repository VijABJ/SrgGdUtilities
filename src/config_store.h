#pragma once
#ifndef __SRG_CONFIGURATION_STORAGE__
#define __SRG_CONFIGURATION_STORAGE__

#include "config_settings.h"
#include "files_source.h"
#include "json_helpers.h"

class ConfigStore GDX_SUBCLASS(Resource)
{
    GDX_CLASS_PREFIX(ConfigStore, Resource);

public:
    ConfigStore();
    virtual ~ConfigStore();

    String getActivePlayer() const { return activePlayer_; }
    void setActivePlayer(const String newPlayer) { activePlayer_ = newPlayer; }

    ConfigItems* getSystemSettings() const { return systemSettings_; }
    ConfigItems* getGameplaySettings() const { return gameplaySettings_; }

    Ref<FileLocator> getRuntimeSource() const { return runtimeSource_; }
    void setRuntimeSource(Ref<FileLocator> newSource) { runtimeSource_ = newSource; }

    Ref<FileLocator> getDefaultSource() const { return defaultSource_; }
    void setDefaultSource(Ref<FileLocator> newSource) { defaultSource_ = newSource; }

    bool getAutoLoad() const { return autoLoad_; }
    void setAutoLoad(const bool newState) { autoLoad_ = newState; }
    bool getAutoSave() const { return autoSave_; }
    void setAutoSave(const bool newState) { autoSave_ = newState; }

    void load();
    void save();

private:
    String activePlayer_;
    ConfigItems* systemSettings_;
    ConfigItems* gameplaySettings_;

    bool autoLoad_;
    bool autoSave_;

    Ref<FileLocator> runtimeSource_;
    Ref<FileLocator> defaultSource_;

    void saveActual(const String filename);
};

#endif /// __SRG_CONFIGURATION_STORAGE__
