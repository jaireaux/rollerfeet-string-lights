#pragma once
#include <stdint.h>

void beginNetworkUpdates();
void serviceNetworkUpdates(uint32_t now);
bool networkUpdateBusy();
bool takeAnimationRestartRequest();
