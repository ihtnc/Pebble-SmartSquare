/** \file
 * Word clock, like the qlock2.
 */
#include "main.h"
#include "btmonitor.h"
#include "thincfg.h"
#include "options.h"

static GFont font_on;
static GFont font_off;

/** 
Phase 1 (no overlap)
    Time:          Status:        Square:
    012345678901   012345678901   012345678901   
0 ITxxxISxAxxx 0 xxBTxISxxOFF 0 ITBThISzAOFF 0 
1 QUARTERTENxx 1 xxxxxxxxxxON 1 QUARTERTENON 1 
2 TWENTYxFIVE# 2 xxxxxxxxxxx# 2 TWENTYsFIVE# 2 
3 HALFxPASTTO# 3 xxxxxxxxxxx# 3 HALFsPASTTO# 3 
4 NINETENFOUR# 4 xxxxxxxxxxx# 4 NINETENFOUR# 4 
5 EIGHTTWELVE# 5 xxxxxxxxxxx# 5 EIGHTTWELVE# 5 
6 ONESIXELEVEN 6 xxxxxxxxxxxx 6 ONESIXELEVEN 6 
7 TWOSEVENFIVE 7 xxxxxxxxxxxx 7 TWOSEVENFIVE 7 
8 THREExOCLOCK 8 xxxxxxxxxxxx 8 THREEzOCLOCK 8 
  012345678901   012345678901   012345678901    
  
Phase 2 (with overlap)
    Time:          Status:        Square:
  012345678901   012345678901   012345678901 
0 ITxxxISxAxxx 0 xxBTxISxxOFF 0 ITBThIStAOFF 0 
1 QUARTERTENxx 1 xxxxxxxxxxON 1 QUARTERTENON 1 
2 TWENTYxFIVE# 2 xxxxxxxxxxx# 2 TWENTYsFIVE# 2 
3 HALFxPASTox# 3 xxxxxxxxxxx# 3 HALFsPASTOn# 3 
4 FOUREIGHTEN# 4 xxxxxxxxxxx# 4 FOUREIGHTEN# 4 
5 THREExTWONE# 5 xxxxxxxxxxx# 5 THREEpTWONE# 5 
6 TWELVExxSIX# 6 xxxxxxxxxxx# 6 TWELVEntSIX# 6 
7 FIVELEVENINE 7 xxxxxxxxxxxx 7 FIVELEVENINE 7 
8 SEVENxOCLOCK 8 xxxxxxxxxxxx 8 SEVENtOCLOCK 8 
  012345678901   012345678901   012345678901   
*/

static const word_t words[] =
{
	// Hours 1-12
	[1] = {  6, 0, "ONE", "one" },
	[2] = {  7, 0, "TWO", "two" },
	[3] = {  8, 0, "THREE", "three" },
	[4] = {  4, 7, "FOUR", "four" },
	[5] = {  7, 8, "FIVE", "five" }, 
	[6] = {  6, 3, "SIX", "six" },
	[7] = {  7, 3, "SEVEN", "seven" },
	[8] = {  5, 0, "EIGHT", "eight" },
	[9] = {  4, 0, "NINE", "nine" },
	[10] = {  4, 4, "TEN", "ten" },
	[11] = {  6, 6, "ELEVEN", "eleven" },
	[12] = {  5, 5, "TWELVE", "twelve" },

	// Minutes
	[LAYER_FIVE]	= {  2, 7, "FIVE", "five" }, 
	[LAYER_TEN]	= {  1, 7, "TEN", "ten" }, 
	[LAYER_A]	= {  0, 8, "A", "a" },
	[LAYER_QUARTER]	= {  1, 0, "QUARTER", "quarter" },
	[LAYER_HALF]	= {  3, 0, "HALF", "half" },
	[LAYER_TWENTY]	= {  2, 0, "TWENTY", "twenty" },

	// Relative
	[LAYER_IT] = {  0, 0, "IT", "it" },
	[LAYER_PAST]	= {  3, 5, "PAST", "past" },
	[LAYER_TO]	= {  3, 9, "TO", "to", },
	[LAYER_OCLOCK]	= {  8, 6, "OCLOCK", "oclock" },
	[LAYER_IS]	= {  0, 5, "IS", "is" },

	// Status: BT	
	[LAYER_BT] = {  0, 2, "BT", "bt" },
	[LAYER_BT_ON] = {  1, 10, "ON", "on" },
	[LAYER_BT_OFF] = {  0, 9, "OFF", "off" },
	
	// Status: Battery	
	[LAYER_BATTERY_25] = {  5, 11, "X", "x" },
	[LAYER_BATTERY_50] = {  4, 11, "X", "x" },
	[LAYER_BATTERY_75] = {  3, 11, "X", "x" },
	[LAYER_BATTERY_100] = {  2, 11, "X", "x" },
		
	// Fillers
	[LAYER_FILLER] =
	{  0, 4, "h", "h" },
	{  0, 7, "z", "z" },
	{  2, 6, "r", "r" },
	{  3, 4, "q", "q" },
	{  8, 5, "x", "x" },
};

static Window *window;
static TextLayer *layers[WORD_COUNT];
static Layer *minute_layer;
static int minute_num;
static InverterLayer *inverter;

static bool is_stat_showing;
static bool show_stat;
static AppTimer *timer;

static void text_layer_setup(Window *window, int which, GRect frame, GFont font) 
{
	layers[which] = text_layer_create(frame);
	text_layer_set_text(layers[which], "");
	text_layer_set_text_color(layers[which], GColorWhite);
	text_layer_set_background_color(layers[which], GColorClear);
	text_layer_set_font(layers[which], font);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(layers[which]));
}

/** Draw a box in a corner to indicate the number of minutes past the five.
 */
static void minute_layer_update(Layer * const me,  GContext *ctx) 
{
	const int w = 4;
	GRect r = GRect(0, 0, w, w);

	if (minute_num == 0) 
	{
		return; // nothing to draw
	}
	else if (minute_num == 1) 
	{
		r.origin = GPoint(0,0);
	}
	else if (minute_num == 2) 
	{
		r.origin = GPoint(144-w,0);
	}
	else if (minute_num == 3) 
	{
		r.origin = GPoint(144-w,168-w);
	}
	else if (minute_num == 4) 
	{
		r.origin = GPoint(0,168-w);
	}
	
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, r, 1, GCornersAll);
}

static void word_mark(int which, int on) 
{
	TextLayer * const layer = layers[which];
	const word_t * const w = &words[which];

	text_layer_set_text(layer, on ? w->text_on : w->text_off);
	text_layer_set_font(layer, on ? font_on : font_off);
}

static void display_time()
{
	time_t t = time(NULL);
    struct tm *local = localtime(&t);
	int hour = local->tm_hour;
	int min = local->tm_min;
	
	word_mark(LAYER_IT, 1);
	word_mark(LAYER_IS, 1);
	
	if (min < 5) 
	{
		word_mark(LAYER_OCLOCK, 1);
	}
	else if (min < 10) 
	{
		word_mark(LAYER_FIVE, 1);
		word_mark(LAYER_PAST, 1);
	}
	else if (min < 15) 
	{
		word_mark(LAYER_TEN, 1);
		word_mark(LAYER_PAST, 1);
	}
	else if (min < 20) 
	{
		word_mark(LAYER_A, 1);
		word_mark(LAYER_QUARTER, 1);
		word_mark(LAYER_PAST, 1);
	}
	else if (min < 25) 
	{
		word_mark(LAYER_TWENTY, 1);
		word_mark(LAYER_PAST, 1);
	}
	else if (min < 30) 
	{
		word_mark(LAYER_TWENTY, 1);
		word_mark(LAYER_FIVE, 1);
		word_mark(LAYER_PAST, 1);
	}
	else if (min < 35) 
	{
		word_mark(LAYER_HALF, 1);
		word_mark(LAYER_PAST, 1);
	}
	else if (min < 40) 
	{
		word_mark(LAYER_TWENTY, 1);
		word_mark(LAYER_FIVE, 1);
		word_mark(LAYER_TO, 1);
		hour++;
	}
	else if (min < 45) 
	{
		word_mark(LAYER_TWENTY, 1);
		word_mark(LAYER_TO, 1);
		hour++;
	}
	else if (min < 50) 
	{
		word_mark(LAYER_A, 1);
		word_mark(LAYER_QUARTER, 1);
		word_mark(LAYER_TO, 1);
		hour++;
	}
	else if (min < 55) 
	{
		word_mark(LAYER_TEN, 1);
		word_mark(LAYER_TO, 1);
		hour++;
	}
	else 
	{
		word_mark(LAYER_FIVE, 1);
		word_mark(LAYER_TO, 1);
		hour++;
	}

	// update the minute box
	minute_num = min % 5;
	layer_set_hidden(minute_layer, false);
	layer_mark_dirty(minute_layer);

	// Convert from 24-hour to 12-hour time
	if (hour == 0) 
	{
		hour = 12;
	}
	else if (hour > 12) 
	{
		hour -= 12;
	}

	word_mark(hour, 1);
}

static void clear_words()
{
	layer_set_hidden(minute_layer, true);
	
	word_mark(LAYER_IT, 0);
	word_mark(LAYER_IS, 0);
	word_mark(LAYER_OCLOCK, 0);
	word_mark(LAYER_FIVE, 0);
	word_mark(LAYER_TEN, 0);
	word_mark(LAYER_A, 0);
	word_mark(LAYER_QUARTER, 0);
	word_mark(LAYER_TWENTY, 0);
	word_mark(LAYER_HALF, 0);
	word_mark(LAYER_PAST, 0);
	word_mark(LAYER_TO, 0);
	word_mark(LAYER_BT, 0);
	word_mark(LAYER_BT_ON, 0);
	word_mark(LAYER_BT_OFF, 0);
	word_mark(LAYER_BATTERY_25, 0);
	word_mark(LAYER_BATTERY_50, 0);
	word_mark(LAYER_BATTERY_75, 0);
	word_mark(LAYER_BATTERY_100, 0);
	
	for (int i = 1 ; i <= 12 ; i++) 
	{
		word_mark(i, 0);
	}
}

static void display_stat()
{
	bool bt = bluetooth_connection_service_peek();
	word_mark(LAYER_BT, 1);
	word_mark(LAYER_IS, 1);
	
	if(bt == true)
	{
		word_mark(LAYER_BT_ON, 1);
	}
	else
	{
		word_mark(LAYER_BT_OFF, 1);
	}
	
	BatteryChargeState batt = battery_state_service_peek();
	uint8_t charge = batt.charge_percent;
	if(charge >= 75)
	{
		word_mark(LAYER_BATTERY_100, 1);
	}
		
	if(batt.charge_percent >= 50)
	{
		word_mark(LAYER_BATTERY_75, 1);
	}
	
	if(batt.charge_percent >= 25)
	{
		word_mark(LAYER_BATTERY_50, 1);
	}
	
	if(batt.charge_percent > 0)
	{
		word_mark(LAYER_BATTERY_25, 1);
	}
}


static void handle_timer(void *data)
{
    app_timer_cancel(timer);
	
	clear_words();
	display_time();
	
	show_stat = false;
	is_stat_showing = false;
}

/** Called once per minute.
 *
0-4 "IT IS X OCLOCK"
5-9 "IT IS FIVE PAST X"
10-14 "IT IS TEN PAST X"
15-19 "IT IS A QUARTER PAST X"
20-24 "IT IS TWENTY PAST X"
25-29 "IT IS TWENTY FIVE PAST X"
30-34 "IT IS HALF PAST X"
35-39 "IT IS TWENTY FIVE TO X+1"
40-44 "IT IS TWENTY TO X+1"
45-49 "IT IS A QUARTER TO X+1"
50-54 "IT IS TEN TO X+1"
55-59 "IT IS FIVE TO X+1"
 */
static void handle_tick(struct tm *tick_time, TimeUnits units_changed) 
{
	if(show_stat == true) return;
	
	timer = app_timer_register(1000, handle_timer, NULL);
}

static void word_layer_init(int which) 
{
	const word_t * const w = &words[which];

	GRect frame = GRect(w->col*FONT_W,
						w->row*FONT_H - 2,
						strlen(w->text_on)*(FONT_W+4),
						FONT_H+8);

	text_layer_setup(window, which, frame, font_off);
	word_mark(which, 0); // all are "off" initially
}

static void word_layer_deinit(int which) 
{
	text_layer_destroy(layers[which]);
}

static void handle_tap(AccelAxisType axis, int32_t direction)
{
	if(is_stat_showing == true) return;
	
	clear_words();
	display_stat();
	timer = app_timer_register(3000, handle_timer, NULL);
}
	
static void determine_invert_status(struct tm *tick_time)
{
	bool invert;
	bool mode = get_invert_mode_value();
	
	if(mode == INVERT_ON_AM)
	{
		invert = (tick_time->tm_hour < 12);
	}
	else if(mode == INVERT_ALWAYS)
	{
		invert = true;
	}
	else
	{
		invert = false;
	}
	
	layer_set_frame(inverter_layer_get_layer(inverter), GRect(0, 0, SCREEN_WIDTH, (invert ? SCREEN_HEIGHT : 0)));
}

static void inverter_deinit()
{
	layer_remove_from_parent(inverter_layer_get_layer(inverter));
	inverter_layer_destroy(inverter);
	free(inverter);
}

static void inverter_init()
{
	inverter = inverter_layer_create(GRect(0, 0, SCREEN_WIDTH, 0));
	layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(inverter));
	
	time_t t = time(NULL);
    struct tm *local = localtime(&t);
	determine_invert_status(local);
}

static void field_changed(const uint32_t key, const void *old_value, const void *new_value)
{
	if(key == CONFIG_KEY_INVERTMODE)
	{
		struct tm *local;
		time_t temp;
		time(&temp);
		local = localtime(&temp);
		determine_invert_status(local);
		free(local);
	}
}

static void init() 
{
	is_stat_showing = false;
	show_stat = false;
	
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_stack_push(window, true);

	thincfg_init();
    thincfg_subscribe((ThinCFGCallbacks) { .field_changed = field_changed, });
	
	inverter_init();
	btmonitor_init(get_bt_notification_value());
	
	font_on = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ON_20));
	font_off = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OFF_20));

	for (unsigned i = 0 ; i < WORD_COUNT ; i++) 
	{
		word_layer_init(i);
	}

	// Create a graphics layer for the entire background
	minute_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(minute_layer, minute_layer_update);
	layer_add_child(window_get_root_layer(window), minute_layer);

	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
	
	accel_tap_service_subscribe(handle_tap);
}


static void deinit() 
{
	accel_tap_service_unsubscribe();
	
	app_timer_cancel(timer);
    free(timer); 
	
	btmonitor_deinit();
    inverter_deinit();
		
	tick_timer_service_unsubscribe();
	layer_destroy(minute_layer);
	
	for (unsigned i = 0 ; i < WORD_COUNT ; i++) 
	{
		word_layer_deinit(i);
	}
	
	window_destroy(window);
	
	fonts_unload_custom_font(font_on);
	fonts_unload_custom_font(font_off);
}
	

int main(void) 
{
	init();
	app_event_loop();
	deinit();
}
