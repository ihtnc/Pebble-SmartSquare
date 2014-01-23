#include "pebble.h"
	
#define ROWS 10
#define COLS 11
#define FONT_H 18
#define FONT_W 12
#define FONT_ON		RESOURCE_ID_FONT_SOURCECODEPRO_BLACK_20
#define FONT_OFF	RESOURCE_ID_FONT_SOURCECODEPRO_LIGHT_20

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
	
#define LAYER_M_05 13
#define LAYER_M_10 14
#define LAYER_M_15 15
#define LAYER_M_20 16
#define LAYER_M_30 17
#define LAYER_M_35 18
#define LAYER_M_40 19
#define LAYER_M_50 20

#define LAYER_Y 0
#define LAYER_ALA 21
#define LAYER_ALAS 22
	
#define LAYER_BT_OFF 23
#define LAYER_BT_ON 24
	
#define LAYER_BATTERY_25 25
#define LAYER_BATTERY_50 26
#define LAYER_BATTERY_75 27
#define LAYER_BATTERY_100 28

#define LAYER_BATTERY_25_OFF 29
#define LAYER_BATTERY_50_OFF 30
#define LAYER_BATTERY_75_OFF 31
#define LAYER_BATTERY_100_OFF 32
	
#define LAYER_DAY 33

#define SPLASH_TWEAKED 34
#define SPLASH_BY 35
#define SPLASH_IHTNC 36
#define SPLASH_SMART 37
#define SPLASH_SQUARE 38
#define SPLASH_C 39

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
