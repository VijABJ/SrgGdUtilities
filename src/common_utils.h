#pragma once
#ifndef __SRG_COMMON_UTILITIES__
#define __SRG_COMMON_UTILITIES__

#include "../../SrgGdHelpers/include/__templates.hpp"
#include "../../SrgGdHelpers/include/nlohmann/json.hpp"
using json = nlohmann::json;

void ensureFolderExists(const String folder);



#endif /// __SRG_COMMON_UTILITIES__

