/** \file
 * Word clock, like the qlock2.
 */
#include "main.h"
#include "btmonitor.h"
#include "thincfg.h"
#include "options.h"

static GFont font;

/** 
Phase 1 (no overlap)
  Time:          Status:        Square:        Splash:
  012345678901   012345678901   012345678901   012345678901 
0 IT.IS...A... 0 .....BT:.OFF 0 IThISBT:AOFF 0 tweaked:AOFF 0  
1 QUARTERTEN.. 1 ..........ON 1 QUARTERTENON 1 QUARTERTbyON 1  
2 TWENTY.FIVE. 2 ...........# 2 TWENTYrFIVE# 2 TWENTYRFIVE# 2  
3 HALF.PASTTO. 3 HALF.PAST..# 3 HALFhPASTTO# 3 HALihtncTTO# 3  
4 NINETENFOUR. 4 N..E.E..O..# 4 NINETENFOUR# 4 NINETENFOUR# 4  
5 EIGHTTWELVE. 5 EIGH.TWEL..# 5 EIGHTTWELVE# 5 EIGHTTWELVE# 5  
6 ONESIXELEVEN 6 O..S.X..E... 6 ONESIXELEVEN 6 ONESIXEsmart 6  
7 TWOSEVENFIVE 7 T..S.V..F... 7 TWOSEVENFIVE 7 TWOSEVsquare 7  
8 THREE.OCLOCK 8 THRE.XOCL... 8 THREExOCLOCK 8 THREE(c)2014 8  
  012345678901   012345678901   012345678901   012345678901    
  
Phase 2 (with overlap)
  Time:          Status:        Square:        Splash:
  012345678901   012345678901   012345678901   012345678901 
0 IT.IS...A... 0 ............ 0 IThISiswAtch 0 tweakedtATCH 0  
1 QUARTERTEN.. 1 ............ 1 QUARTERTENty 1 QUARTERTbyTY 1  
2 TWENTY.FIVE. 2 ............ 2 TWENTYnFIVEs 2 TWENTYNFIVEs 2  
3 ..HALF.PASTO 3 DY.......... 3 DYHALFhPASTO 3 DYHihtncASTO 3  
4 FOUREIGHTEN. 4 ............ 4 FOUREIGHTENt 4 FOUREIGHTENt 4  
5 THREE.TWONE. 5 ............ 5 THREEeTWONES 5 THREEETWONES 5  
6 TWELVESIXXXX 6 ........XXXX 6 TWELVESIXXXX 6 TWELVESsmart 6  
7 FIVELEVENINE 7 ............ 7 FIVELEVENINE 7 FIVELEsquare 7  
8 SEVEN.OCLOCK 8 .....X...... 8 SEVENXOCLOCK 8 SEVEN(C)2014 8  
  012345678901   012345678901   012345678901   012345678901    
*/

static const text_t default_text[] =
{
	[DEFAULT_TEXT_1] = { "ithisiswatch" },
	[DEFAULT_TEXT_2] = { "quartertenty" },
	[DEFAULT_TEXT_3] = { "twentynfives" },
	[DEFAULT_TEXT_4] = { "dyhalfhpasto" },
	[DEFAULT_TEXT_5] = { "foureightent" },
	[DEFAULT_TEXT_6] = { "threeetwones" },
	[DEFAULT_TEXT_7] = { "twelvesixxxx" },
	[DEFAULT_TEXT_8] = { "fivelevenine" },
    [DEFAULT_TEXT_9] = { "sevenxoclock" },
};

static text_t current_text[] =
{
	[DEFAULT_TEXT_1] = { "ithisiswatch" },
	[DEFAULT_TEXT_2] = { "quartertenty" },
	[DEFAULT_TEXT_3] = { "twentynfives" },
	[DEFAULT_TEXT_4] = { "dyhalfhpasto" },
	[DEFAULT_TEXT_5] = { "foureightent" },
	[DEFAULT_TEXT_6] = { "threeetwones" },
	[DEFAULT_TEXT_7] = { "twelvesixxxx" },
	[DEFAULT_TEXT_8] = { "fivelevenine" },
    [DEFAULT_TEXT_9] = { "sevenxoclock" },
};

#define LAYER_COUNT ((sizeof(default_text) / sizeof(*default_text)))

static const word_t words[] =
{
	// Hours 1-12
	[LAYER_H_01] = { 5, 8, "ONE" },
	[LAYER_H_02] = { 5, 6, "TWO" },
	[LAYER_H_03] = { 5, 0, "THREE" },
	[LAYER_H_04] = { 4, 0, "FOUR" },
	[LAYER_H_05] = { 7, 0, "FIVE" },
	[LAYER_H_06] = { 6, 6, "SIX" },
	[LAYER_H_07] = { 8, 0, "SEVEN" },
	[LAYER_H_08] = { 4, 4, "EIGHT" },
	[LAYER_H_09] = { 7, 8, "NINE" },
	[LAYER_H_10] = { 4, 8, "TEN" },
	[LAYER_H_11] = { 7, 3, "ELEVEN" },
	[LAYER_H_12] = { 6, 0, "TWELVE" },
	
	// Minutes
	[LAYER_M_OCLOCK] = { 8, 6, "OCLOCK" },
	[LAYER_M_FIVE] = { 2, 7, "FIVE" },
	[LAYER_M_TEN] = { 1, 7, "TEN" },
	[LAYER_M_QUARTER] = { 1, 0, "QUARTER" },
	[LAYER_M_TWENTY] = { 2, 0, "TWENTY" },
	[LAYER_M_HALF] = { 3, 2, "HALF" },

	[LAYER_IT] = { 0, 0, "IT" },
	[LAYER_IS] = { 0, 3, "IS" },
	[LAYER_A] = { 0, 8, "A" },
	[LAYER_PAST] = { 3, 7, "PAST" },
	[LAYER_TO] = { 3, 10, "TO" },
	
	[LAYER_BT_OFF] = { 8, 5, "X" },
	[LAYER_BT_ON] = { 8, 5, "x" },
	
	[LAYER_BATTERY_25] = { 6, 11, ":" },
	[LAYER_BATTERY_50] = { 6, 10, ":" },
	[LAYER_BATTERY_75] = { 6, 9, ":" },
	[LAYER_BATTERY_100] = { 6, 8, ":" },
	
	[LAYER_BATTERY_25_OFF] = { 6, 11, "x" },
	[LAYER_BATTERY_50_OFF] = { 6, 10, "x" },
	[LAYER_BATTERY_75_OFF] = { 6, 9, "x" },
	[LAYER_BATTERY_100_OFF] = { 6, 8, "x" },
	
	[LAYER_DAY] = { 3, 0, "DY" },
		
	[SPLASH_TWEAKED] = {  0, 0, "TWEAKED" },
	[SPLASH_BY] = {  1, 8, "BY" },
	[SPLASH_IHTNC] = {  3, 3, "IHTNC" },
	[SPLASH_SMART] = {  6, 7, "SMART" },
	[SPLASH_SQUARE] = {  7, 6, "SQUARE" },
	[SPLASH_C] = {  8, 5, "(C)2014" },
};

static Window *window;
static TextLayer *layers[LAYER_COUNT];
static Layer *minute_layer;
static int minute_num;
static InverterLayer *inverter;

static bool is_init_complete;
static bool is_splash_showing;
static bool is_stat_showing;
static bool is_charging;
static int8_t charging_state;
static bool show_stat;
static AppTimer *timer;

static int8_t current_day;

static void reset_words();
static void display_stat();
static void refresh_layers();
static void face_init();
static void splash_deinit();
static void handle_timer(void *data);

static void determine_invert_status()
{
	bool invert;
	int mode = get_invert_mode_value();
	
	if(mode == INVERT_ON_AM)
	{
		time_t t = time(NULL);
		struct tm *local = localtime(&t);
		invert = (local->tm_hour < 12);
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
	layer_mark_dirty(inverter_layer_get_layer(inverter));
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

static void handle_tap(AccelAxisType axis, int32_t direction)
{
	if(is_stat_showing == true) return;
	
	reset_words();
	display_stat();
	refresh_layers();
}

static void field_changed(const uint32_t key, const void *old_value, const void *new_value)
{
	if(key == CONFIG_KEY_INVERTMODE)
	{
		determine_invert_status();
	}
}

static void replace_text(int word_index) 
{
	const word_t *const w = &words[word_index];
	text_t *cur_t = &current_text[w->row];
		
	for(int i = 0; i < (int)strlen(w->text); i++)
	{
		cur_t->text[w->col + i] = w->text[i];
	}	
}

static void replace_day(int day) 
{
	if(day < 1 || day > 31) return;
	
	const word_t *const w = &words[LAYER_DAY];
	text_t *cur_t = &current_text[w->row];
	
	cur_t->text[w->col] = '0' + (day / 10);
	cur_t->text[w->col + 1] = '0' + (day % 10);
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
static void display_time()
{
	time_t t = time(NULL);
    struct tm *local = localtime(&t);
	int hour = local->tm_hour;
	int min = local->tm_min;
	
	current_day = local->tm_mday;
	
	replace_text(LAYER_IT);
	replace_text(LAYER_IS);
	
	if (min < 5) 
	{
		replace_text(LAYER_M_OCLOCK);
	}
	else if (min < 10) 
	{
		replace_text(LAYER_M_FIVE);
		replace_text(LAYER_PAST);
	}
	else if (min < 15) 
	{
		replace_text(LAYER_M_TEN);
		replace_text(LAYER_PAST);
	}
	else if (min < 20) 
	{
		replace_text(LAYER_A);
		replace_text(LAYER_M_QUARTER);
		replace_text(LAYER_PAST);
	}
	else if (min < 25) 
	{
		replace_text(LAYER_M_TWENTY);
		replace_text(LAYER_PAST);
	}
	else if (min < 30) 
	{
		replace_text(LAYER_M_TWENTY);
		replace_text(LAYER_M_FIVE);
		replace_text(LAYER_PAST);
	}
	else if (min < 35) 
	{
		replace_text(LAYER_M_HALF);
		replace_text(LAYER_PAST);
	}
	else if (min < 40) 
	{
		replace_text(LAYER_M_TWENTY);
		replace_text(LAYER_M_FIVE);
		replace_text(LAYER_TO);
		hour++;
	}
	else if (min < 45) 
	{
		replace_text(LAYER_M_TWENTY);
		replace_text(LAYER_TO);
		hour++;
	}
	else if (min < 50) 
	{
		replace_text(LAYER_A);
		replace_text(LAYER_M_QUARTER);
		replace_text(LAYER_TO);
		hour++;
	}
	else if (min < 55) 
	{
		replace_text(LAYER_M_TEN);
		replace_text(LAYER_TO);
		hour++;
	}
	else 
	{
		replace_text(LAYER_M_FIVE);
		replace_text(LAYER_TO);
		hour++;
	}

	// Convert from 24-hour to 12-hour time
	if (hour == 0) hour = 12;
	else if (hour > 12) hour -= 12;

	replace_text(hour);
}

static void reset_text(int index)
{
	const text_t *const def_t = &default_text[index];
	text_t *cur_t = &current_text[index];
	
	for(int i = 0; i < (int)strlen(def_t->text); i++) 
	{
		cur_t->text[i] = def_t->text[i];
	}
}

static void reset_words()
{
	for (int i = 0; i < (int)LAYER_COUNT; i++) reset_text(i);
}

/** Called when watch is tapped or for each animation frame
............
............
............
DY..........
............
............
........XXXX
............
.....X......
*/
static void display_stat()
{
	uint8_t charge = 0;
	
	//at the start of the animation, retrieve the battery charge
	if(charging_state == 0)
	{
		BatteryChargeState batt = battery_state_service_peek();
		if(batt.is_charging == true) is_charging = true;
		
		charge = batt.charge_percent;
	}
	
	replace_text(LAYER_BATTERY_100_OFF);
	replace_text(LAYER_BATTERY_75_OFF);
	replace_text(LAYER_BATTERY_50_OFF);
	replace_text(LAYER_BATTERY_25_OFF);
	
	//if there's no animation, show the appropriate charge level
	//otherwise, show increasing charge level at every call
	if((is_charging == false && charge >= 75)
	   || (is_charging == true && charging_state == 5)
	   || (is_charging == true && charging_state == 0)) replace_text(LAYER_BATTERY_100);
	
	if((is_charging == false && charge >= 50)
	   || (is_charging == true && charging_state == 4)
	   || (is_charging == true && charging_state == 0)) replace_text(LAYER_BATTERY_75);
	
	if((is_charging == false && charge >= 25)
	   || (is_charging == true && charging_state == 3)
	   || (is_charging == true && charging_state == 0)) replace_text(LAYER_BATTERY_50);
	
	if((is_charging == false && charge > 0)
	   || (is_charging == true && charging_state == 2)
	   || (is_charging == true && charging_state == 0)) replace_text(LAYER_BATTERY_25);
	
	//at the start of the animation, show bt status
	if(charging_state == 0)
	{
		bool bt = bluetooth_connection_service_peek();
	
		if(bt == true) replace_text(LAYER_BT_ON);
		else replace_text(LAYER_BT_OFF);
		
		replace_day(current_day);
	}
	
	if(is_charging == true) timer = app_timer_register(250, handle_timer, NULL);
	else timer = app_timer_register(1500, handle_timer, NULL);
}

static void handle_timer(void *data)
{
    timer = NULL;
	determine_invert_status();
	
	if(is_splash_showing == true)
	{
		//this condition is called after the splash screen is shown
		//this clears the screen and starts the timer for initializing the actual watchface
		is_splash_showing = false;
		
		splash_deinit();
		
		timer = app_timer_register(750, handle_timer, NULL);
	}
	else if(is_init_complete == false)
	{
		//this condition is called after the splash screen has been cleared
		//this initializes the watchface
		face_init();
		
		is_init_complete = true;
	}
	else if(is_charging == true && charging_state <= 5)
	{
		//this condition is called when the status screen is animating
		charging_state++;
		display_stat();
		refresh_layers();
	}
	else
	{
		//this condition is called after the status screen has been shown
		//this always shows the current time
		reset_words();
		display_time();
		refresh_layers();
		layer_mark_dirty(minute_layer);
				
		show_stat = false;
		is_stat_showing = false;
		is_charging = false;
		charging_state = 0; //reset the animation
		is_charging = false;
	}
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) 
{
	if(show_stat == true || is_init_complete == false) return;
	
	minute_num = tick_time->tm_min % 5;
	
	//every five minutes, the screen will clear
	//and be updated with the current time eventually
	if(tick_time->tm_min % 5 == 0) 
	{
		reset_words();
		refresh_layers();
		timer = app_timer_register(750, handle_timer, NULL);
	}
	else
	{
		layer_mark_dirty(minute_layer);
	}
}

static void layer_refresh(int index)
{
	const text_t *const cur_t = &current_text[index];
	
	text_layer_set_text(layers[index], cur_t->text);
	layer_mark_dirty(text_layer_get_layer(layers[index]));
}

static void refresh_layers()
{
	for (unsigned i = 0 ; i < LAYER_COUNT ; i++) 
	{
		layer_refresh(i);
	}
}

static void face_deinit()
{	
	tick_timer_service_unsubscribe();
	accel_tap_service_unsubscribe();
	
	layer_destroy(minute_layer);
}

static void face_init()
{	
	// Create a graphics layer for the entire background
	minute_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(minute_layer, minute_layer_update);
	layer_insert_below_sibling(minute_layer, inverter_layer_get_layer(inverter));
	
	reset_words();
	display_time();
	refresh_layers();
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
	accel_tap_service_subscribe(handle_tap);
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
	
	determine_invert_status();
}

static void display_splash()
{
	replace_text(SPLASH_TWEAKED);
	replace_text(SPLASH_BY);
	replace_text(SPLASH_IHTNC);
	replace_text(SPLASH_SMART);
	replace_text(SPLASH_SQUARE);
	replace_text(SPLASH_C);
	
	refresh_layers();
	
	is_splash_showing = true;
}

static void splash_deinit()
{	
	reset_words();
	refresh_layers();
}

static void splash_init()
{
	display_splash();
	
	timer = app_timer_register(2250, handle_timer, NULL);
}

static void layer_deinit(int index)
{
	layer_remove_from_parent(text_layer_get_layer(layers[index]));
	text_layer_destroy(layers[index]);
}

static void layer_init(int index) 
{	
	GRect frame = GRect(0, index*FONT_H - 2,
						12*(FONT_W+4), FONT_H+8);
	layers[index] = text_layer_create(frame);
	
	text_layer_set_text(layers[index], "");
	text_layer_set_text_color(layers[index], GColorWhite);
	text_layer_set_background_color(layers[index], GColorClear);
	text_layer_set_font(layers[index], font);
	
	layer_insert_below_sibling(text_layer_get_layer(layers[index]), inverter_layer_get_layer(inverter));
	
	reset_text(index);
	layer_refresh(index);
}

static void deinit() 
{
	if(timer != NULL)
	{
		app_timer_cancel(timer);
    	free(timer); 
	}
	
	face_deinit();
	
	for (unsigned i = 0 ; i < LAYER_COUNT ; i++) 
	{
		layer_deinit(i);
	}
	
	btmonitor_deinit();
    inverter_deinit();
	thincfg_deinit();
		
	window_destroy(window);
	
	fonts_unload_custom_font(font);
}

static void init() 
{
	is_init_complete = false;
	is_stat_showing = false;
	charging_state = 0;
	is_charging = false;
	show_stat = false;
	
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_stack_push(window, true);

	thincfg_init();
    thincfg_subscribe((ThinCFGCallbacks) { .field_changed = field_changed, });
	
	inverter_init();
	btmonitor_init(get_bt_notification_value());
	
	font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SOURCE_20));

	for (unsigned i = 0 ; i < LAYER_COUNT ; i++) 
	{
		layer_init(i);
	}
	
	splash_init();
}

int main(void) 
{
	init();
	app_event_loop();
	deinit();
}

