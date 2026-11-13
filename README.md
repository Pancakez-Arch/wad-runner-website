# WAD Runner (website)

Play Doom, Ultimate Doom, Doom II, Final Doom and Freedoom in the browser with **your own WAD
files**. Everything runs locally in the page; WADs never leave your device.

**Play:** https://pancakez-arch.github.io/wad-runner-website/

- The original Doom engine ([doomgeneric](https://github.com/ozkl/doomgeneric), GPL) compiled to
  WebAssembly. Single-threaded, so it works on any static host
- Keyboard + mouse, on-screen touch controls on phones, installable as an app (PWA), works offline
- Vertical free-look (mouse / PgUp / PgDn), WASD movement
- Music through an emulated OPL (AdLib / Sound Blaster) synth using your WAD's instruments
- Saves and your last WAD are kept in the browser; PWADs and DeHackEd patches supported
- Also plays old IWADs such as Doom v1.1

No game data is included. The Android app version lives in
[wad-runner-android](https://github.com/Pancakez-Arch/wad-runner-android).

## Controls

| Keys | Action |
|---|---|
| ↑ ↓ or W S | Walk |
| ← → or mouse (click the game to capture it) | Turn |
| A D, or Alt + ← → | Strafe |
| Mouse up/down, PgUp / PgDn, End | Look up / down, re-centre |
| Ctrl or left click | Fire |
| Space or right click | Use (doors, switches) |
| Shift | Run |
| 1–7 | Weapons |
| Tab | Automap |
| Esc | Menu (first Esc releases the mouse) |
| F2 / F3 | Save / load |

## Repository layout

| Path | What |
|---|---|
| `web/` | The page (`index.html`), service worker, manifest and icons |
| `engine/` | Doom engine C source (GPL-2.0). Changes are listed in [`engine/CHANGES.md`](engine/CHANGES.md) |
| `scripts/build.sh` | Compiles the engine with Emscripten and assembles `dist/` |
| `.github/workflows/pages.yml` | On every push to `main`: build and deploy to GitHub Pages |

## Building / hosting yourself

```sh
source /path/to/emsdk/emsdk_env.sh   # Emscripten 3.1.74
bash scripts/build.sh                # -> dist/
python3 -m http.server -d dist 8000  # open http://localhost:8000
```

`dist/` is plain static files: any web server works (no special headers needed). It has to be
served over http(s); `file://` won't load the WebAssembly.

### Pages setup (one time)

Settings → Pages → Build and deployment → Source: **GitHub Actions**. After that every push to
`main` redeploys the site.

## Licence

GPL-2.0 (`LICENSE`). Doom source © id Software; doomgeneric by ozkl; OPL music and parts of the
engine from Chocolate Doom (Simon Howard and contributors); Nuked OPL3 by Alexey Khokholov.
SDL2/SDL2_mixer (zlib) and the Emscripten runtime (MIT) are compiled in. DOOM is a trademark of
id Software; this project is not affiliated with id Software and ships no game data.
