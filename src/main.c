/** \file
 * Word clock, like the qlock2.
 */
#include "main.h"
#include "btmonitor.h"
#include "thincfg.h"
#include "options.h"

static GFont font;

/** 
  Time:          Status:        Square:        Splash:
  012345678901   012345678901   012345678901   012345678901 
0 ALASUNADIYES 0 ............ 0 ALASUNADIYES 0 tweakedDIYES 0  
1 DOSEKWATRO.. 1 ..........DY 1 DOSEKWATRODY 1 DOSEKWARbyFF 1  
2 SINGKOTSONSE 2 ............ 2 SINGKOTSONSE 2 SINGKOTSONSE 2  
3 TRESAISIYETE 3 ............ 3 TRESAISIYETE 3 TREihtncYETE 3  
4 NUWEBE.BENTE 4 ............ 4 NUWEBEwBENTE 4 NUWEBEWBENTE 4  
5 DIYESIKWENTA 5 ............ 5 DIYESIKWENTA 5 DIYESIKWENTA 5  
6 ....KWARENTA 6 XXXX........ 6 XXXXKWARENTA 6 XXXXKWAsmart 6  
7 TRENTAKINSEY 7 ............ 7 TRENTAKINSEY 7 TRENTAsquare 7  
8 SINGKO.MEDYA 8 ......X..... 8 SINGKO!MEDYA 8 phNGK(c)2014 8  
  012345678901   012345678901   012345678901   012345678901    
*/

static const text_t default_text[] =
{
	[DEFAULT_TEXT_1] = { "alasunadiyes" },
	[DEFAULT_TEXT_2] = { "dosekwatrody" },
	[DEFAULT_TEXT_3] = { "singkotsonse" },
	[DEFAULT_TEXT_4] = { "tresaisiyete" },
	[DEFAULT_TEXT_5] = { "nuwebewbente" },
	[DEFAULT_TEXT_6] = { "diyesikwenta" },
	[DEFAULT_TEXT_7] = { "xxxxkwarenta" },
	[DEFAULT_TEXT_8] = { "trentakinsey" },
    [DEFAULT_TEXT_9] = { "singkoxmedya" },
};

static text_t current_text[] =
{
	[DEFAULT_TEXT_1] = { "alasunadiyes" },
	[DEFAULT_TEXT_2] = { "dosekwatrody" },
	[DEFAULT_TEXT_3] = { "singkotsonse" },
	[DEFAULT_TEXT_4] = { "tresaisiyete" },
	[DEFAULT_TEXT_5] = { "nuwebewbente" },
	[DEFAULT_TEXT_6] = { "diyesikwenta" },
	[DEFAULT_TEXT_7] = { "xxxxkwarenta" },
	[DEFAULT_TEXT_8] = { "trentakinsey" },
    [DEFAULT_TEXT_9] = { "singkoxmedya" },
};

#define LAYER_COUNT ((sizeof(default_text) / sizeof(*default_text)))
	
static const word_t words[] =
{
	// Hours 1-12
	[LAYER_H_01] = { 0, 4, "UNA" },
	[LAYER_H_02] = { 1, 0, "DOS" },
	[LAYER_H_03] = { 3, 0, "TRES" },
	[LAYER_H_04] = { 1, 4, "KWATRO" },
	[LAYER_H_05] = { 2, 0, "SINGKO" },
	[LAYER_H_06] = { 3, 3, "SAIS" },
	[LAYER_H_07] = { 3, 6, "SIYETE" },
	[LAYER_H_08] = { 2, 5, "OTSO" },
	[LAYER_H_09] = { 4, 0, "NUWEBE" },
	[LAYER_H_10] = { 0, 7, "DIYES" },
	[LAYER_H_11] = { 2, 8, "ONSE" },
	[LAYER_H_12] = { 1, 0, "DOSE" },
	
	// Minutes
	[LAYER_M_05] = { 8, 0, "SINGKO" },
	[LAYER_M_10] = { 5, 0, "DIYES" },
	[LAYER_M_15] = { 7, 6, "KINSE" },
	[LAYER_M_20] = { 4, 7, "BENTE" },
	[LAYER_M_30] = { 8, 7, "MEDYA" },
	[LAYER_M_35] = { 7, 0, "TRENTA" },
	[LAYER_M_40] = { 6, 4, "KWARENTA" },
	[LAYER_M_50] = { 5, 4, "SIKWENTA" },

	[LAYER_ALA] = { 0, 0, "ALA" },
	[LAYER_ALAS] = { 0, 0, "ALAS" },
	[LAYER_Y] = { 7, 11, "Y" },
	
	[LAYER_BT_OFF] = { 8, 6, "X" },
	[LAYER_BT_ON] = { 8, 6, "x" },
	
	[LAYER_BATTERY_25] = { 6, 0, ":" },
	[LAYER_BATTERY_50] = { 6, 1, ":" },
	[LAYER_BATTERY_75] = { 6, 2, ":" },
	[LAYER_BATTERY_100] = { 6, 3, ":" },
	
	[LAYER_BATTERY_25_OFF] = { 6, 0, "x" },
	[LAYER_BATTERY_50_OFF] = { 6, 1, "x" },
	[LAYER_BATTERY_75_OFF] = { 6, 2, "x" },
	[LAYER_BATTERY_100_OFF] = { 6, 3, "x" },
	
	[LAYER_DAY] = { 1, 10, "dy" },
		
	[SPLASH_TWEAKED] = {  0, 0, "TWEAKED" },
	[SPLASH_BY] = {  1, 8, "BY" },
	[SPLASH_IHTNC] = {  3, 3, "IHTNC" },
	[SPLASH_SMART] = {  6, 7, "SMART" },
	[SPLASH_SQUARE] = {  7, 6, "SQUARE" },
	[SPLASH_C] = {  8, 0, "PHngk(C)2014" },
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
0-4 "ALA/ALAS X"
5-9 "ALA/ALAS X SINGKO"
10-14 "ALA/ALAS X DIYES"
15-19 "ALA/ALAS X KINSE"
20-24 "ALA/ALAS X BENTE"
25-29 "ALA/ALAS X BENTE SINGKO"
30-34 "ALA/ALAS X Y MEDYA"
35-39 "ALA/ALAS X TRENTA Y SINGKO"
40-44 "ALA/ALAS X KWARENTA"
45-49 "ALA/ALAS X KWARENTA Y SINGKO"
50-54 "ALA/ALAS X SIKWENTA"
55-59 "ALA/ALAS X SIKWENTA Y SINGKO"
 */
static void display_time()
{
	time_t t = time(NULL);
    struct tm *local = localtime(&t);
	int hour = local->tm_hour;
	int min = local->tm_min;
	
	current_day = local->tm_mday;
	
	if(min < 5)
	{
		//do nothing since this would be covered on the hour part 
	}
	else if (min < 10) 
	{
		replace_text(LAYER_M_05);
	}
	else if (min < 15) 
	{
		replace_text(LAYER_M_10);
	}
	else if (min < 20) 
	{
		replace_text(LAYER_M_15);
	}
	else if (min < 25) 
	{
		replace_text(LAYER_M_20);
	}
	else if (min < 30) 
	{
		replace_text(LAYER_M_20);
		replace_text(LAYER_M_05);
	}
	else if (min < 35) 
	{
		replace_text(LAYER_Y);
		replace_text(LAYER_M_30);
	}
	else if (min < 40) 
	{
		replace_text(LAYER_M_35);
		replace_text(LAYER_Y);
		replace_text(LAYER_M_05);
	}
	else if (min < 45) 
	{
		replace_text(LAYER_M_40);
	}
	else if (min < 50) 
	{
		replace_text(LAYER_M_40);
		replace_text(LAYER_Y);
		replace_text(LAYER_M_05);
	}
	else if (min < 55) 
	{
		replace_text(LAYER_M_50);
	}
	else
	{
		replace_text(LAYER_M_50);
		replace_text(LAYER_Y);
		replace_text(LAYER_M_05);
	}

	// Convert from 24-hour to 12-hour time
	if (hour == 0) hour = 12;
	else if (hour > 12) hour -= 12;

	if (hour == 1) replace_text(LAYER_ALA);
	else replace_text(LAYER_ALAS);
		
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
..........DY
............
............
............
............
XXXX........
............
......X.....
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
	
	//at the start of the animation, show bt status and the day of the month
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
