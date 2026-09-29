# GeForce NOW Cursor Fix for Linux

A local workaround for the official `com.nvidia.geforcenow` Flatpak. On some
X11/XWayland desktops, the app turns the streamed game cursor into a two-color
shape. This launcher passes the intact cursor bitmap directly to Xcursor, so
its texture and transparency remain visible.

## Download

> **Recommended:** Instead of cloning the repository, just download the ready-made archive:
>
> [`gfn-cursor-fix-0.1.0-linux-x86_64.tar.gz`](gfn-cursor-fix-0.1.0-linux-x86_64.tar.gz)

Extract the archive and run `install.sh` to create an application menu entry, or run `launch.sh` directly – the shared library is rebuilt automatically if needed.


## Requirements

- Linux x86_64 and the official GeForce NOW Flatpak
- X11 or XWayland with ARGB cursor support
- `libXcursor.so.1` and `libX11.so.6` in the Flatpak runtime

Tested with GeForce NOW 2.0.89.141 (bundled SDL 2.32.10) on Hyprland/XWayland.
The workaround is not specific to Omarchy. Native Wayland and other client
versions are untested. A GeForce NOW update may require a new build.

## Run without installing

Extract the archive somewhere under your home directory, then run:

```sh
./launch.sh
```

## Add an application-menu entry

```sh
./install.sh
```

Start **GeForce NOW (Cursor Fix)** from your application menu. `./uninstall.sh`
removes this menu entry and the installed copy. The original GeForce NOW
Flatpak is never modified. Starting it normally bypasses the workaround.

If the game cursor still fails, `GFN_CURSOR_MODE=arrow ./launch.sh` shows a
plain arrow as a fallback.

## Build from source

The prebuilt library is included. Rebuilding requires a C compiler and the
SDL2, Xcursor, and X11 development headers:

```sh
cc -shared -fPIC -O2 -Wall -Wextra -o libgfn_cursor_arrow.so cursor_arrow.c -ldl -lXcursor -lX11
```

The launcher starts `/app/cef/GeForceNOW` directly because NVIDIA's wrapper
clears `LD_PRELOAD`. The wrapper's built-in self-update does not run through
this launcher; update the Flatpak normally.
