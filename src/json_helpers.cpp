#include "json_helpers.h"
#include "config_settings.h"
#include "player_profile.h"


void to_json(json& j, ConfigItems* source)
{
    auto& settings = source->getSettings_();
    for (auto& item : settings) {
        json entry;
        entry["name"] = item.first;
        switch (item.second->getType()) {
        case ConfigItem::T_BOOL:
            entry["value"] = (static_cast<BoolConfigItem*>(item.second))->getValue();
            break;
        case ConfigItem::T_INT:
            entry["value"] = (static_cast<IntConfigItem*>(item.second))->getValue();
            break;
        case ConfigItem::T_FLOAT:
            entry["value"] = (static_cast<FloatConfigItem*>(item.second))->getValue();
            break;
        case ConfigItem::T_STRING:
            entry["value"] = translate((static_cast<StringConfigItem*>(item.second))->getValue());
            break;
        }
        j.push_back(entry);
    }
}

void from_json(json& j, ConfigItems* settings)
{
    settings->clear();
    for (auto& [key, item] : j.items()) {
        std::string configKey = item["name"];
        auto value = item["value"];

        if (value.is_number_float()) {
            double floatValue = static_cast<double>(value);
            settings->add(configKey, floatValue);
        }
        if (value.is_number_integer()) {
            int64_t intValue = static_cast<int64_t>(value);
            settings->add(configKey, intValue);
        }
        else if (value.is_boolean()) {
            bool boolValue = static_cast<bool>(value);
            settings->add(configKey, boolValue);
        }
        else {
            // treat as string
            std::string stringValue = static_cast<std::string>(value);
            settings->add(configKey, stringValue);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void to_json(json& j, PlayerProfile* profile)
{
    j["id"] = translate(profile->getPlayerId());
    j["name"] = translate(profile->getPlayerName());
    j["use_settings"] = profile->getUseSettings();
    j["portrait"] = translate(profile->getPortraitFile());

    json stats;
    auto items = profile->getStatistics()->getItems();
    auto keys = items.keys();
    for (int i = 0; i < keys.size(); i++) {
        stats.push_back(translate(keys[i]));
        stats.push_back(translate(items[keys[i]]));
    }

    json gameplay;
    to_json(gameplay, profile->getSettings());

    j["statistics"] = stats;
    j["settings"] = gameplay;
}

void from_json(json& j, PlayerProfile* profile)
{
    profile->setPlayerId(translate(j["id"]));
    profile->setPlayerName(translate(j["name"]));
    profile->setPortraitFile(translate(j["portrait"]));
    profile->setUseSettings(j["use_settings"]);

    int index = 0;
    json items = j["statistics"];
    if (items != nullptr) {
        Dictionary temp;
        while (index < items.size()) {
            String statName = translate(items[index++]);
            String statValue = translate(items[index++]);
            temp[statName] = statValue;
        }
        profile->getStatistics()->setItems(temp);
    }

    from_json(j["settings"], profile->getSettings());
}





//PlayerProfile* PlayerProfile::fromFile(const String filename)
//{
//    if(!FileAccess::file_exists(filename))
//        return nullptr;
//
//    auto fileContents = std::string(FileAccess::get_file_as_string(filename).utf8().get_data());
//    json j = json::parse(fileContents);
//
//    playerId_ = translate(j["id"]);
//    playerName_ = translate(j["name"]);
//    portraitFile_ = translate(j["portrait"]);
//    useSettings_ = j["use_settings"];
//
//    int index = 0;
//    json items = j["statistics"];
//    if(items != nullptr) {
//        while(index < items.size()) {
//            std::string statName = items[index++];
//            double statValue = items[index++];
//
//            auto item = memnew(NamedStatistic);
//            item->setStringId(translate(statName));
//            item->setValue(statValue);
//            statistics_.push_back(item);
//        }
//    }
//
//    from_json(j["settings"], gameplaySettings_);
//
//    return this;
//}
//
//bool PlayerProfile::toFile(const String filename)
//{
//
//    return true;
//}
