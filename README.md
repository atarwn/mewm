# mewm (Minimal Elegant Window Manager)

> **An adequate fusion of gbwm and sowm**

*A tiny floating window manager with grid-based window placement.*

---

## Philosophy

This project started as an attempt to merge sowm with gbwm, but ultimately went the opposite direction-building upon sowm's simplicity to create cleaner, more maintainable code. The result is a minimal floating window manager that adds intelligent grid-based window placement to sowm's already excellent foundation.

## Features

### From sowm
- **Floating only** - No tiling complexity
- **Tiny codebase** - Easy to understand and modify
- **Fullscreen toggle** - One key to maximize
- **Window centering** - Quick window organization
- **Focus follows mouse** - Natural workflow
- **Mix of mouse and keyboard** - Use what works
- **Multiple workspaces** - 9 virtual desktops
- **No cruft** - No ICCCM, minimal EWMH, just what works

### New in mewm
- **Grid overlay system** - Visual grid for precise window placement
- **Two-key placement** - Press two keys to define window position and size
- **Intelligent cell detection** - Windows snap to grid positions
- **Configurable grid layout** - 3×4 default, fully customizable
- **Beautiful overlay** - Semi-transparent with smooth rendering
- **Keyboard-driven sizing** - No mouse needed for window management

## How Grid Placement Works

1. Press `MOD4 + Space` to activate grid overlay
2. Press first key (e.g., `q`) - top-left corner of desired area
3. Press second key (e.g., `s`) - bottom-right corner of desired area
4. Window instantly resizes to span from first to second cell

**Example:** Press `q`, then `v` to make window span the entire screen.

**Default Grid Layout:**
```
┌───┬───┬───┬───┐
│ q │ w │ e │ r │
├───┼───┼───┼───┤
│ a │ s │ d │ f │
├───┼───┼───┼───┤
│ z │ x │ c │ v │
└───┴───┴───┴───┘
```

## Default Keybindings

### Window Management

| Combo                      | Action                        |
| -------------------------- | ----------------------------- |
| `Mouse`                    | Focus under cursor            |
| `MOD4` + `Left Mouse`      | Move window                   |
| `MOD4` + `Right Mouse`     | Resize window                 |
| `MOD4` + `Space`           | **Grid placement mode**       |
| `MOD4` + `f`               | Fullscreen toggle             |
| `MOD4` + `c`               | Center window                 |
| `MOD4` + `q`               | Kill window                   |
| `MOD4` + `1-9`             | Switch to workspace           |
| `MOD4` + `Shift` + `1-9`   | Send window to workspace      |
| `MOD1` + `TAB`             | Focus next window             |
| `MOD1` + `Shift` + `TAB`   | Focus previous window         |

### Grid Overlay (when active)

| Key         | Action                          |
| ----------- | ------------------------------- |
| `ESC`       | Cancel and close overlay        |
| `Backspace` | Clear last input                |
| `q-v`       | Grid cell selection (see above) |

### Programs (customize in config.h)

| Combo                      | Action           | Default Program |
| -------------------------- | ---------------- | --------------- |
| `MOD4` + `Return`          | Terminal         | `st`            |
| `MOD4` + `d`               | Application menu | `dmenu_run`     |

## Dependencies

- `xlib` (usually `libX11`)
- `libXft` (for overlay rendering)

## Installation

1. Copy `def.config.h` to `config.h` and customize to your liking
2. Run `make` to build mewm
3. Copy to your path or run `make install`
   - `DESTDIR` and `PREFIX` are supported

### Starting mewm

**From .xinitrc:**
```sh
exec mewm
```

**With display manager (Ly, LightDM, etc):**

Save to `/usr/share/xsessions/mewm.desktop`:
```ini
[Desktop Entry]
Name=mewm
Comment=Minimal Elegant Window Manager
Exec=mewm
Type=Application
```

## Configuration

All configuration is done in `config.h` before compilation (suckless style).

## Why mewm?

- **Simpler than gbwm** - Removed multi-monitor complexity, EWMH bloat, and unnecessary abstractions
- **Smarter than sowm** - Added intelligent grid-based placement while keeping the minimalist philosophy
- **Easy to hack** - Small codebase, clear structure, well-commented
- **Practical** - Grid overlay makes window management fast and precise
- **Opinionated** - Does one thing (floating windows) and does it well

## Differences from sowm

### Added
- Grid overlay system for precise window placement
- Visual feedback during window sizing
- Configurable grid layouts
- Xft rendering for beautiful text

### Removed
- dylan's programs (like scr, bri and bud) from keys array
- All other sowm functionality is preserved

### Modified
- Enhanced key handling for overlay mode
- Slightly larger binary (but still tiny)

## Differences from gbwm

### Removed
- Multi-monitor support
- Full EWMH implementation
- Tiling modes
- Window arrange/rearrange logic
- Complex monitor tracking

### Kept
- Grid overlay system (the best part!)
- Clean grid-based placement
- Visual feedback

## Technical Details

- **Lines of code:** ~520 (compared to sowm's 220 and gbwm's 1100+)
- **Binary size:** ~30KB (stripped)
- **Memory usage:** <1MB resident
- **Dependencies:** X11, Xft, Xrender
- **Language:** Pure C99

## Limitations

- Single monitor only
- No window decorations yet
- No tiling (use the grid for that)
- No saved layouts (restart and rearrange - builds character)

## Credits & Thanks

**Based on:**
- [sowm](https://github.com/dylanaraps/sowm) by Dylan Araps - The foundation
- [gbwm](https://github.com/atarwn/gbwm) by me! - Grid overlay port

**Also inspired by:**
- [2bwm](https://github.com/venam/2bwm)
- [SmallWM](https://github.com/adamnew123456/SmallWM)
- [berry](https://github.com/JLErvin/berry)
- [catwm](https://github.com/pyknite/catwm)
- [dwm](https://dwm.suckless.org)
- [tinywm](http://incise.org/tinywm.html)

## License

MIT License - See LICENSE file for details

## Contributing

This is a personal project tailored to specific needs, but bug reports and patches are welcome. Keep changes minimal and in the spirit of simplicity.

---

*"The best code is the code you can understand at 3am"*