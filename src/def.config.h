#ifndef CONFIG_H
#define CONFIG_H

#define MOD Mod1Mask

const char ow_background[]   = "#1e1e2e";
const char ow_foreground[]   = "#cdd6f4";
const char ow_selection[]    = "#89b4fa";
const char root_background[] = "#1e1e2e";
const char overlay_font[] = "monospace:size=24:antialias=true";

#define GRID_ROWS 3
#define GRID_COLS 4

static const char grid_chars[GRID_ROWS][GRID_COLS] = {
	{'q', 'w', 'e', 'r'},
	{'a', 's', 'd', 'f'},
	{'z', 'x', 'c', 'v'}
};

const char* menu[]    = {"dmenu_run",      0};
const char* term[]    = {"st",             0};

static struct key keys[] = {
    {MOD,      XK_q,   win_kill,   {0}},
    {MOD,      XK_c,   win_center, {0}},
    {MOD,      XK_f,   win_fs,     {0}},
    {MOD,      XK_a,   overlay_enter,   {0}},

    {MOD,           XK_Tab, win_next,   {0}},
    {MOD|ShiftMask, XK_Tab, win_prev,   {0}},

    {MOD, XK_d,      run, {.com = menu}},
    {MOD, XK_Return, run, {.com = term}},

    {MOD,           XK_1, ws_go,     {.i = 1}},
    {MOD|ShiftMask, XK_1, win_to_ws, {.i = 1}},
    {MOD,           XK_2, ws_go,     {.i = 2}},
    {MOD|ShiftMask, XK_2, win_to_ws, {.i = 2}},
    {MOD,           XK_3, ws_go,     {.i = 3}},
    {MOD|ShiftMask, XK_3, win_to_ws, {.i = 3}},
    {MOD,           XK_4, ws_go,     {.i = 4}},
    {MOD|ShiftMask, XK_4, win_to_ws, {.i = 4}},
    {MOD,           XK_5, ws_go,     {.i = 5}},
    {MOD|ShiftMask, XK_5, win_to_ws, {.i = 5}},
    {MOD,           XK_6, ws_go,     {.i = 6}},
    {MOD|ShiftMask, XK_6, win_to_ws, {.i = 6}},
};

#endif
