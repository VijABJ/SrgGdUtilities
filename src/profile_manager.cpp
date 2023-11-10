#include "profile_manager.h"
#include "cguid.h"
#include <godot_cpp/classes/file_access.hpp>
#include "../../SrgGdHelpers/include/nlohmann/json.hpp"
using json = nlohmann::json;

void ProfileManager::_bind_methods()
{
    DECLARE_PROPERTY(ProfileManager, AutoLoad, newState, Variant::BOOL);
    DECLARE_RESOURCE_PROPERTY(ProfileManager, ProfileSource, source, FileList);
    DECLARE_PROPERTY(ProfileManager, ActiveProfileIndex, index, Variant::INT);
    DECLARE_PROPERTY(ProfileManager, AutoCreateDefault, newState, Variant::BOOL);

    ClassDB::bind_method(D_METHOD("getProfile", "index"), &ProfileManager::getProfile);
    ClassDB::bind_method(D_METHOD("getProfiles"), &ProfileManager::getProfiles);
    ClassDB::bind_method(D_METHOD("getProfileByName", "playerName"), &ProfileManager::getProfileByName);
    ClassDB::bind_method(D_METHOD("getNameToIndex", "playerName"), &ProfileManager::getNameToIndex);
    ClassDB::bind_method(D_METHOD("getIndexOf", "profile"), &ProfileManager::getIndexOf);
    ClassDB::bind_method(D_METHOD("getActiveProfile"), &ProfileManager::getActiveProfile);
    ClassDB::bind_method(D_METHOD("activateProfileByName", "playerName"), &ProfileManager::activateProfileByName);

    ClassDB::bind_method(D_METHOD("addNewProfile", "name"), &ProfileManager::addNewProfile);
    ClassDB::bind_method(D_METHOD("addNewProfileEx", "name", "id"), &ProfileManager::addNewProfileEx);
    ClassDB::bind_method(D_METHOD("isNameAvailable", "name"), &ProfileManager::isNameAvailable);

    ClassDB::bind_method(D_METHOD("deleteProfile", "index"), &ProfileManager::deleteProfile);
    ClassDB::bind_method(D_METHOD("deleteProfileByName", "name"), &ProfileManager::deleteProfileByname);

    ClassDB::bind_method(D_METHOD("loadProfiles"), &ProfileManager::loadProfiles);
    ClassDB::bind_method(D_METHOD("saveProfile", "profile"), &ProfileManager::saveProfile);

    ADD_SIGNAL(MethodInfo("profile_added", PropertyInfo(Variant::OBJECT, "profile", PROPERTY_HINT_OBJECT_ID, "PlayerProfile")));
    ADD_SIGNAL(MethodInfo("active_profile_changed", PropertyInfo(Variant::OBJECT, "profile", PROPERTY_HINT_OBJECT_ID, "PlayerProfile")));
}

ProfileManager::ProfileManager() : autoLoad_(true), profileSource_(Ref<FileList>()), autoCreateDefault_(true), profiles_(), activeProfileIndex_(-1)
{}

ProfileManager::~ProfileManager()
{
    for (auto profile : profiles_) {
        memdelete(profile);
    }
    profiles_.clear();
}

void ProfileManager::setActiveProfileIndex(const int64_t index)
{
    // do nothing if we already have it selected
    if (activeProfileIndex_ == index)
        return;

    activeProfileIndex_ = index;
    if (index >= profiles_.size()) activeProfileIndex_ = -1;
    auto profile = getProfile(activeProfileIndex_);
    emit_signal("active_profile_changed", profile);
}

void ProfileManager::activateProfileByName(String playerName)
{
    setActiveProfileIndex(getNameToIndex(playerName));
}

PlayerProfile* ProfileManager::getActiveProfile()
{
    if (profiles_.size() == 0)
        performAutoLoading();

    return getProfile(activeProfileIndex_);
}

PlayerProfile* ProfileManager::getProfile(const int64_t index)
{
    if ((activeProfileIndex_ >= 0) && (activeProfileIndex_ < profiles_.size()))
        return profiles_[activeProfileIndex_];

    return nullptr;
}

Array ProfileManager::getProfiles()
{
    return translate(profiles_);
}

PlayerProfile* ProfileManager::getProfileByName(const String playerName)
{
    return getProfile(getNameToIndex(playerName));
}

int64_t ProfileManager::getNameToIndex(const String playerName)
{
    if (playerName.is_empty())
        return -1;

    for (int i = 0; i < profiles_.size(); i++) {
        if (profiles_[i]->getPlayerName() == playerName)
            return i;
     }

    return -1;
}
int64_t ProfileManager::getIndexOf(PlayerProfile* profile)
{
    if (profile == nullptr)
        return -1;

    return getNameToIndex(profile->getPlayerName());
}

PlayerProfile* ProfileManager::addNewProfile(const String name)
{
    auto guid = xg::newGuid();
    String id = translate(guid.str());
    return addNewProfileEx(name, id);
}

// do NOT add a duplicate.  rather, just return an existing one.
PlayerProfile* ProfileManager::addNewProfileEx(const String name, const String id)
{
    auto item = getProfileByName(name);
    if (item != nullptr)
        return item;

    auto profile = memnew(PlayerProfile);
    profile->setPlayerId(id);
    profile->setPlayerName(name);
    profiles_.push_back(profile);

    emit_signal("profile_added", profile);

    return profile;
}

bool ProfileManager::isNameAvailable(const String name)
{
    for (auto& profile : profiles_) {
        if (profile->getPlayerName() == name)
            return false;
    }

    return true;
}

void ProfileManager::deleteProfile(const int64_t index)
{
    // can't delete if indices are bad, or if there's only one entry to begin with
    if ((index < 0) || (index >= profiles_.size()) || (profiles_.size() == 1))
        return;

    auto newSelectionIndex = index < activeProfileIndex_ ? activeProfileIndex_ - 1 : activeProfileIndex_;
    bool needActivating = newSelectionIndex != activeProfileIndex_;

    auto profile = profiles_[index];
    profiles_.erase(profiles_.begin() + index);

    memdelete(profile);

    if (needActivating)
        setActiveProfileIndex(newSelectionIndex);
}

void ProfileManager::deleteProfileByname(const String playerName)
{
    deleteProfile(getNameToIndex(playerName));
}

void ProfileManager::performAutoLoading()
{
    // only do this if we have an empty list!
    if (profiles_.size() != 0)
        return;

    loadProfiles();
    // if we STILL have an empty list, create the default one, AND SAVE IT!
    if (profiles_.size() == 0) {
        auto profile = addNewProfileEx("Default", "default");
        saveProfile(profile);
    }
    setActiveProfileIndex(0);
}

void ProfileManager::loadProfiles()
{
    auto files = profileSource_->getItemsLong();
    for (int i = 0; i < files.size(); i++) {
        auto& filename = files[i];
        if (!FileAccess::file_exists(filename))
            continue;

        auto fileContents = std::string(FileAccess::get_file_as_string(filename).utf8().get_data());
        json j = json::parse(fileContents);
        auto profile = memnew(PlayerProfile);
        from_json(j, profile);

        profiles_.push_back(profile);
    }
    if ((profiles_.size() == 0) && (autoCreateDefault_)) {
        addNewProfileEx("Default", "default");
    }
    if (profiles_.size() > 0) {
        setActiveProfileIndex(0);
    }
}

void ProfileManager::saveProfile(PlayerProfile* profile)
{
    json j;
    to_json(j, profile);

    auto data = String(j.dump(4).data());
    auto filename = profileSource_->getActualSourceFolder().path_join(profile->getPlayerId()) + ".json";
    auto file = FileAccess::open(filename, FileAccess::WRITE);
    file->store_line(data);
    file->close();
}

