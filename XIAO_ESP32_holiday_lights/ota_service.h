#pragma once
#include <stdint.h>
#include "status_pixel.h"

void beginNetworkUpdates();
void serviceNetworkUpdates(uint32_t now);
bool networkUpdateBusy();
bool takeAnimationRestartRequest();
StatusPixel networkStatusPixel(uint32_t now);
void setStatusFrameCallback(void (*callback)(uint32_t));
