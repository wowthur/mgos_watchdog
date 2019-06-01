#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "mgos.h"

void mgos_watchdog_init_net();
void mgos_watchdog_init_net_timer(int interval);
bool mgos_watchdog_init(void);

#ifdef __cplusplus
}
#endif