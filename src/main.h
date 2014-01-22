#include "pebble.h"
	
#define ROWS 10
#define COLS 11
#define FONT_H 18
#define FONT_W 12
#define FONT_ON		RESOURCE_ID_FONT_SOURCECODEPRO_BLACK_20
#define FONT_OFF	RESOURCE_ID_FONT_SOURCECODEPRO_LIGHT_20

#define LAYER_01_O		1
#define LAYER_01_NE		2
	
#define LAYER_02_T		3
#define LAYER_02_WO		4
	
#define LAYER_03_T		5
#define LAYER_03_HR		6
#define LAYER_03_E1		7
#define LAYER_03_E2		8
	
#define LAYER_04_F		9
#define LAYER_04_O		10
#define LAYER_04_UR		11
	
#define LAYER_05_F		12
#define LAYER_05_IVE	13
	
#define LAYER_06_S		14
#define LAYER_06_I		15
#define LAYER_06_X		16
	
#define LAYER_07_S		17
#define LAYER_07_E		18
#define LAYER_07_V		19
#define LAYER_07_EN		20
	
#define LAYER_08_E		21
#define LAYER_08_IG		22
#define LAYER_08_H		23
#define LAYER_08_T		24
	
#define LAYER_09_NI		25
#define LAYER_09_N		26
#define LAYER_09_E		27
	
#define LAYER_10_T		28
#define LAYER_10_E		29
#define LAYER_10_N		30
	
#define LAYER_11_EL		31
#define LAYER_11_E		32
#define LAYER_11_VEN	33
	
#define LAYER_12_T		34
#define LAYER_12_WE		35
#define LAYER_12_L		36
#define LAYER_12_VE		37

#define LAYER_FIVE		38
#define LAYER_TEN		39
#define LAYER_QUARTER	40
#define LAYER_H			41
#define LAYER_AL		42
#define LAYER_F			43
#define LAYER_TWENTY	44
	
#define LAYER_IT	 	45
#define LAYER_IS		46
#define LAYER_P			47
#define LAYER_AS		48
#define LAYER_T			49
#define LAYER_TO		50
#define LAYER_OC		51
#define LAYER_L			52
#define LAYER_OCK		53
#define LAYER_A			54
	
#define LAYER_BT		55
#define LAYER_BT_ON		56
#define LAYER_BT_OFF	57
#define LAYER_BATTERY_25	58
#define LAYER_BATTERY_50	59
#define LAYER_BATTERY_75	60
#define LAYER_BATTERY_100	61

#define LAYER_FILLER_X	62
#define LAYER_FILLER	63

#define SPLASH_TWEAKED	0
#define SPLASH_BY		1
#define SPLASH_IHTNC	2
#define SPLASH_SMART	3
#define SPLASH_SQUARE	4
#define SPLASH_C		5
#define SPLASH_FILLER	6
	
typedef struct
{
	int row;
	int col;
	char text_on[12];
	char text_off[12];
} word_t;
	
#define SCREEN_HEIGHT 168
#define SCREEN_WIDTH 144
