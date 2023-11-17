#include "player_profile.h"
#include "json_helpers.h"
#include <godot_cpp/classes/file_access.hpp>
#include "cguid.h"

void NamedStatistics::_bind_methods()
{
    DECLARE_PROPERTY(NamedStatistics, Items, items, Variant::OBJECT);
    ClassDB::bind_method(D_METHOD("appendItems", "items"), &NamedStatistics::appendItems);
}

Dictionary NamedStatistics::getItems() const
{
    Dictionary result;

    for (auto& item : items_) {
        result[translate(item.first)] = translate(item.second);
    }

    return result;
}
void NamedStatistics::setItems(Dictionary items)
{
    items_.clear();
    appendItems(items);
}
void NamedStatistics::appendItems(Dictionary items)
{
    auto keys = items.keys();
    for (int i = 0; i < keys.size(); i++) {
        String name = keys[i];
        String value = items[name];
        items_[translate(name)] = translate(value);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////


void PlayerProfile::_bind_methods()
{
    DECLARE_PROPERTY(PlayerProfile, PlayerId, newId, Variant::STRING);
    DECLARE_PROPERTY(PlayerProfile, PlayerName, newName, Variant::STRING);
    DECLARE_PROPERTY(PlayerProfile, PortraitFile, filename, Variant::STRING);
    DECLARE_PROPERTY(PlayerProfile, UseSettings, state, Variant::BOOL);

    ClassDB::bind_method(D_METHOD("getStatistics"), &PlayerProfile::getStatistics);
    ClassDB::bind_method(D_METHOD("getSettings"), &PlayerProfile::getSettings);
}

PlayerProfile::PlayerProfile() :
    playerId_(), playerName_(), portraitFile_(), statistics_(),
    useSettings_(true), gameplaySettings_(nullptr)
{
    gameplaySettings_ = memnew(ConfigItems);
    statistics_ = memnew(NamedStatistics);
}

PlayerProfile::~PlayerProfile()
{
    memdelete(gameplaySettings_);
    memdelete(statistics_);
}

// this will generate a new player id if it gets called
// while the field is empty
String PlayerProfile::getPlayerId()
{
    if (playerId_.is_empty()) {
        auto guid = xg::newGuid();
        playerId_ = translate(guid.str());
    }
    return playerId_;
}
