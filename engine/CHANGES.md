# Engine sources and changes

Based on [doomgeneric](https://github.com/ozkl/doomgeneric) at commit `e257a09`, plus the
OPL music files from [Chocolate Doom](https://github.com/chocolate-doom/chocolate-doom) 3.0.1
(`opl*.c/h`, `opl3.c/h` (Nuked OPL3), `i_oplmusic.c`, `midifile.c/h`). All GPL-2.0-or-later.

## Added

- `doomgeneric_web.c` — browser backend: draws to a 2D canvas (no WebGL), keyboard and mouse
  events are pushed from JavaScript (`dg_push_key`, `dg_push_mouse`).
- `Makefile.web` — Emscripten build.
- OPL music: `i_sound.c` uses the OPL music module; `opl.c` skips chip detection under
  Emscripten (it would block waiting for the audio thread).

## Fixes

- **WebAssembly crash in `I_Error`**: `G_CheckDemoStatus` (returns `boolean`) was registered as a
  `void (*)(void)` exit function; calling it through that pointer traps in WebAssembly
  ("indirect call signature mismatch"). Now wrapped (`d_main.c`).
- Fatal errors and Quit are reported to the page (`Module.onDoomError` / `onDoomQuit`) and stop
  the main loop (`i_system.c`); ENDOOM no longer calls `exit()` under Emscripten.
- `idclev` ignores maps the WAD doesn't contain instead of aborting (`st_stuff.c`).
- **Old IWADs (Doom v1.1 and earlier)**:
  - status bar drawn from the split `STMBARL`/`STMBARR` lumps when `STBAR` is missing;
  - missing menu, status bar and intermission graphics are skipped (`m_menu.c`, `st_lib.c`,
    `st_stuff.c`, `wi_stuff.c`, `v_video.c` treats NULL patches as no-ops);
  - missing music and sound lumps play silence (`s_sound.c`, `i_sdlsound.c`);
  - pre-v1.4 demos (no version byte) are parsed; unknown demo formats are skipped instead of
    leaving the game with no players, which hung `P_LookForPlayers` (`g_game.c`).
- `dg_set_paused(on)` (Android app): pauses an active level while the app is in the background
  and only un-pauses what it paused itself (`doomgeneric_web.c`).
- Vertical free-look (`dg_set_look`): `R_SetupFrame` shifts `centery` by `dg_lookdir` and rebuilds
  `yslope` (as Heretic does); the weapon sprite is compensated so it stays in place
  (`r_main.c`, `r_things.c`, `doomgeneric_web.c`). Aiming is unchanged (vanilla autoaim).
- Cheat codes only consume printable characters (`st_stuff.c`), so movement keys sent together with
  a letter (WASD on the keyboard) don't reset a half-typed cheat.
