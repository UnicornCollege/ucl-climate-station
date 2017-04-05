#ifndef _APPLICATION_H
#define _APPLICATION_H

#include <bcl.h>
#include <bc_radio.h>
#include <bc_module_co2.h>

// Forward declarations

void climate_event_handler(bc_module_climate_event_t event, void *event_param);
void co2_event_handler(bc_module_co2_event_t event, void *event_param);

#endif // _APPLICATION_H