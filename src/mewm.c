#define _POSIX_C_SOURCE 199309L
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/XF86keysym.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xft/Xft.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include "mewm.h"

static client       *list = {0}, *ws_list[10] = {0}, *cur,
                    *ws_last_focus[10] = {0};
static int          ws = 1, sw, sh, wx, wy, numlock = 0;
static unsigned int ww, wh;

static Display      *d;
static XButtonEvent mouse;
static Window       root;

static void (*events[LASTEvent])(XEvent *e) = {
    [ButtonPress]      = button_press,
    [ButtonRelease]    = button_release,
    [ConfigureRequest] = configure_request,
    [KeyPress]         = key_press,
    [MapRequest]       = map_request,
    [MappingNotify]    = mapping_notify,
    [DestroyNotify]    = notify_destroy,
    [EnterNotify]      = notify_enter,
    [MotionNotify]     = notify_motion,
    [Expose]           = expose
};

#include "config.h"

void win_focus(client *c) {
    cur = c;
    ws_last_focus[ws] = c;
    XSetInputFocus(d, cur->w, RevertToParent, CurrentTime);
}

void notify_destroy(XEvent *e) {
    win_del(e->xdestroywindow.window);

    if (list) win_focus(list->prev);
}

void notify_enter(XEvent *e) {
    while(XCheckTypedEvent(d, EnterNotify, e));

    for win if (c->w == e->xcrossing.window) win_focus(c);
}

void notify_motion(XEvent *e) {
    if (!mouse.subwindow || cur->f) return;

    while(XCheckTypedEvent(d, MotionNotify, e));

    int xd = e->xbutton.x_root - mouse.x_root;
    int yd = e->xbutton.y_root - mouse.y_root;

    XMoveResizeWindow(d, mouse.subwindow,
        wx + (mouse.button == 1 ? xd : 0),
        wy + (mouse.button == 1 ? yd : 0),
        MAX(1, ww + (mouse.button == 3 ? xd : 0)),
        MAX(1, wh + (mouse.button == 3 ? yd : 0)));
}

void key_press(XEvent *e) {
    if (overlay_mode) {
        KeySym k = XLookupKeysym(&e->xkey, 0);
        
        if (k == XK_Escape) {
            overlay_hide();
            return;
        }
        
        if (k == XK_BackSpace) {
            if (overlay_input[1] != 0) {
                overlay_input[1] = 0;
            } else if (overlay_input[0] != 0) {
                overlay_input[0] = 0;
            }
            overlay_draw();
            return;
        }

        char ch = 0;
        if (k >= '0' && k <= '9') ch = (char)k;
        else if (k >= 'a' && k <= 'z') ch = (char)k;
        else if (k >= 'A' && k <= 'Z') ch = (char)(k - 'A' + 'a');
        else return;

        int found = 0;
        for (int r = 0; r < GRID_ROWS && !found; r++)
            for (int c = 0; c < GRID_COLS && !found; c++)
                if (grid_chars[r][c] == ch) found = 1;
        if (!found) return;

        if (overlay_input[0] == 0) {
            overlay_input[0] = ch;
            overlay_draw();
        } else if (overlay_input[1] == 0) {
            overlay_input[1] = ch;
            overlay_draw();
            
            // Небольшая задержка для визуализации
            struct timespec ts = {
                .tv_sec = 0,
                .tv_nsec = 150000 * 1000
            };
            nanosleep(&ts, NULL);
            
            overlay_process_input();
            overlay_hide();
        }
        return;
    }

    KeySym keysym = XkbKeycodeToKeysym(d, e->xkey.keycode, 0, 0);

    for (unsigned int i=0; i < sizeof(keys)/sizeof(*keys); ++i)
        if (keys[i].keysym == keysym &&
            mod_clean(keys[i].mod) == mod_clean(e->xkey.state))
            keys[i].function(keys[i].arg);
}

void button_press(XEvent *e) {
    if (!e->xbutton.subwindow) return;

    win_size(e->xbutton.subwindow, &wx, &wy, &ww, &wh);
    XRaiseWindow(d, e->xbutton.subwindow);
    mouse = e->xbutton;
}

void button_release(XEvent *e) {
    mouse.subwindow = 0;
}

void win_add(Window w) {
    client *c;

    if (!(c = (client *) calloc(1, sizeof(client))))
        exit(1);

    c->w = w;

    if (list) {
        list->prev->next = c;
        c->prev          = list->prev;
        list->prev       = c;
        c->next          = list;

    } else {
        list = c;
        list->prev = list->next = list;
    }

    ws_save(ws);
}

void win_del(Window w) {
    client *x = 0;

    for win if (c->w == w) x = c;

    if (!list || !x)  return;
    if (x->prev == x) list = 0;
    if (list == x)    list = x->next;
    if (x->next)      x->next->prev = x->prev;
    if (x->prev)      x->prev->next = x->next;

    free(x);
    ws_save(ws);
}

void win_kill(const Arg arg) {
    if (cur) XKillClient(d, cur->w);
}

void win_center(const Arg arg) {
    if (!cur) return;

    win_size(cur->w, &(int){0}, &(int){0}, &ww, &wh);
    XMoveWindow(d, cur->w, (sw - ww) / 2, (sh - wh) / 2);
}

void win_fs(const Arg arg) {
    if (!cur) return;

    if ((cur->f = cur->f ? 0 : 1)) {
        win_size(cur->w, &cur->wx, &cur->wy, &cur->ww, &cur->wh);
        XMoveResizeWindow(d, cur->w, 0, 0, sw, sh);
        XRaiseWindow(d, cur->w);
    } else {
        XMoveResizeWindow(d, cur->w, cur->wx, cur->wy, cur->ww, cur->wh);
        XRaiseWindow(d, cur->w);
    }
}

void win_to_ws(const Arg arg) {
    int tmp = ws;

    if (arg.i == tmp) return;

    ws_sel(arg.i);
    win_add(cur->w);
    ws_save(arg.i);

    ws_sel(tmp);
    win_del(cur->w);
    XUnmapWindow(d, cur->w);
    ws_save(tmp);

    if (list) win_focus(list);
}

void win_prev(const Arg arg) {
    if (!cur || !list || !cur->prev) return;

    client *target = cur->prev;
    if (target == cur) return;

    XRaiseWindow(d, target->w);
    win_focus(cur->prev);
}

void win_next(const Arg arg) {
    if (!cur || !list || !cur->next) return;

    client *target = cur->next;
    if (target == cur) return;

    XRaiseWindow(d, target->w);
    win_focus(cur->next);
}

void ws_go(const Arg arg) {
    if (arg.i == ws) return;

    ws_save(ws);
    int old_ws = ws;

    ws_sel(arg.i);
    for win XMapWindow(d, c->w);

    ws_sel(old_ws);
    for win XUnmapWindow(d, c->w);

    ws_sel(arg.i);

    // if (list) win_focus(list); else cur = 0;
    if (!list) {
        cur = 0;
        ws_last_focus[ws] = 0;
        return;
    }

    client *found = NULL;
    for win if (c == ws_last_focus[ws]) found = c;
    client *target = found ? found : list;

    cur = target;
    XSetInputFocus(d, cur->w, RevertToParent, CurrentTime);
    ws_last_focus[ws] = cur;
}

void configure_request(XEvent *e) {
    XConfigureRequestEvent *ev = &e->xconfigurerequest;

    XConfigureWindow(d, ev->window, ev->value_mask, &(XWindowChanges) {
        .x          = ev->x,
        .y          = ev->y,
        .width      = ev->width,
        .height     = ev->height,
        .sibling    = ev->above,
        .stack_mode = ev->detail
    });
}

void map_request(XEvent *e) {
    Window w = e->xmaprequest.window;

    XSelectInput(d, w, StructureNotifyMask|EnterWindowMask);
    win_size(w, &wx, &wy, &ww, &wh);
    win_add(w);
    cur = list->prev;

    if (wx + wy == 0) win_center((Arg){0});

    XMapWindow(d, w);
    win_focus(list->prev);
}

void mapping_notify(XEvent *e) {
    XMappingEvent *ev = &e->xmapping;

    if (ev->request == MappingKeyboard || ev->request == MappingModifier) {
        XRefreshKeyboardMapping(ev);
        input_grab(root);
    }
}

void run(const Arg arg) {
    if (fork()) return;
    if (d) close(ConnectionNumber(d));

    setsid();
    execvp((char*)arg.com[0], (char**)arg.com);
}

void input_grab(Window root) {
    unsigned int i, j, modifiers[] = {0, LockMask, numlock, numlock|LockMask};
    XModifierKeymap *modmap = XGetModifierMapping(d);
    KeyCode code;

    for (i = 0; i < 8; i++)
        for (int k = 0; k < modmap->max_keypermod; k++)
            if (modmap->modifiermap[i * modmap->max_keypermod + k]
                == XKeysymToKeycode(d, 0xff7f))
                numlock = (1 << i);

    XUngrabKey(d, AnyKey, AnyModifier, root);

    for (i = 0; i < sizeof(keys)/sizeof(*keys); i++)
        if ((code = XKeysymToKeycode(d, keys[i].keysym)))
            for (j = 0; j < sizeof(modifiers)/sizeof(*modifiers); j++)
                XGrabKey(d, code, keys[i].mod | modifiers[j], root,
                        True, GrabModeAsync, GrabModeAsync);

    for (i = 1; i < 4; i += 2)
        for (j = 0; j < sizeof(modifiers)/sizeof(*modifiers); j++)
            XGrabButton(d, i, MOD | modifiers[j], root, True,
                ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
                GrabModeAsync, GrabModeAsync, 0, 0);

    XFreeModifiermap(modmap);
}

// ============================================================================
// OVERLAY GRID FUNCTIONS FOR SOWM
// ============================================================================

void overlay_draw(void) {
    if (!overlay_win) return;

    XClearWindow(d, overlay_win);

    int cell_w = (sw - padding * (GRID_COLS + 1)) / GRID_COLS;
    int cell_h = (sh - padding * (GRID_ROWS + 1)) / GRID_ROWS;

    int r1 = -1, c1 = -1, r2 = -1, c2 = -1;
    if (overlay_input[0]) {
        for (int r = 0; r < GRID_ROWS; r++)
            for (int c = 0; c < GRID_COLS; c++)
                if (grid_chars[r][c] == overlay_input[0]) { r1 = r; c1 = c; }
    }
    if (overlay_input[1]) {
        for (int r = 0; r < GRID_ROWS; r++)
            for (int c = 0; c < GRID_COLS; c++)
                if (grid_chars[r][c] == overlay_input[1]) { r2 = r; c2 = c; }
    }

    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            int x = padding + c * (cell_w + padding);
            int y = padding + r * (cell_h + padding);

            int is_selected = 0;
            if (r1 >= 0 && c1 >= 0) {
                if (r2 >= 0 && c2 >= 0) {
                    int min_r = r1 < r2 ? r1 : r2;
                    int max_r = r1 > r2 ? r1 : r2;
                    int min_c = c1 < c2 ? c1 : c2;
                    int max_c = c1 > c2 ? c1 : c2;
                    if (r >= min_r && r <= max_r && c >= min_c && c <= max_c)
                        is_selected = 1;
                } else if (r == r1 && c == c1) {
                    is_selected = 1;
                }
            }

            if (is_selected) {
                XSetForeground(d, gc, xft_col_selection.pixel);
                XFillRectangle(d, overlay_win, gc, x, y, cell_w, cell_h);
            }

            XSetForeground(d, gc, xft_col_foreground.pixel);
            XDrawRectangle(d, overlay_win, gc, x, y, cell_w, cell_h);

            if (font && xftdraw) {
                char txt[2] = {grid_chars[r][c], 0};
                XGlyphInfo extents;
                XftTextExtentsUtf8(d, font, (FcChar8*)txt, strlen(txt), &extents);

                int tx = x + (cell_w - extents.width) / 2;
                int ty = y + (cell_h - extents.height) / 2 + extents.y;

                XftDrawStringUtf8(xftdraw, &xft_col_foreground, font, tx, ty,
                                (FcChar8*)txt, strlen(txt));
            }
        }
    }

    if (overlay_input[0] || overlay_input[1]) {
        char status[64];
        snprintf(status, sizeof(status), "Input: %c%c",
                overlay_input[0] ? overlay_input[0] : ' ',
                overlay_input[1] ? overlay_input[1] : ' ');

        if (font && xftdraw) {
            XftDrawStringUtf8(xftdraw, &xft_col_foreground, font, 
                            20, sh - 20,
                            (FcChar8*)status, strlen(status));
        }
    }

    XFlush(d);
}

void overlay_enter(const Arg arg) {
    if (!cur) return;

    overlay_mode = 1;
    memset(overlay_input, 0, sizeof(overlay_input));

    if (!overlay_win) {
        XSetWindowAttributes wa = {
            .override_redirect = True,
            .background_pixel = xft_col_background.pixel,
            .event_mask = ExposureMask | KeyPressMask
        };
        overlay_win = XCreateWindow(d, root, 0, 0, sw, sh, 0,
            CopyFromParent, InputOutput, CopyFromParent,
            CWOverrideRedirect | CWBackPixel | CWEventMask, &wa);

        gc = XCreateGC(d, overlay_win, 0, NULL);

        Visual *visual = DefaultVisual(d, DefaultScreen(d));
        Colormap cmap = DefaultColormap(d, DefaultScreen(d));
        xftdraw = XftDrawCreate(d, overlay_win, visual, cmap);
    } else {
        XMoveResizeWindow(d, overlay_win, 0, 0, sw, sh);
        XClearWindow(d, overlay_win);
    }

    XMapRaised(d, overlay_win);
    XSetInputFocus(d, overlay_win, RevertToPointerRoot, CurrentTime);
    overlay_draw();
}

void overlay_hide(void) {
    overlay_mode = 0;
    memset(overlay_input, 0, sizeof(overlay_input));
    if (overlay_win) {
        XUnmapWindow(d, overlay_win);
    }
    if (cur) {
        XSetInputFocus(d, cur->w, RevertToParent, CurrentTime);
    }
}

void overlay_process_input(void) {
    if (!cur || overlay_input[0] == 0 || overlay_input[1] == 0) return;

    int r1 = -1, c1 = -1, r2 = -1, c2 = -1;
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid_chars[r][c] == overlay_input[0]) { r1 = r; c1 = c; }
            if (grid_chars[r][c] == overlay_input[1]) { r2 = r; c2 = c; }
        }
    }
    if (r1 == -1 || r2 == -1) return;

    if (r1 > r2) { int t = r1; r1 = r2; r2 = t; }
    if (c1 > c2) { int t = c1; c1 = c2; c2 = t; }

    int cols_span = c2 - c1 + 1;
    int rows_span = r2 - r1 + 1;

    int cell_w = (sw - padding * (GRID_COLS + 1)) / GRID_COLS;
    int cell_h = (sh - padding * (GRID_ROWS + 1)) / GRID_ROWS;

    int x = padding + c1 * (cell_w + padding);
    int y = padding + r1 * (cell_h + padding);
    int w = cols_span * cell_w + (cols_span - 1) * padding;
    int h = rows_span * cell_h + (rows_span - 1) * padding;

    XMoveResizeWindow(d, cur->w, x, y, w, h);
    win_focus(cur);
}

void expose(XEvent *e) {
    if (e->xexpose.window == overlay_win && overlay_mode) {
        overlay_draw();
    }
}

int main(void) {
    XEvent ev;

    if (!(d = XOpenDisplay(0))) exit(1);

    signal(SIGCHLD, SIG_IGN);
    XSetErrorHandler(xerror);

    int s = DefaultScreen(d);
    root  = RootWindow(d, s);
    sw    = XDisplayWidth(d, s);
    sh    = XDisplayHeight(d, s);

    Colormap cmap = DefaultColormap(d, DefaultScreen(d));
	XColor color;

	if (XParseColor(d, cmap, root_background, &color) &&
		XAllocColor(d, cmap, &color)) {
		XSetWindowBackground(d, root, color.pixel);
		XClearWindow(d, root);
	}

    XSelectInput(d,  root, SubstructureRedirectMask);
    XDefineCursor(d, root, XCreateFontCursor(d, 68));
    input_grab(root);
    Visual *visual = DefaultVisual(d, s);
    cmap = DefaultColormap(d, s);
    XftColorAllocName(d, visual, cmap, ow_background, &xft_col_background);
    XftColorAllocName(d, visual, cmap, ow_foreground, &xft_col_foreground);
    XftColorAllocName(d, visual, cmap, ow_selection, &xft_col_selection);
    font = XftFontOpenName(d, s, overlay_font);

    while (1 && !XNextEvent(d, &ev)) // 1 && will forever be here.
        if (events[ev.type]) events[ev.type](&ev);
}
