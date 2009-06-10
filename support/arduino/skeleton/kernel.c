#include <stdbool.h>
#include <stdlib.h>
#include "scheme.h"

// 640 kB ought to be enough...
#define MAX_TICK_HANDLERS 8

val_t WorldKernel_world = & (struct val) { 1, NIL };
int WorldKernel_period[MAX_TICK_HANDLERS];

int WorldKernel_num_tick_handlers = 0;
function_t *WorldKernel_tick_handler[MAX_TICK_HANDLERS];
function_t *WorldKernel_button_handler = NULL;
function_t *WorldKernel_redraw_led = NULL;
function_t *WorldKernel_redraw_meter = NULL;

int WorldKernel_num_leds = 0, WorldKernel_num_meters = 0;

FUN(WorldKernel_bigBang) {
	START();
	WorldKernel_world = ARG(0);
	RETURN(alloc_nil());
}

FUN(WorldKernel_onTick) {
	START();
	WorldKernel_tick_handler[WorldKernel_num_tick_handlers] = function_val(ARG(0));
	WorldKernel_period[WorldKernel_num_tick_handlers] = (int) (number_val(ARG(1)) * 1000);
	WorldKernel_num_tick_handlers++;
	RETURN(alloc_nil());
}

FUN(WorldKernel_onButton) {
	START();
	WorldKernel_button_handler = function_val(ARG(0));
	RETURN(alloc_nil());
}

FUN(WorldKernel_onRedrawLED) {
	START();
	WorldKernel_num_leds = (int) number_val(ARG(0));
	WorldKernel_redraw_led = function_val(ARG(1));
	RETURN(alloc_nil());
}

FUN(WorldKernel_onRedrawMeter) {
	START();
	WorldKernel_num_meters = (int) number_val(ARG(0));
	WorldKernel_redraw_meter = function_val(ARG(1));
	RETURN(alloc_nil());
}
