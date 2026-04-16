#include "epd_driver.h"
#include "config.h"
#include "DEV_Config.h"
#include <stdlib.h>
#include <string.h>

#if defined(USE_WAVESHARE_4IN2_V2)
#include "EPD_4in2_V2.h"
#define WS_WIDTH EPD_4IN2_V2_WIDTH
#define WS_HEIGHT EPD_4IN2_V2_HEIGHT
static inline void ws_init_full_panel() { EPD_4IN2_V2_Init(); }
static inline void ws_clear_panel() { EPD_4IN2_V2_Clear(); }
static inline void ws_display_panel(UBYTE* img) { EPD_4IN2_V2_Display(img); }
static inline void ws_partial_panel(UBYTE* img, UWORD xs, UWORD ys, UWORD xe, UWORD ye) { EPD_4IN2_V2_PartialDisplay(img, xs, ys, xe, ye); }
static inline void ws_sleep_panel() { EPD_4IN2_V2_Sleep(); }
#else
#include "EPD_4in2.h"
#define WS_WIDTH EPD_4IN2_WIDTH
#define WS_HEIGHT EPD_4IN2_HEIGHT
static inline void ws_init_full_panel() { EPD_4IN2_Init_Fast(); }
static inline void ws_clear_panel() { EPD_4IN2_Clear(); }
static inline void ws_display_panel(UBYTE* img) { EPD_4IN2_Display(img); }
static inline void ws_partial_panel(UBYTE* img, UWORD xs, UWORD ys, UWORD xe, UWORD ye) { EPD_4IN2_PartialDisplay(xs, ys, xe, ye, img); }
static inline void ws_sleep_panel() { EPD_4IN2_Sleep(); }
#endif

enum class EpdMode {
  None,
  Full,
  Partial,
};

static bool g_epd_initialized = false;
static EpdMode g_epd_mode = EpdMode::None;
static UBYTE* g_epd_image = nullptr;
static UWORD g_epd_image_size = 0;

static constexpr UBYTE EPD_BYTE_WHITE = 0xFF;
static constexpr UBYTE EPD_BYTE_BLACK = 0x00;

static void ensureBuffer() {
  if (g_epd_image) return;
  g_epd_image_size = ((WS_WIDTH % 8 == 0) ? (WS_WIDTH / 8) : (WS_WIDTH / 8 + 1)) * WS_HEIGHT;
  g_epd_image = (UBYTE*)malloc(g_epd_image_size);
  if (!g_epd_image) {
    Serial.println("[EPD] malloc failed");
    while (1) delay(1000);
  }
  memset(g_epd_image, EPD_BYTE_WHITE, g_epd_image_size);
}

static void resetPanelState() {
  g_epd_initialized = false;
  g_epd_mode = EpdMode::None;
}

static void initPanel(EpdMode mode, bool clearPanel) {
  ensureBuffer();
  if (g_epd_initialized && g_epd_mode == mode) return;

  DEV_Module_Init();
  switch (mode) {
    case EpdMode::Full:
    case EpdMode::Partial:
      ws_init_full_panel();
      break;
    case EpdMode::None:
      return;
  }

  if (clearPanel) {
    ws_clear_panel();
    memset(g_epd_image, EPD_BYTE_WHITE, g_epd_image_size);
  }

  g_epd_initialized = true;
  g_epd_mode = mode;
}

static void blitFullImage(const uint8_t* image) {
  ensureBuffer();
  memset(g_epd_image, EPD_BYTE_WHITE, g_epd_image_size);
  if (!image) return;

  const int dstRowBytes = WS_WIDTH / 8;
  const int srcRowBytes = ROW_BYTES;
  const int rows = (WS_HEIGHT < H) ? WS_HEIGHT : H;
  const int cols = (dstRowBytes < srcRowBytes) ? dstRowBytes : srcRowBytes;
  for (int y = 0; y < rows; ++y) {
    memcpy(g_epd_image + y * dstRowBytes, image + y * srcRowBytes, cols);
  }
}

static void blitRegionToFullBuffer(const uint8_t* region, int xStart, int yStart, int xEnd, int yEnd) {
  ensureBuffer();
  if (!region) return;

  const int xs = xStart < 0 ? 0 : xStart;
  const int ys = yStart < 0 ? 0 : yStart;
  const int xe = xEnd > WS_WIDTH ? WS_WIDTH : xEnd;
  const int ye = yEnd > WS_HEIGHT ? WS_HEIGHT : yEnd;
  if (xe <= xs || ye <= ys) return;

  const int alignedXs = xs & ~7;
  const int alignedXe = (xe + 7) & ~7;
  const int regionRowBytes = (alignedXe - alignedXs) / 8;
  const int dstRowBytes = WS_WIDTH / 8;

  for (int y = ys; y < ye; ++y) {
    memcpy(g_epd_image + y * dstRowBytes + alignedXs / 8,
           region + (y - ys) * regionRowBytes,
           regionRowBytes);
  }
}

void gpioInit() {
  DEV_Module_Init();
}

void epdInit() {
  initPanel(EpdMode::Full, true);
}

void epdInitFast() {
  epdInit();
}

void epdDisplay(const uint8_t *image) {
  resetPanelState();
  initPanel(EpdMode::Full, false);

  // Clear: 1 white pass (the EPD_4IN2_Display layer also does a 0x00 pre-clear)
  memset(g_epd_image, EPD_BYTE_WHITE, g_epd_image_size);
  ws_display_panel(g_epd_image);
  DEV_Delay_ms(200);

  // New image
  blitFullImage(image);
  ws_display_panel(g_epd_image);
  DEV_Delay_ms(1500);  // Give display extra time to complete refresh
}

void epdDisplayDual(const uint8_t *blackImage, const uint8_t *colorImage) {
  (void)colorImage;
  epdDisplay(blackImage ? blackImage : colorImage);
}

void epdDisplayDualCombo(const uint8_t *blackImage, const uint8_t *colorImage) {
  (void)colorImage;
  epdDisplay(blackImage ? blackImage : colorImage);
}

void epdDisplayFast(const uint8_t *image) {
  epdDisplay(image);
}

void epdPartialDisplay(uint8_t *data, int xStart, int yStart, int xEnd, int yEnd) {
  if (!data) return;
  initPanel(EpdMode::Partial, false);
  blitRegionToFullBuffer(data, xStart, yStart, xEnd, yEnd);
  ws_partial_panel(g_epd_image, (UWORD)xStart, (UWORD)yStart, (UWORD)xEnd, (UWORD)yEnd);
}

void epdSleep() {
  if (!g_epd_initialized) return;
  ws_sleep_panel();
  resetPanelState();
}

void epd_wft_4in2b_init() { epdInit(); }
void epd_wft_4in2b_clear(uint8_t color) {
  initPanel(EpdMode::Full, false);
  memset(g_epd_image, color == 0 ? EPD_BYTE_BLACK : EPD_BYTE_WHITE, g_epd_image_size);
  ws_clear_panel();
}
void epd_wft_4in2b_display(const uint8_t* image, size_t len) { (void)len; epdDisplay(image); }
void epd_wft_4in2b_sleep() { epdSleep(); }
void epd_wft_4in2b_dual_plane_test(const uint8_t* black, const uint8_t* color) { (void)color; epdDisplay(black ? black : color); }
void epd_wft_4in2b_ram_channel_test(const uint8_t* plane10, const uint8_t* plane13) { (void)plane13; epdDisplay(plane10 ? plane10 : plane13); }
void epd_wft_4in2b_native_test(const uint8_t* data1, const uint8_t* data2) { (void)data2; epdDisplay(data1 ? data1 : data2); }
void epd_wft_4in2b_old_compat_test(const uint8_t* black, const uint8_t* color, bool doClear) {
  if (doClear) {
    epd_wft_4in2b_clear(1);
  }
  epdDisplay(black ? black : color);
}
