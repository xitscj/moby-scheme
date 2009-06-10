#ifndef KERNEL_H
#define KERNEL_H

#include "scheme.h"

#ifdef __cplusplus
extern "C" {
#endif 

extern val_t WorldKernel_world;
extern int WorldKernel_period[];

extern function_t *WorldKernel_tick_handler[],
                  *WorldKernel_button_handler,
                  *WorldKernel_redraw_led,
                  *WorldKernel_redraw_meter;
extern int WorldKernel_num_leds, WorldKernel_num_meters, WorldKernel_num_tick_handlers;

FUN(WorldKernel_bigBang);
FUN(WorldKernel_onTick);
FUN(WorldKernel_onButton);
FUN(WorldKernel_onRedrawLED);
FUN(WorldKernel_onRedrawMeter);

#ifdef __cplusplus
}
#endif

#endif
