#pragma once
#ifndef __SRG_JSON_HELPERS__
#define __SRG_JSON_HELPERS__

#include "../../SrgGdHelpers/include/nlohmann/json.hpp"
using json = nlohmann::json;

class ConfigItems;
void to_json(json& j, ConfigItems* settings);
void from_json(json& j, ConfigItems* settings);

class PlayerProfile;
void to_json(json& j, PlayerProfile* profile);
void from_json(json& j, PlayerProfile* profile);


#endif /// __SRG_JSON_HELPERS__

