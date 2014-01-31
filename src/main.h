#include "pebble.h"
	
#define ROWS 10
#define COLS 11
#define FONT_H 18
#define FONT_W 12

#define DEFAULT_TEXT_1 0
#define DEFAULT_TEXT_2 1
#define DEFAULT_TEXT_3 2
#define DEFAULT_TEXT_4 3
#define DEFAULT_TEXT_5 4
#define DEFAULT_TEXT_6 5
#define DEFAULT_TEXT_7 6
#define DEFAULT_TEXT_8 7
#define DEFAULT_TEXT_9 8

#define LAYER_H_01 1
#define LAYER_H_02 2
#define LAYER_H_03 3
#define LAYER_H_04 4
#define LAYER_H_05 5
#define LAYER_H_06 6
#define LAYER_H_07 7
#define LAYER_H_08 8
#define LAYER_H_09 9
#define LAYER_H_10 10
#define LAYER_H_11 11
#define LAYER_H_12 12

#define LAYER_M_OCLOCK 	13
#define LAYER_M_FIVE 	14
#define LAYER_M_TEN 	15
#define LAYER_M_QUARTER 16
#define LAYER_M_TWENTY 	17
#define LAYER_M_HALF 	18
	
#define LAYER_IT	 	19
#define LAYER_IS		20
#define LAYER_A			21
#define LAYER_PAST		22
#define LAYER_TO		23
	
#define LAYER_BT_ON		24
#define LAYER_BT_OFF	25

#define LAYER_BATTERY_25	26
#define LAYER_BATTERY_50	27
#define LAYER_BATTERY_75	28
#define LAYER_BATTERY_100	29

#define LAYER_BATTERY_25_OFF 30
#define LAYER_BATTERY_50_OFF 31
#define LAYER_BATTERY_75_OFF 32
#define LAYER_BATTERY_100_OFF 33

#define LAYER_DAY 34
	
#define SPLASH_TWEAKED	35
#define SPLASH_BY		36
#define SPLASH_IHTNC	37
#define SPLASH_SMART	38
#define SPLASH_SQUARE	39
#define SPLASH_C		40

typedef struct
{
	char text[12];
} text_t;

typedef struct
{
	int row;
	int col;
	char text[12];
} word_t;
	
#define SCREEN_HEIGHT 168
#define SCREEN_WIDTH 144

