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
  Time:          Status:        Square:        Splash:
  012345678901   012345678901   012345678901   012345678901 
0 IT.IS...A... 0 .....BT:.OFF 0 IThISBT:AOFF 0 tweaked:AOFF 0  
1 QUARTERTEN.. 1 ..........ON 1 QUARTERTENON 1 QUARTERTbyON 1  
2 TWENTY.FIVE. 2 ...........# 2 TWENTYrFIVE# 2 TWENTYRFIVE# 2  
3 HALF.PASTTO. 3 HALF.PAST..# 3 HALFhPASTTO# 3 HALihtncTTO# 3  
4 NINETENFOUR. 4 N..E.E..O..# 4 NINETENFOUR# 4 NINETENFOUR# 4  
5 EIGHTTWELVE. 5 EIGH.TWEL..# 5 EIGHTTWELVE# 5 EIGHTTWELVE# 5  
6 ONESIXELEVEN 6 O..S.X..E... 6 ONESIXELEVEN 6 ONESIXELword 6  
7 TWOSEVENFIVE 7 T..S.V..F... 7 TWOSEVENFIVE 7 TWOSEVsquare 7  
8 THREE.OCLOCK 8 THRE.XOCL... 8 THREExOCLOCK 8 THREE(c)2014 8  
  012345678901   012345678901   012345678901   012345678901    
  
Phase 2 (with overlap)
  Time:          Status:        Square:        Splash:
  012345678901   012345678901   012345678901   012345678901 
0 ITxISxxxAxxx 0 xxxxxBT:xOFF 0 IThISBT:AOFF 0 tweakedtAOFF 0  
1 QUARTERTENxx 1 xxxxxxxxxxON 1 QUARTERTENON 1 QUARTERTbyON 1  
2 TWENTYxFIVE# 2 xxxxxxxxxxx# 2 TWENTYsFIVE# 2 TWENTYSFIVE# 2  
3 HALFxPASTox# 3 HALFxPASTxx# 3 HALFhPASTOn# 3 HALihtncTON# 3  
4 FOUREIGHTEN# 4 NxxExExxOxx# 4 FOUREIGHTEN# 4 FOUREIGHTEN# 4  
5 THREExTWONE# 5 EIGHxTWELxx# 5 THREEpTWONE# 5 THREEPTWONE# 5  
6 TWELVExxSIX# 6 OxxSxXxxExx# 6 TWELVEntSIX# 6 TWELVENword# 6  
7 FIVELEVENINE 7 TxxSxVxxFxxx 7 FIVELEVENINE 7 FIVELEsquare 7  
8 SEVENxOCLOCK 8 THRExXOCLxxx 8 SEVENtOCLOCK 8 SEVEN(C)2014 8  
  012345678901   012345678901   012345678901   012345678901    
*/

static const word_t words[] =
{
	// Hours 1-12
	[LAYER_01_O] = { 6, 0, "O", "o" },
	[LAYER_01_NE] = { 6, 1, "NE", "ne" },
		
	[LAYER_02_T] = { 7, 0, "T", "t" },
	[LAYER_02_WO] = { 7, 1, "WO", "wo" },
		
	[LAYER_03_T] = { 8, 0, "T", "t" },
	[LAYER_03_HR] = { 8, 1, "HR", "hr" },
	[LAYER_03_E1] = { 8, 3, "E", "e" },
	[LAYER_03_E2] = { 8, 4, "E", "e" },
		
	[LAYER_04_F] = { 4, 7, "F", "f" },
	[LAYER_04_O] = { 4, 8, "O", "o" },
	[LAYER_04_UR] = { 4, 9, "UR", "ur" },
		
	[LAYER_05_F] = { 7, 8, "F", "f" }, 
	[LAYER_05_IVE] = { 7, 9, "IVE", "ive" }, 
		
	[LAYER_06_S] = { 6, 3, "S", "s" },
	[LAYER_06_I] = { 6, 4, "I", "i" },
	[LAYER_06_X] = { 6, 5, "X", "x" },
		
	[LAYER_07_S] = { 7, 3, "S", "s" },
	[LAYER_07_E] = { 7, 4, "E", "e" },
	[LAYER_07_V] = { 7, 5, "V", "v" },
	[LAYER_07_EN] = { 7, 6, "EN", "en" },
		
	[LAYER_08_E] = { 5, 0, "E", "e" },
	[LAYER_08_IG] = { 5, 1, "IG", "ig" },
	[LAYER_08_H] = { 5, 3, "H", "h" },
	[LAYER_08_T] = { 5, 4, "T", "t" },
		
	[LAYER_09_NI] = { 4, 0, "NI", "ni" },
	[LAYER_09_N] = { 4, 2, "N", "n" },
	[LAYER_09_E] = { 4, 3, "E", "e" },
		
	[LAYER_10_T] = { 4, 4, "T", "t" },
	[LAYER_10_E] = { 4, 5, "E", "e" },
	[LAYER_10_N] = { 4, 6, "N", "n" },
		
	[LAYER_11_EL] = { 6, 6, "EL", "el" },
	[LAYER_11_E] = { 6, 8, "E", "e" },
	[LAYER_11_VEN] = { 6, 9, "VEN", "ven" },
		
	[LAYER_12_T] = { 5, 5, "T", "t" },
	[LAYER_12_WE] = { 5, 6, "WE", "we" },
	[LAYER_12_L] = { 5, 8, "L", "l" },
	[LAYER_12_VE] = { 5, 9, "VE", "ve" },

	// Minutes
	[LAYER_FIVE]	= {  2, 7, "FIVE", "five" }, 
	[LAYER_TEN]	= {  1, 7, "TEN", "ten" }, 
	[LAYER_A]	= {  0, 8, "A", "a" },
	[LAYER_QUARTER]	= {  1, 0, "QUARTER", "quarter" },
	[LAYER_H] = {  3, 0, "H", "h" },
	[LAYER_AL] = {  3, 1, "AL", "al" },
	[LAYER_F] = {  3, 3, "F", "f" },
	[LAYER_TWENTY]	= {  2, 0, "TWENTY", "twenty" },

	// Relative
	[LAYER_IT] = {  0, 0, "IT", "it" },
	[LAYER_IS]	= {  0, 3, "IS", "is" },
	[LAYER_TO]	= {  3, 9, "TO", "to", },
	
	[LAYER_P] = {  3, 5, "P", "p" },
	[LAYER_AS] = {  3, 6, "AS", "as" },
	[LAYER_T] = {  3, 8, "T", "t" },
	
	[LAYER_OC]	= {  8, 6, "OC", "oc" },
	[LAYER_L]	= {  8, 8, "L", "l" },
	[LAYER_OCK]	= {  8, 9, "OCK", "ock" },

	// Status: BT	
	[LAYER_BT] = {  0, 5, "BT:", "bt:" },
	[LAYER_BT_ON] = {  1, 10, "ON", "on" },
	[LAYER_BT_OFF] = {  0, 9, "OFF", "off" },
	
	// Status: Battery	
	[LAYER_BATTERY_25] = {  5, 11, "X", "x" },
	[LAYER_BATTERY_50] = {  4, 11, "X", "x" },
	[LAYER_BATTERY_75] = {  3, 11, "X", "x" },
	[LAYER_BATTERY_100] = {  2, 11, "X", "x" },
	
	[LAYER_FILLER_X] = {  8, 5, "X", "x" },
		
	// Fillers
	[LAYER_FILLER] =
	{  0, 2, "h", "h" },
	{  2, 6, "r", "r" },
	{  3, 4, "h", "h" },
};

#define WORD_COUNT ((sizeof(words) / sizeof(*words)))
	
static const word_t splash[] =
{
	[SPLASH_TWEAKED] = {  0, 0, "TWEAKED", "ithisbt" },
	[SPLASH_BY] = {  1, 8, "BY", "en" },
	[SPLASH_IHTNC] = {  3, 3, "IHTNC", "fhpas" },
	[SPLASH_SMART] = {  6, 7, "SMART", "leven" },
	[SPLASH_SQUARE] = {  7, 6, "SQUARE", "enfive" },
	[SPLASH_C] = {  8, 5, "(c)2014", "xoclock" },
		
	// Fillers
	[SPLASH_FILLER] =
	{  0, 7, ":AOFF", ":aoff" },
	{  1, 0, "QUARTERT", "quartert" },
	{  1, 10, "ON", "on" },
	{  2, 0, "TWENTYRFIVEX", "twentyrfivex" },
	{  3, 0, "HAL", "hal" },
	{  3, 8, "TTOX", "ttox" },
	{  4, 0, "NINETENFOURX", "ninetenfourx" },
	{  5, 0, "EIGHTTWELVEX", "eighttwelvex" },
	{  6, 0, "ONESIXE", "onesixe" },
	{  7, 0, "TWOSEV", "twosev" },
	{  8, 0, "THREE", "three" },
};

#define SPLASH_COUNT ((sizeof(splash) / sizeof(*splash)))
	
static Window *window;
static TextLayer *layers[WORD_COUNT];
static TextLayer *splash_layers[SPLASH_COUNT];
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

static void handle_timer(void *data);
static void handle_tap(AccelAxisType axis, int32_t direction);
static void word_init();
static void splash_deinit();
static void clear_splash();

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

static void text_layer_setup(Window *window, int which, GRect frame, GFont font) 
{
	layers[which] = text_layer_create(frame);
	text_layer_set_text(layers[which], "");
	text_layer_set_text_color(layers[which], GColorWhite);
	text_layer_set_background_color(layers[which], GColorClear);
	text_layer_set_font(layers[which], font);
	layer_insert_below_sibling(text_layer_get_layer(layers[which]), inverter_layer_get_layer(inverter));
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

static void splash_mark(int which, int on) 
{
	TextLayer * const layer = splash_layers[which];
	const word_t * const w = &splash[which];

	text_layer_set_text(layer, on ? w->text_on : w->text_off);
	text_layer_set_font(layer, on ? font_on : font_off);
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
	
	word_mark(LAYER_IT, 1);
	word_mark(LAYER_IS, 1);
	
	if (min < 5) 
	{
		word_mark(LAYER_OC, 1);
		word_mark(LAYER_L, 1);
		word_mark(LAYER_OCK, 1);
	}
	else if (min < 10) 
	{
		word_mark(LAYER_FIVE, 1);
		word_mark(LAYER_P, 1);
		word_mark(LAYER_AS, 1);
		word_mark(LAYER_T, 1);
	}
	else if (min < 15) 
	{
		word_mark(LAYER_TEN, 1);
		word_mark(LAYER_P, 1);
		word_mark(LAYER_AS, 1);
		word_mark(LAYER_T, 1);
	}
	else if (min < 20) 
	{
		word_mark(LAYER_A, 1);
		word_mark(LAYER_QUARTER, 1);
		word_mark(LAYER_P, 1);
		word_mark(LAYER_AS, 1);
		word_mark(LAYER_T, 1);
	}
	else if (min < 25) 
	{
		word_mark(LAYER_TWENTY, 1);
		word_mark(LAYER_P, 1);
		word_mark(LAYER_AS, 1);
		word_mark(LAYER_T, 1);
	}
	else if (min < 30) 
	{
		word_mark(LAYER_TWENTY, 1);
		word_mark(LAYER_FIVE, 1);
		word_mark(LAYER_P, 1);
		word_mark(LAYER_AS, 1);
		word_mark(LAYER_T, 1);
	}
	else if (min < 35) 
	{
		word_mark(LAYER_H, 1);
		word_mark(LAYER_AL, 1);
		word_mark(LAYER_F, 1);
		word_mark(LAYER_P, 1);
		word_mark(LAYER_AS, 1);
		word_mark(LAYER_T, 1);
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

	// Convert from 24-hour to 12-hour time
	if (hour == 0) hour = 12;
	else if (hour > 12) hour -= 12;

	if (hour == 1) 
	{
		word_mark(LAYER_01_O, 1);
		word_mark(LAYER_01_NE, 1);
	}
	else if (hour == 2)
	{
		word_mark(LAYER_02_T, 1);
		word_mark(LAYER_02_WO, 1);
	}
	else if (hour == 3)
	{
		word_mark(LAYER_03_T, 1);
		word_mark(LAYER_03_HR, 1);
		word_mark(LAYER_03_E1, 1);
		word_mark(LAYER_03_E2, 1);
	}
	else if (hour == 4)
	{
		word_mark(LAYER_04_F, 1);
		word_mark(LAYER_04_O, 1);
		word_mark(LAYER_04_UR, 1);
	}
	else if (hour == 5)
	{
		word_mark(LAYER_05_F, 1);
		word_mark(LAYER_05_IVE, 1);
	}
	else if (hour == 6)
	{
		word_mark(LAYER_06_S, 1);
		word_mark(LAYER_06_I, 1);
		word_mark(LAYER_06_X, 1);
	}
	else if (hour == 7)
	{
		word_mark(LAYER_07_S, 1);
		word_mark(LAYER_07_E, 1);
		word_mark(LAYER_07_V, 1);
		word_mark(LAYER_07_EN, 1);
	}
	else if (hour == 8)
	{
		word_mark(LAYER_08_E, 1);
		word_mark(LAYER_08_IG, 1);
		word_mark(LAYER_08_H, 1);
		word_mark(LAYER_08_T, 1);
	}
	else if (hour == 9)
	{
		word_mark(LAYER_09_NI, 1);
		word_mark(LAYER_09_N, 1);
		word_mark(LAYER_09_E, 1);
	}
	else if (hour == 10)
	{
		word_mark(LAYER_10_T, 1);
		word_mark(LAYER_10_E, 1);
		word_mark(LAYER_10_N, 1);
	}
	else if (hour == 11)
	{
		word_mark(LAYER_11_EL, 1);
		word_mark(LAYER_11_E, 1);
		word_mark(LAYER_11_VEN, 1);
	}
	else if (hour == 12)
	{
		word_mark(LAYER_12_T, 1);
		word_mark(LAYER_12_WE, 1);
		word_mark(LAYER_12_L, 1);
		word_mark(LAYER_12_VE, 1);
	}
}

static void clear_words()
{
	layer_set_hidden(minute_layer, true);
	
	for (int i = 0; i < LAYER_FILLER; i++) word_mark(i, 0);
}

/** Called when watch is tapped or for each animation frame
.....BT:.OFF
..........ON
...........#
HALF.PAST..#
N..E.E..O..#
EIGH.TWEL..#
O..S.X..E...
T..S.V..F...
THRE.XOCL...
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
	
	//if there's no animation, show the appropriate charge level
	//otherwise, show increasing charge level at every call
	if((is_charging == false && charge >= 75)
	   || (is_charging == true && charging_state == 5)
	   || (is_charging == true && charging_state == 0)) word_mark(LAYER_BATTERY_100, 1);
	
	if((is_charging == false && charge >= 50)
	   || (is_charging == true && charging_state == 4)
	   || (is_charging == true && charging_state == 0)) word_mark(LAYER_BATTERY_75, 1);
	
	if((is_charging == false && charge >= 25)
	   || (is_charging == true && charging_state == 3)
	   || (is_charging == true && charging_state == 0)) word_mark(LAYER_BATTERY_50, 1);
	
	if((is_charging == false && charge > 0)
	   || (is_charging == true && charging_state == 2)
	   || (is_charging == true && charging_state == 0)) word_mark(LAYER_BATTERY_25, 1);
	
	//at the start of the animation, show bt status
	if(charging_state == 0)
	{
		bool bt = bluetooth_connection_service_peek();
		word_mark(LAYER_BT, 1);
	
		if(bt == true) word_mark(LAYER_BT_ON, 1);
		else word_mark(LAYER_BT_OFF, 1);
	}
	
	//at the start of the animation, show the day of the month
	//highlight parts on the lower left area of the screen to resemble numerals
	if(charging_state == 0)
	{
		uint8_t tens = current_day / 10;
		uint8_t ones = current_day % 10;
		
		if(tens == 1)
		{
			word_mark(LAYER_F, 1);
			word_mark(LAYER_09_E, 1);
			word_mark(LAYER_08_H, 1);
			word_mark(LAYER_06_S, 1);
			word_mark(LAYER_07_S, 1);
			word_mark(LAYER_03_E1, 1);
		}
		else if(tens == 2 || tens == 3)
		{
			word_mark(LAYER_H, 1);
			word_mark(LAYER_AL, 1);
			word_mark(LAYER_F, 1);
			
			word_mark(LAYER_09_E, 1);
			
			word_mark(LAYER_08_E, 1);
			word_mark(LAYER_08_IG, 1);
			word_mark(LAYER_08_H, 1);
			
			if(tens == 2)
			{
				word_mark(LAYER_01_O, 1);
				word_mark(LAYER_02_T, 1);
			}
			if(tens == 3)
			{
				word_mark(LAYER_06_S, 1);
				word_mark(LAYER_07_S, 1);
			}
			
			word_mark(LAYER_03_T, 1);
			word_mark(LAYER_03_HR, 1);
			word_mark(LAYER_03_E1, 1);
		}
		
		if(ones == 1 || ones == 4 || ones == 7)
		{
			if(ones == 4 || ones == 7) word_mark(LAYER_P, 1);
			if(ones == 7) word_mark(LAYER_AS, 1);			
			word_mark(LAYER_T, 1);
			
			if(ones == 4) word_mark(LAYER_10_E, 1);		
			word_mark(LAYER_04_O, 1);
			
			if(ones == 4) 
			{
				word_mark(LAYER_12_T, 1);
				word_mark(LAYER_12_WE, 1);
			}			
			word_mark(LAYER_12_L, 1);
			
			word_mark(LAYER_11_E, 1);
			word_mark(LAYER_05_F, 1);
			
			word_mark(LAYER_L, 1);
		}
		else if(ones == 0 || ones == 2 || ones == 3 
				|| ones == 5 || ones == 6 || ones == 8 
				|| ones == 9)
		{
			word_mark(LAYER_P, 1);
			word_mark(LAYER_AS, 1);
			word_mark(LAYER_T, 1);
			
			if(ones == 0 || ones == 5 || ones == 6 
			   || ones == 8 || ones == 9)
			{
				word_mark(LAYER_10_E, 1);
			}
			
			if(ones == 0 || ones == 2 || ones == 3 
			   || ones == 8 || ones == 9)
			{
				word_mark(LAYER_04_O, 1);
			}
			
			word_mark(LAYER_12_T, 1);
			if (ones == 2 || ones == 3 || ones == 5 
				|| ones == 6 || ones == 8 || ones == 9) word_mark(LAYER_12_WE, 1);
			word_mark(LAYER_12_L, 1);
			
			if(ones == 0 ||ones == 2 || ones == 6 
			   || ones == 8)
			{
				word_mark(LAYER_06_X, 1);
				word_mark(LAYER_07_V, 1);
			}
			
			if(ones == 0 || ones == 3 || ones == 5 
			   || ones == 6 || ones == 8 || ones == 9)
			{
				word_mark(LAYER_11_E, 1);
				word_mark(LAYER_05_F, 1);
			}
			
			word_mark(LAYER_FILLER_X, 1);
			word_mark(LAYER_OC, 1);
			word_mark(LAYER_L, 1);
		}
	}
	
	if(is_charging == true) timer = app_timer_register(250, handle_timer, NULL);
	else timer = app_timer_register(1500, handle_timer, NULL);
}

static void handle_timer(void *data)
{
    app_timer_cancel(timer);
	determine_invert_status();
	
	if(is_splash_showing == true)
	{
		//this condition is called after the splash screen is shown
		//this clears the screen and starts the timer for initializing the actual watchface
		is_splash_showing = false;
		clear_splash();
		timer = app_timer_register(750, handle_timer, NULL);
	}
	else if(is_init_complete == false)
	{
		//this condition is called after the splash screen has been cleared
		//this initializes the watchface
		splash_deinit();
		word_init();
		
		is_init_complete = true;
	}
	else if(is_charging == true && charging_state <= 5)
	{
		//this condition is called when the status screen is animating
		charging_state++;
		display_stat();
	}
	else
	{
		//this condition is called after the status screen has been shown
		//this always shows the current time
		clear_words();
		display_time();
		
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
	
	//every five minutes, the screen will clear
	//and be updated with the current time eventually
	if(tick_time->tm_min % 5 == 0) clear_words();
	timer = app_timer_register(750, handle_timer, NULL);
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
	layer_remove_from_parent(text_layer_get_layer(layers[which]));
	text_layer_destroy(layers[which]);
}

static void word_init()
{
	for (unsigned i = 0 ; i < WORD_COUNT ; i++) 
	{
		word_layer_init(i);
	}
	
	// Create a graphics layer for the entire background
	minute_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(minute_layer, minute_layer_update);
	layer_insert_below_sibling(minute_layer, inverter_layer_get_layer(inverter));
	
	display_time();
	
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
	accel_tap_service_subscribe(handle_tap);
}

static void handle_tap(AccelAxisType axis, int32_t direction)
{
	if(is_stat_showing == true) return;
	
	clear_words();
	display_stat();
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

static void field_changed(const uint32_t key, const void *old_value, const void *new_value)
{
	if(key == CONFIG_KEY_INVERTMODE)
	{
		determine_invert_status();
	}
}

static void display_splash()
{
	splash_mark(SPLASH_TWEAKED, 1);
	splash_mark(SPLASH_BY, 1);
	splash_mark(SPLASH_IHTNC, 1);
	splash_mark(SPLASH_SMART, 1);
	splash_mark(SPLASH_SQUARE, 1);
	splash_mark(SPLASH_C, 1);
	
	is_splash_showing = true;
}

static void clear_splash()
{
	splash_mark(SPLASH_TWEAKED, 0);
	splash_mark(SPLASH_BY, 0);
	splash_mark(SPLASH_IHTNC, 0);
	splash_mark(SPLASH_SMART, 0);
	splash_mark(SPLASH_SQUARE, 0);
	splash_mark(SPLASH_C, 0);
}

static void splash_layer_setup(Window *window, int which, GRect frame, GFont font) 
{
	splash_layers[which] = text_layer_create(frame);
	text_layer_set_text(splash_layers[which], "");
	text_layer_set_text_color(splash_layers[which], GColorWhite);
	text_layer_set_background_color(splash_layers[which], GColorClear);
	text_layer_set_font(splash_layers[which], font);
	layer_insert_below_sibling(text_layer_get_layer(splash_layers[which]), inverter_layer_get_layer(inverter));
}

static void splash_layer_init(int which) 
{
	const word_t * const w = &splash[which];

	GRect frame = GRect(w->col*FONT_W,
						w->row*FONT_H - 2,
						strlen(w->text_on)*(FONT_W+4),
						FONT_H+8);

	splash_layer_setup(window, which, frame, font_off);
	splash_mark(which, 0); // all are "off" initially
}

static void splash_layer_deinit(int which) 
{
	layer_remove_from_parent(text_layer_get_layer(splash_layers[which]));
	text_layer_destroy(splash_layers[which]);
}

static void splash_init()
{
	for (unsigned i = 0 ; i < SPLASH_COUNT ; i++) 
	{
		splash_layer_init(i);
	}
	
	display_splash();
	timer = app_timer_register(2250, handle_timer, NULL);
}

static void splash_deinit()
{
	for (unsigned i = 0 ; i < SPLASH_COUNT ; i++) 
	{
		splash_layer_deinit(i);
	}
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
	
	font_on = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ON_20));
	font_off = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OFF_20));

	splash_init();
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
