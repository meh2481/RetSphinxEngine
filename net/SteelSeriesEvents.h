#pragma once

//JSON keys for SteelSeries testing
#define JSON_KEY_BLUE "blue"
#define JSON_KEY_COLOR "color"
#define JSON_KEY_DATAS "datas"
#define JSON_KEY_DEVICE_TYPE "device-type"
#define JSON_KEY_EVENT "event"
#define JSON_KEY_FREQUENCY "frequency"
#define JSON_KEY_GAME "game"
#define JSON_KEY_GRADIENT "gradient"
#define JSON_KEY_GREEN "green"
#define JSON_KEY_HANDLERS "handlers"
#define JSON_KEY_HAS_TEXT "has-text"
#define JSON_KEY_HUNDRED "hundred"
#define JSON_KEY_ICON_ID "icon-id"
#define JSON_KEY_LENGTH_MILLIS "length-millis"
#define JSON_KEY_LENGTH_MS "length-ms"
#define JSON_KEY_MAX_VALUE "max_value"
#define JSON_KEY_MIN_VALUE "min_value"
#define JSON_KEY_MODE "mode"
#define JSON_KEY_PATTERN "pattern"
#define JSON_KEY_PREFIX	"prefix"
#define JSON_KEY_RATE "rate"
#define JSON_KEY_RED "red"
#define JSON_KEY_REPEAT_LIMIT "repeat_limit"
#define JSON_KEY_SUFFIX	"suffix"
#define JSON_KEY_TYPE "type"
#define JSON_KEY_ZERO "zero"
#define JSON_KEY_ZONE "zone"

//Other keys for SteelSeries testing
#define TYPE_MOUSE "mouse"
#define TYPE_SCREENED "screened"
#define TYPE_TACTILE "tactile"
#define ZONE_ONE "one"
#define MODE_COLOR "color"
#define MODE_SCREEN "screen"
#define MODE_VIBRATE "vibrate"

#define EVENT_TYPE_CUSTOM "custom"

static const char* steelSeriesTactileEvents[] = {
    EVENT_TYPE_CUSTOM,
    "ti_predefined_strongclick_100",
    "ti_predefined_strongclick_60",
    "ti_predefined_strongclick_30",
    "ti_predefined_sharpclick_100",
    "ti_predefined_sharpclick_60",
    "ti_predefined_sharpclick_30",
    "ti_predefined_softbump_100",
    "ti_predefined_softbump_60",
    "ti_predefined_softbump_30",
    "ti_predefined_doubleclick_100",
    "ti_predefined_doubleclick_60",
    "ti_predefined_tripleclick_100",
    "ti_predefined_softfuzz_60",
    "ti_predefined_strongbuzz_100",
    "ti_predefined_buzzalert750ms",
    "ti_predefined_buzzalert1000ms",
    "ti_predefined_strongclick1_100",
    "ti_predefined_strongclick2_80",
    "ti_predefined_strongclick3_60",
    "ti_predefined_strongclick4_30",
    "ti_predefined_mediumclick1_100",
    "ti_predefined_mediumclick2_80",
    "ti_predefined_mediumclick3_60",
    "ti_predefined_sharptick1_100",
    "ti_predefined_sharptick2_80",
    "ti_predefined_sharptick3_60",
    "ti_predefined_shortdoubleclickstrong1_100",
    "ti_predefined_shortdoubleclickstrong2_80",
    "ti_predefined_shortdoubleclickstrong3_60",
    "ti_predefined_shortdoubleclickstrong4_30",
    "ti_predefined_shortdoubleclickmedium1_100",
    "ti_predefined_shortdoubleclickmedium2_80",
    "ti_predefined_shortdoubleclickmedium3_60",
    "ti_predefined_shortdoublesharptick1_100",
    "ti_predefined_shortdoublesharptick2_80",
    "ti_predefined_shortdoublesharptick3_60",
    "ti_predefined_longdoublesharpclickstrong1_100",
    "ti_predefined_longdoublesharpclickstrong2_80",
    "ti_predefined_longdoublesharpclickstrong3_60",
    "ti_predefined_longdoublesharpclickstrong4_30",
    "ti_predefined_longdoublesharpclickmedium1_100",
    "ti_predefined_longdoublesharpclickmedium2_80",
    "ti_predefined_longdoublesharpclickmedium3_60",
    "ti_predefined_longdoublesharptick1_100",
    "ti_predefined_longdoublesharptick2_80",
    "ti_predefined_longdoublesharptick3_60",
    "ti_predefined_buzz1_100",
    "ti_predefined_buzz2_80",
    "ti_predefined_buzz3_60",
    "ti_predefined_buzz4_40",
    "ti_predefined_buzz5_20",
    "ti_predefined_pulsingstrong1_100",
    "ti_predefined_pulsingstrong2_60",
    "ti_predefined_pulsingmedium1_100",
    "ti_predefined_pulsingmedium2_60",
    "ti_predefined_pulsingsharp1_100",
    "ti_predefined_pulsingsharp2_60",
    "ti_predefined_transitionclick1_100",
    "ti_predefined_transitionclick2_80",
    "ti_predefined_transitionclick3_60",
    "ti_predefined_transitionclick4_40",
    "ti_predefined_transitionclick5_20",
    "ti_predefined_transitionclick6_10",
    "ti_predefined_transitionhum1_100",
    "ti_predefined_transitionhum2_80",
    "ti_predefined_transitionhum3_60",
    "ti_predefined_transitionhum4_40",
    "ti_predefined_transitionhum5_20",
    "ti_predefined_transitionhum6_10",
    "ti_predefined_transitionrampdownlongsmooth1_100to0",
    "ti_predefined_transitionrampdownlongsmooth2_100to0",
    "ti_predefined_transitionrampdownmediumsmooth1_100to0",
    "ti_predefined_transitionrampdownmediumsmooth2_100to0",
    "ti_predefined_transitionrampdownshortsmooth1_100to0",
    "ti_predefined_transitionrampdownshortsmooth2_100to0",
    "ti_predefined_transitionrampdownlongsharp1_100to0",
    "ti_predefined_transitionrampdownlongsharp2_100to0",
    "ti_predefined_transitionrampdownmediumsharp1_100to0",
    "ti_predefined_transitionrampdownmediumsharp2_100to0",
    "ti_predefined_transitionrampdownshortsharp1_100to0",
    "ti_predefined_transitionrampdownshortsharp2_100to0",
    "ti_predefined_transitionrampuplongsmooth1_0to100",
    "ti_predefined_transitionrampuplongsmooth2_0to100",
    "ti_predefined_transitionrampupmediumsmooth1_0to100",
    "ti_predefined_transitionrampupmediumsmooth2_0to100",
    "ti_predefined_transitionrampupshortsmooth1_0to100",
    "ti_predefined_transitionrampupshortsmooth2_0to100",
    "ti_predefined_transitionrampuplongsharp1_0to100",
    "ti_predefined_transitionrampuplongsharp2_0to100",
    "ti_predefined_transitionrampupmediumsharp1_0to100",
    "ti_predefined_transitionrampupmediumsharp2_0to100",
    "ti_predefined_transitionrampupshortsharp1_0to100",
    "ti_predefined_transitionrampupshortsharp2_0to100",
    "ti_predefined_transitionrampdownlongsmooth1_50to0",
    "ti_predefined_transitionrampdownlongsmooth2_50to0",
    "ti_predefined_transitionrampdownmediumsmooth1_50to0",
    "ti_predefined_transitionrampdownmediumsmooth2_50to0",
    "ti_predefined_transitionrampdownshortsmooth1_50to0",
    "ti_predefined_transitionrampdownshortsmooth2_50to0",
    "ti_predefined_transitionrampdownlongsharp1_50to0",
    "ti_predefined_transitionrampdownlongsharp2_50to0",
    "ti_predefined_transitionrampdownmediumsharp1_50to0",
    "ti_predefined_transitionrampdownmediumsharp2_50to0",
    "ti_predefined_transitionrampdownshortsharp1_50to0",
    "ti_predefined_transitionrampdownshortsharp2_50to0",
    "ti_predefined_transitionrampuplongsmooth1_0to50",
    "ti_predefined_transitionrampuplongsmooth2_0to50",
    "ti_predefined_transitionrampupmediumsmooth1_0to50",
    "ti_predefined_transitionrampupmediumsmooth2_0to50",
    "ti_predefined_transitionrampupshortsmooth1_0to50",
    "ti_predefined_transitionrampupshortsmooth2_0to50",
    "ti_predefined_transitionrampuplongsharp1_0to50",
    "ti_predefined_transitionrampuplongsharp2_0to50",
    "ti_predefined_transitionrampupmediumsharp1_0to50",
    "ti_predefined_transitionrampupmediumsharp2_0to50",
    "ti_predefined_transitionrampupshortsharp1_0to50",
    "ti_predefined_transitionrampupshortsharp2_0to50",
    "ti_predefined_longbuzzforprogrammaticstopping_100",
    "ti_predefined_smoothhum1nokickorbrakepulse_50",
    "ti_predefined_smoothhum2nokickorbrakepulse_40",
    "ti_predefined_smoothhum3nokickorbrakepulse_30",
    "ti_predefined_smoothhum4nokickorbrakepulse_20",
    "ti_predefined_smoothhum5nokickorbrakepulse_10"
};

static const char* steelSeriesEventIcons[] = {
    "None",
    "Health",
    "Armor",
    "Ammo",
    "Money",
    "Explosion",
    "Skull",
    "Headshot",
    "Helmet",
    "UNDEFINED",	//Blank space there but no icon for id=9
    "Hunger",
    "Air",
    "Compass",
    "Pickaxe",
    "Potion",
    "Clock",
    "Lightning",
    "Backpack"
};

static const char* steelSeriesColorZones[] = {
    "wheel",
    "logo",
    "base"
};


