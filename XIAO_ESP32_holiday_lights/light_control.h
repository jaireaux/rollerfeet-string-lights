#pragma once
#include <stdint.h>
#include <stddef.h>
bool applyLightCommand(const char *action, uint32_t value, uint32_t now);
void writeLightState(char *buffer, size_t capacity);
