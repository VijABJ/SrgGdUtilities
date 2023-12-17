#pragma once
#ifndef __SRG_ENVIRONMENT_WRAPPER_HEADER__
#define __SRG_ENVIRONMENT_WRAPPER_HEADER__

#include "config_store.h"
#include "profile_manager.h"

/// 
/// Should serve as a wrapper for useful things a program needs.
/// It should have the following minimum:
/// 
/// * Configuration settings, read/write to file
/// * Player profiles, portraits, and manager
/// 
class RuntimeEnvironment GDX_SUBCLASS(Resource)
{
    GDX_CLASS_PREFIX(RuntimeEnvironment, Resource);

public:
    RuntimeEnvironment() = default;
    virtual ~RuntimeEnvironment() = default;

    Ref<ConfigStore> getConfiguration() { return configuration_; }
    void setConfiguration(Ref<ConfigStore> configuration) { configuration_ = configuration; }

    Ref<ProfileManager> getProfiles() { return profiles_; }
    void setProfiles(Ref<ProfileManager> profiles) { profiles_ = profiles; }

    Ref<PathNamesCollection> getPortraits() { return portraits_; }
    void setPortraits(Ref<PathNamesCollection> portraits) { portraits_ = portraits; }

    Ref<FileLocator> getDefaultPortraitFile() { return defaultPortraitFile_; }
    void setDefaultPortraitFile(Ref<FileLocator> portraitFile) { defaultPortraitFile_ = portraitFile; }

    // this is required for proper functioning 
    void initialize();

protected:
    void onProfileChanged(PlayerProfile* profile);
    void onSettingApplied(String settingName, ConfigItem* setting);

private:
    Ref<ConfigStore> configuration_{};
    Ref<ProfileManager> profiles_{};
    Ref<PathNamesCollection> portraits_{};
    Ref<FileLocator> defaultPortraitFile_{};
};

#endif /// __SRG_ENVIRONMENT_WRAPPER_HEADER__
