#include "json_helpers.h"
#include "config_settings.h"
#include "player_profile.h"


void to_json(json& j, ConfigItems* settings)
{
    if (settings == nullptr)
        return;

    auto& items = settings->getSettings_();
    for (auto& item : items) {
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
    if (settings == nullptr)
        return;

    settings->clear();

    for (auto& [key, item] : j.items()) {
        std::string configKey = item["name"];
        auto& value = item["value"];

        if (value.is_number_float()) {
            double floatValue = value;
            settings->add(configKey, floatValue);
        }
        if (value.is_number_integer()) {
            int64_t intValue = value;
            settings->add(configKey, intValue);
        }
        else if (value.is_boolean()) {
            bool boolValue = value;
            settings->add(configKey, boolValue);
        }
        else if (value.is_string()) {
            std::string stringValue = value;
            settings->add(configKey, stringValue);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////

void to_json(json& j, PlayerProfile* profile)
{
    if (profile == nullptr)
        return;

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
    if (profile == nullptr)
        return;

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

