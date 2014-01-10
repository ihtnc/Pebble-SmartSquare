/** \file
 * Word clock, like the qlock2.
 */
#include <pebble.h>

#define ROWS 10
#define COLS 11
#define FONT_H 18
#define FONT_W 12
#define FONT_ON		RESOURCE_ID_FONT_SOURCECODEPRO_BLACK_20
#define FONT_OFF	RESOURCE_ID_FONT_SOURCECODEPRO_LIGHT_20
static GFont font_on;
static GFont font_off;

typedef struct {
	int row;
	int col;
	char text_on[8];
	char text_off[8];
} word_t;

#define LAYER_FIVE	13
#define LAYER_TEN	14
#define LAYER_QUARTER	15
#define LAYER_HALF	16
#define LAYER_TWENTY	17
#define LAYER_IT	 0
#define LAYER_IS	18
#define LAYER_PAST	19
#define LAYER_TO	20
#define LAYER_OCLOCK	21
#define LAYER_A		22
#define LAYER_FILLER	23

/** Square:
  012345678901
0 ITxISxpebble
1 AxQUARTERTEN
2 xTWENTYxFIVE
3 HALFxPASTTOx
4 SEVENFIVETWO
5 ONESIXELEVEN
6 TENxNINEFOUR
7 EIGHTTWELVEx
8 THREExOCLOCK
  012345678901
*/

static const word_t words[] = {
	// Hours 1-12
	[1] = {  5, 0, "ONE", "one" },
	[2] = {  4, 9, "TWO", "two" },
	[3] = {  8, 0, "THREE", "three" },
	[4] = {  6, 8, "FOUR", "four" },
	[5] = {  4, 5, "FIVE", "five" }, // hour
	[6] = {  5, 3, "SIX", "six" },
	[7] = {  4, 0, "SEVEN", "seven" },
	[8] = {  7, 0, "EIGHT", "eight" },
	[9] = {  6, 4, "NINE", "nine" },
	[10] = {  6, 0, "TEN", "ten" }, // hour
	[11] = {  5, 6, "ELEVEN", "eleven" },
	[12] = {  7, 5, "TWELVE", "twelve" },

	// Minutes 13-
	[LAYER_FIVE]	= {  2, 8, "FIVE", "five" }, // minute
	[LAYER_TEN]	= {  1, 9, "TEN", "ten" }, // minute
	[LAYER_A]	= {  1, 0, "A", "a" },
	[LAYER_QUARTER]	= {  1, 2, "QUARTER", "quarter" },
	[LAYER_HALF]	= {  3, 0, "HALF", "half" },
	[LAYER_TWENTY]	= {  2, 1, "TWENTY", "twenty" },

	// Relative
	[LAYER_IT] = {  0, 0, "IT", "it" },
	[LAYER_PAST]	= {  3, 5, "PAST", "past" },
	[LAYER_TO]	= {  3, 9, "TO", "to", },
	[LAYER_OCLOCK]	= {  8, 6, "OCLOCK", "oclock" },
	[LAYER_IS]	= {  0, 3, "IS", "is" },

	// Fillers
	[LAYER_FILLER] =
	{  0, 2, "o", "z" },
	{  0, 5, "apebble", "apebble" },
	{  1, 1, "T", "t" },
	{  2, 0, "K", "k" },
	{  2, 7, "N", "n" },
	{  3, 4, "B", "o" },
	{  3, 11, "F", "f" },
	{  6, 3, "E", "e" },
	{  7, 11, "S", "s" },
	{  8, 5, "H", "h" },
};

#define WORD_COUNT ((sizeof(words) / sizeof(*words)))

static Window *window;
static TextLayer *layers[WORD_COUNT];
static Layer *minute_layer;
static int minute_num;

void text_layer_setup(Window *window, int which, GRect frame, GFont font) {
	layers[which] = text_layer_create(frame);
	text_layer_set_text(layers[which], "");
	text_layer_set_text_color(layers[which], GColorWhite);
	text_layer_set_background_color(layers[which], GColorClear);
	text_layer_set_font(layers[which], font);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(layers[which]));
}


/** Draw a box in a corner to indicate the number of minutes past the five.
 */
static void minute_layer_update(Layer * const me,  GContext *ctx) {
	const int w = 4;
	GRect r = GRect(0, 0, w, w);

	if (minute_num == 0) {
		return; // nothing to draw
	} else if (minute_num == 1) {
		r.origin = GPoint(0,0);
	} else if (minute_num == 2) {
		r.origin = GPoint(144-w,0);
	} else if (minute_num == 3) {
		r.origin = GPoint(144-w,168-w);
	} else if (minute_num == 4) {
		r.origin = GPoint(0,168-w);
	}
	
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, r, 1, GCornersAll);
}


static void word_mark(int which, int on) {
	TextLayer * const layer = layers[which];
	const word_t * const w = &words[which];

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
static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	int hour = tick_time->tm_hour;
	int min = tick_time->tm_min;

	// mark all of the minutes as off,
	// and then turn on the ones that count
	word_mark(LAYER_OCLOCK, 0);
	word_mark(LAYER_FIVE, 0);
	word_mark(LAYER_TEN, 0);
	word_mark(LAYER_A, 0);
	word_mark(LAYER_QUARTER, 0);
	word_mark(LAYER_TWENTY, 0);
	word_mark(LAYER_HALF, 0);
	word_mark(LAYER_PAST, 0);
	word_mark(LAYER_TO, 0);

	if (min < 5) {
		word_mark(LAYER_OCLOCK, 1);
	} else if (min < 10) {
		word_mark(LAYER_FIVE, 1);
		word_mark(LAYER_PAST, 1);
	} else if (min < 15) {
		word_mark(LAYER_TEN, 1);
		word_mark(LAYER_PAST, 1);
	} else if (min < 20) {
		word_mark(LAYER_A, 1);
		word_mark(LAYER_QUARTER, 1);
		word_mark(LAYER_PAST, 1);
	} else if (min < 25) {
		word_mark(LAYER_TWENTY, 1);
		word_mark(LAYER_PAST, 1);
	} else if (min < 30) {
		word_mark(LAYER_TWENTY, 1);
		word_mark(LAYER_FIVE, 1);
		word_mark(LAYER_PAST, 1);
	} else if (min < 35) {
		word_mark(LAYER_HALF, 1);
		word_mark(LAYER_PAST, 1);
	} else if (min < 40) {
		word_mark(LAYER_TWENTY, 1);
		word_mark(LAYER_FIVE, 1);
		word_mark(LAYER_TO, 1);
		hour++;
	} else if (min < 45) {
		word_mark(LAYER_TWENTY, 1);
		word_mark(LAYER_TO, 1);
		hour++;
	} else if (min < 50) {
		word_mark(LAYER_A, 1);
		word_mark(LAYER_QUARTER, 1);
		word_mark(LAYER_TO, 1);
		hour++;
	} else if (min < 55) {
		word_mark(LAYER_TEN, 1);
		word_mark(LAYER_TO, 1);
		hour++;
	} else {
		word_mark(LAYER_FIVE, 1);
		word_mark(LAYER_TO, 1);
		hour++;
	}

	// update the minute box
	minute_num = min % 5;
	layer_mark_dirty(minute_layer);

	// Convert from 24-hour to 12-hour time
	if (hour == 0) {
		hour = 12;
	} else if (hour > 12) {
		hour -= 12;
	}

	// light up the one hour marker
	for (int i = 1 ; i <= 12 ; i++) {
		word_mark(i, i == hour ? 1 : 0);
	}
}


static void word_layer_init(int which) {
	const word_t * const w = &words[which];

	GRect frame = GRect(
		w->col*FONT_W,
		w->row*FONT_H - 2,
		strlen(w->text_on)*(FONT_W+4),
		FONT_H+8
	);

	text_layer_setup(window, which, frame, font_off);
	word_mark(which, 0); // all are "off" initially
}


static void word_layer_deinit(int which) {
	text_layer_destroy(layers[which]);
}

static void init() {
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_stack_push(window, true);

	font_on = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ON_20));
	font_off = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_OFF_20));

	for (unsigned i = 0 ; i < WORD_COUNT ; i++) {
		word_layer_init(i);
	}

	// Flag the ones that are always on
	word_mark(LAYER_IT, 1);
	word_mark(LAYER_IS, 1);

	// Create a graphics layer for the entire background
	minute_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(minute_layer, minute_layer_update);
	layer_add_child(window_get_root_layer(window), minute_layer);

	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}


static void deinit() {
	tick_timer_service_unsubscribe();
	layer_destroy(minute_layer);
	
	for (unsigned i = 0 ; i < WORD_COUNT ; i++) {
		word_layer_deinit(i);
	}
	
	window_destroy(window);
	
	fonts_unload_custom_font(font_on);
	fonts_unload_custom_font(font_off);
}
	

int main(void) {
	init();
	app_event_loop();
	deinit();
}
