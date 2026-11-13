// doomgeneric browser backend: draws to a 2D canvas (no WebGL needed),
// keyboard input is delivered from JavaScript via dg_push_key().

#include "doomkeys.h"
#include "doomgeneric.h"
#include "d_event.h"
#include "doomstat.h"
#include "s_sound.h"

#include <stdint.h>
#include <emscripten.h>

#define KEYQUEUE_SIZE 64

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

EMSCRIPTEN_KEEPALIVE void dg_push_key(int pressed, int doomKey)
{
  unsigned int next = (s_KeyQueueWriteIndex + 1) % KEYQUEUE_SIZE;
  if (next == s_KeyQueueReadIndex) return; // queue full, drop
  s_KeyQueue[s_KeyQueueWriteIndex] = (unsigned short)(((pressed ? 1 : 0) << 8) | (doomKey & 0xFF));
  s_KeyQueueWriteIndex = next;
}

// Mouse: buttons bitfield (1=fire, 2=strafe, 4=forward), dx = horizontal turn.
// Vertical motion is ignored so the mouse never walks the player forward/back.
EMSCRIPTEN_KEEPALIVE void dg_push_mouse(int buttons, int dx)
{
  event_t ev;
  ev.type = ev_mouse;
  ev.data1 = buttons;
  ev.data2 = dx;
  ev.data3 = 0;
  ev.data4 = 0;
  D_PostEvent(&ev);
}

// App lifecycle: pause the game while the app is in the background.
// Only pauses an active level, and only un-pauses what it paused itself.
extern boolean paused;
static int paused_by_app = 0;

EMSCRIPTEN_KEEPALIVE void dg_set_paused(int on)
{
  if (on)
  {
    if (!paused && gamestate == GS_LEVEL && !demoplayback)
    {
      paused = 1;
      paused_by_app = 1;
      S_PauseSound();
    }
  }
  else if (paused_by_app)
  {
    paused = 0;
    paused_by_app = 0;
    S_ResumeSound();
  }
}

// Free-look: vertical view offset in 200-line pixels (+ = look up), clamped.
extern int dg_lookdir;
EMSCRIPTEN_KEEPALIVE void dg_set_look(int look)
{
  if (look > 60) look = 60;
  if (look < -60) look = -60;
  dg_lookdir = look;
}

void DG_Init()
{
  EM_ASM({
    const c = Module.canvas;
    c.width = $0; c.height = $1;
    Module.dgCtx = c.getContext('2d', { alpha: false });
    Module.dgImg = Module.dgCtx.createImageData($0, $1);
  }, DOOMGENERIC_RESX, DOOMGENERIC_RESY);
}

void DG_DrawFrame()
{
  EM_ASM({
    const n = $1 * $2;
    const src = HEAPU8.subarray($0, $0 + n * 4);  // bytes per pixel: B, G, R, X
    const dst = Module.dgImg.data;
    for (let i = 0; i < n * 4; i += 4) {
      dst[i] = src[i + 2]; dst[i + 1] = src[i + 1]; dst[i + 2] = src[i]; dst[i + 3] = 255;
    }
    Module.dgCtx.putImageData(Module.dgImg, 0, 0);
  }, DG_ScreenBuffer, DOOMGENERIC_RESX, DOOMGENERIC_RESY);
}

void DG_SleepMs(uint32_t ms)
{
  (void)ms; // the browser main loop paces frames
}

uint32_t DG_GetTicksMs()
{
  return (uint32_t)emscripten_get_now();
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
  if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex) return 0;
  unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
  s_KeyQueueReadIndex = (s_KeyQueueReadIndex + 1) % KEYQUEUE_SIZE;
  *pressed = keyData >> 8;
  *doomKey = keyData & 0xFF;
  return 1;
}

void DG_SetWindowTitle(const char * title)
{
  (void)title;
}

int main(int argc, char **argv)
{
  doomgeneric_Create(argc, argv);
  emscripten_set_main_loop(doomgeneric_Tick, 0, 1);
  return 0;
}
