#pragma once

#include "crackbot/types.h"

#include <cstdint>
#include <vector>

namespace crackbot {

extern std::vector<NetworkInfo> gNetworks;
extern NetworkInfo gSelected;
extern uint8_t gDeauthPacket[26];

void initDeauthTemplate();

}  // namespace crackbot
