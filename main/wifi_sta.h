
#pragma once

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief connect as wifi station.
 * @par
 */
void connectWIFI(const char *_hostname);

#ifdef __cplusplus
} // extern "C"
#endif

