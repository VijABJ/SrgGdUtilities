#pragma once
#ifndef __SRG_PROFILE_MANAGER_HEADER__
#define __SRG_PROFILE_MANAGER_HEADER__


#include "files_source.h"
#include "config_store.h"
#include "player_profile.h"

class ProfileManager GDX_SUBCLASS(Resource)
{
    GDX_CLASS_PREFIX(ProfileManager, Resource);

public:
    ProfileManager();
    virtual ~ProfileManager();

    bool getAutoLoad() const { return autoLoad_; }
    void setAutoLoad(const bool newState) { autoLoad_ = newState; }

    Ref<FileList> getProfileSource() const { return profileSource_; }
    void setProfileSource(Ref<FileList> sources) { profileSource_ = sources; }

    int64_t getActiveProfileIndex() const { return activeProfileIndex_; }
    void setActiveProfileIndex(const int64_t index);
    void activateProfileByName(String playerName);

    PlayerProfile* getActiveProfile();
    PlayerProfile* getProfile(const int64_t index);
    Array getProfiles();
    PlayerProfile* getProfileByName(const String playerName);
    int64_t getNameToIndex(const String playerName);
    int64_t getIndexOf(PlayerProfile* profile);

    PlayerProfile* addNewProfile(const String name);
    PlayerProfile* addNewProfileEx(const String name, const String id);

    void deleteProfile(const int64_t index);
    void deleteProfileByname(const String playerName);

    void loadProfiles();
    void saveProfile(PlayerProfile* profile);

    bool getAutoCreateDefault() const { return autoCreateDefault_; }
    void setAutoCreateDefault(const bool newState) { autoCreateDefault_ = newState; }

private:
    bool autoLoad_;
    Ref<FileList> profileSource_;

    bool autoCreateDefault_;
    std::vector<PlayerProfile*> profiles_;
    int64_t activeProfileIndex_;

    void performAutoLoading();
};





#endif /// __SRG_PROFILE_MANAGER_HEADER__
