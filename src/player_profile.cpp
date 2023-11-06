#include "player_profile.h"
#include <godot_cpp/classes/file_access.hpp>

void NamedStatistic::_bind_methods()
{
    DECLARE_PROPERTY_READONLY(NamedStatistic, StringId, Variant::STRING, getStringId);
    DECLARE_PROPERTY(NamedStatistic, Value, value, Variant::FLOAT, getValue, setValue);

    ClassDB::bind_method(D_METHOD("initialize", "stringId", "initialValue"), &NamedStatistic::initialize);
}

NamedStatistic::NamedStatistic() : stringId_(String()), value_() 
{};

void NamedStatistic::initialize(String stringId, const double initialValue)
{
    stringId_ = stringId;
    value_ = initialValue;
}

/////////////////////////////////////////////////////////////////////////////////////////////////


void PlayerProfile::_bind_methods()
{
    DECLARE_PROPERTY(PlayerProfile, PlayerId, newId, Variant::STRING);
    DECLARE_PROPERTY(PlayerProfile, PlayerName, newName, Variant::STRING);
    DECLARE_PROPERTY(PlayerProfile, PortraitFile, filename, Variant::STRING);
    DECLARE_PROPERTY(PlayerProfile, UseSettings, state, Variant::BOOL);

    DECLARE_PROPERTY_READONLY(PlayerProfile, Statistics, Variant::OBJECT, getStatistics);
    DECLARE_PROPERTY_READONLY(PlayerProfile, Settings, Variant::OBJECT, getSettings);

    ClassDB::bind_method(D_METHOD("fromFile", "filename"), &PlayerProfile::fromFile);
    ClassDB::bind_method(D_METHOD("toFile", "filename"), &PlayerProfile::toFile);
}

Array PlayerProfile::getStatistics()
{
    Array arr;
    for(auto item : statistics_) {
        arr.append(item);
    }
    return arr;
}

NamedStatistic* PlayerProfile::getStatistic(String stringId)
{
    auto result = std::find_if(statistics_.begin(), statistics_.end(), [&stringId](auto ns) {
        return ns->getStringId() == stringId;
    });
    if(result != statistics_.end())
        return *result;

    auto ns = memnew(NamedStatistic);
    ns->setStringId(stringId);
    statistics_.push_back(ns);
    return ns;
}

PlayerProfile::PlayerProfile() : 
    playerId_(), playerName_(), portraitFile_(), statistics_(), 
    useSettings_(true), gameplaySettings_(nullptr)
{
    gameplaySettings_ = memnew(ConfigSettings);
}

PlayerProfile::~PlayerProfile()
{
    for(auto item : statistics_) {
        memdelete(item);
    }
    memdelete(gameplaySettings_);
}

PlayerProfile* PlayerProfile::fromFile(const String filename)
{
    if(!FileAccess::file_exists(filename))
        return nullptr;

    auto fileContents = std::string(FileAccess::get_file_as_string(filename).utf8().get_data());
    json j = json::parse(fileContents);

    playerId_ = translate(j["id"]);
    playerName_ = translate(j["name"]);
    portraitFile_ = translate(j["portrait"]);
    useSettings_ = j["use_settings"];

    int index = 0;
    json items = j["statistics"];
    if(items != nullptr) {
        while(index < items.size()) {
            std::string statName = items[index++];
            double statValue = items[index++];

            auto item = memnew(NamedStatistic);
            item->setStringId(translate(statName));
            item->setValue(statValue);
            statistics_.push_back(item);
        }
    }

    from_json(j["settings"], gameplaySettings_);

    return this;
}

bool PlayerProfile::toFile(const String filename)
{
    json j;
    j["id"] = translate(playerId_);
    j["name"] = translate(playerName_);
    j["use_settings"] = useSettings_;
    j["portrait"] = translate(portraitFile_);

    json stats;
    for(auto item : statistics_) {
        stats.push_back(translate(item->getStringId()));
        stats.push_back(item->getValue());
    }

    json gameplay;
    to_json(gameplay, gameplaySettings_);

    j["statistics"] = stats;
    j["settings"] = gameplay;

    auto data = String(j.dump(4).data());
    auto file = FileAccess::open(filename, FileAccess::WRITE);
    file->store_line(data);
    file->close();

    return true;
}
