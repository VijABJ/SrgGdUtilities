#include "runtime_environment.h"


void RuntimeEnvironment::_bind_methods()
{
    DECLARE_RESOURCE_PROPERTY(RuntimeEnvironment, Configuration, configuration, ConfigStore);
    DECLARE_RESOURCE_PROPERTY(RuntimeEnvironment, Profiles, profileManager, ProfileManager);
    DECLARE_RESOURCE_PROPERTY(RuntimeEnvironment, Portraits, portraits, PathNamesCollection);
    DECLARE_RESOURCE_PROPERTY(RuntimeEnvironment, DefaultPortraitFile, portraitFile, FileLocator);

    ClassDB::bind_method(D_METHOD("initialize"), &RuntimeEnvironment::initialize);
    ClassDB::bind_method(D_METHOD("onProfileChanged", "profile"), &RuntimeEnvironment::onProfileChanged);
    ClassDB::bind_method(D_METHOD("onSettingApplied", "settingName", "setting"), &RuntimeEnvironment::onSettingApplied);

    ADD_SIGNAL(MethodInfo("active_profile_changed", PropertyInfo(Variant::OBJECT, "profile", PROPERTY_HINT_OBJECT_ID, "PlayerProfile")));
    ADD_SIGNAL(MethodInfo("apply_setting", PropertyInfo(Variant::STRING, "setting_name"), PropertyInfo(Variant::OBJECT, "setting", PROPERTY_HINT_OBJECT_ID, "ConfigItem")));

}

RuntimeEnvironment::RuntimeEnvironment() : 
    configuration_(Ref<ConfigStore>()),
    profiles_(Ref<ProfileManager>()),
    portraits_(Ref<PathNamesCollection>()),
    defaultPortraitFile_(Ref<FileLocator>())
{}

void RuntimeEnvironment::initialize()
{
    configuration_->connect("apply_setting", Callable(this, "onSettingApplied"));
    profiles_->connect("active_profile_changed", Callable(this, "onProfileChanged"));
    //
    configuration_->load();
    profiles_->loadProfiles();
}

void RuntimeEnvironment::onProfileChanged(PlayerProfile* profile)
{
    if (profile == nullptr) {
        configuration_->setActivePlayer("");
    }
    else {
        configuration_->setActivePlayer(profile->getPlayerName());
    }
    
    emit_signal("active_profile_changed", profile);
}

void RuntimeEnvironment::onSettingApplied(String settingName, ConfigItem* setting)
{
    emit_signal("apply_setting", settingName, setting);
}