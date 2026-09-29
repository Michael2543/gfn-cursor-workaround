# gfn-cursor-workaround

Workaround for a mouse cursor bug in the GeForce NOW Flatpak on Linux (X11): software-rendered color cursors are sometimes displayed incorrectly there (e.g. shown as a plain arrow instead of the actual icon, or with wrong pixels). This project loads a small shim library (`cursor_arrow.c`) via `LD_PRELOAD` that intercepts `SDL_CreateColorCursor` and instead creates the cursor as a native Xcursor ARGB image through X11.

## Download

> **Recommended:** Instead of cloning the repository and building it yourself, just download the ready-made archive:
>
> [`gfn-cursor-fix-0.1.0-linux-x86_64.tar.gz`](gfn-cursor-fix-0.1.0-linux-x86_64.tar.gz)

Extract the archive and run `install.sh` to install the fix and create an application menu entry, or run `launch.sh` directly – the shared library is rebuilt automatically if needed.

## Requirements

- GeForce NOW as a Flatpak (`com.nvidia.geforcenow`), installed for `--user` or `--system`
- An X11 session
- `cc` (GCC/Clang) plus the development packages for `libX11`, `libXcursor` and `SDL2`

## Usage

### Install (recommended)

```sh
./install.sh
```

This installs the shim library and `launch.sh` to `$XDG_DATA_HOME/gfn-cursor-fix` (defaults to `~/.local/share/gfn-cursor-fix`) and creates a `.desktop` file, adding a **"GeForce NOW (Cursor Fix)"** entry to your application menu. Run `uninstall.sh` to remove it again.

### Run directly

```sh
./launch.sh
```

This builds `libgfn_cursor_arrow.so` if needed, starts GeForce NOW via Flatpak, and sets `LD_PRELOAD` as well as `GFN_CURSOR_MODE` for the client process.

The cursor mode can optionally be controlled via an environment variable:

```sh
GFN_CURSOR_MODE=arrow ./launch.sh   # disables the Xcursor workaround, uses the default arrow
GFN_CURSOR_MODE=xcursor ./launch.sh # default: native ARGB cursor via Xcursor
```

## How it works

`cursor_arrow.c` overrides `SDL_CreateColorCursor` via symbol interposition (`LD_PRELOAD`). Instead of using the color cursor rendered by the GeForce NOW client, the cursor image is converted into a native `XcursorImage` and set as the system cursor via Xlib/Xcursor. If a condition isn't met (e.g. no X11, no ARGB support, `GFN_CURSOR_MODE=arrow`), the code falls back to the original behavior.

## License

No license specified.
