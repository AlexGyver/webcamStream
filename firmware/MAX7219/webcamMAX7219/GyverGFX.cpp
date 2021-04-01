#include "GyverGFX.h"

void GyverGFX::dot(int x, int y, uint8_t fill) {}

void GyverGFX::fastLineH(int y, int x0, int x1, uint8_t fill) {
  GFX_SWAP(x0, x1);
  for (int x = x0; x <= x1; x++) dot(x, y, fill);
}

void GyverGFX::fastLineV(int x, int y0, int y1, uint8_t fill) {
  GFX_SWAP(y0, y1);
  for (int y = y0; y <= y1; y++) dot(x, y, fill);
}

void GyverGFX::line(int x0, int y0, int x1, int y1, uint8_t fill) {
  if (x0 == x1) fastLineV(x0, y0, y1, fill);
  else if (y0 == y1) fastLineH(y0, x0, x1, fill);
  else {
    GFX_SWAP(x0, x1);
    GFX_SWAP(y0, y1);
    int sx, sy, e2, err;
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    if (x0 < x1) sx = 1; else sx = -1;
    if (y0 < y1) sy = 1; else sy = -1;
    err = dx - dy;
    for (;;) {
      dot(x0, y0, fill);
      if (x0 == x1 && y0 == y1) return;
      e2 = err << 1;
      if (e2 > -dy) {
        err = err - dy;
        x0 = x0 + sx;
      }
      if (e2 < dx) {
        err = err + dx;
        y0 = y0 + sy;
      }
    }
  }
}

void GyverGFX::rect(int x0, int y0, int x1, int y1, uint8_t fill) {
  if (fill == GFX_STROKE) {
    fastLineH(y0, x0 + 1, x1 - 1);
    fastLineH(y1, x0 + 1, x1 - 1);
    fastLineV(x0, y0, y1);
    fastLineV(x1, y0, y1);
  } else {
    GFX_SWAP(y0, y1);
    for (int y = y0; y <= y1; y++) fastLineH(y, x0, x1, fill);
  }
}

void GyverGFX::roundRect(int x0, int y0, int x1, int y1, uint8_t fill) {
  if (fill == GFX_STROKE) {
    fastLineV(x0, y0 + 2, y1 - 2);
    fastLineV(x1, y0 + 2, y1 - 2);
    fastLineH(y0, x0 + 2, x1 - 2);
    fastLineH(y1, x0 + 2, x1 - 2);
    dot(x0 + 1, y0 + 1);
    dot(x1 - 1, y0 + 1);
    dot(x1 - 1, y1 - 1);
    dot(x0 + 1, y1 - 1);
  } else {
    fastLineV(x0, y0 + 2, y1 - 2, fill);
    fastLineV(x0 + 1, y0 + 1, y1 - 1, fill);
    fastLineV(x1 - 1, y0 + 1, y1 - 1, fill);
    fastLineV(x1, y0 + 2, y1 - 2, fill);
    rect(x0 + 2, y0, x1 - 2, y1, fill);
  }
}

void GyverGFX::circle(int x, int y, int radius, uint8_t fill) {
  int f = 1 - radius;
  int ddF_x = 1;
  int ddF_y = -2 * radius;
  int x1 = 0;
  int y1 = radius;
  uint8_t fillLine = (fill == GFX_CLEAR) ? 0 : 1;
  dot(x, y + radius, fillLine);
  dot(x, y - radius, fillLine);
  dot(x + radius, y, fillLine);
  dot(x - radius, y, fillLine);
  if (fill != GFX_STROKE) fastLineV(x, y - radius, y + radius - 1, fillLine);
  while (x1 < y1) {
    if (f >= 0) {
      y1--;
      ddF_y += 2;
      f += ddF_y;
    }
    x1++;
    ddF_x += 2;
    f += ddF_x;
    if (fill == GFX_STROKE) {
      dot(x + x1, y + y1);
      dot(x - x1, y + y1);
      dot(x + x1, y - y1);
      dot(x - x1, y - y1);
      dot(x + y1, y + x1);
      dot(x - y1, y + x1);
      dot(x + y1, y - x1);
      dot(x - y1, y - x1);
    } else {
      fastLineV(x + x1, y - y1, y + y1, fillLine);
      fastLineV(x - x1, y - y1, y + y1, fillLine);
      fastLineV(x + y1, y - x1, y + x1, fillLine);
      fastLineV(x - y1, y - x1, y + x1, fillLine);
    }
  }
}

void GyverGFX::bezier(uint8_t* arr, uint8_t size, uint8_t dense, uint8_t fill = 1) {
  int a[size * 2];
  for (int i = 0; i < (1 << dense); i++) {
    for (int j = 0; j < size * 2; j++) a[j] = arr[j] << 3;
    for (int j = (size - 1) * 2 - 1; j > 0; j -= 2)
      for (int k = 0; k <= j; k++)
        a[k] = a[k] + (((a[k + 2] - a[k]) * i) >> dense);
    dot(a[0] >> 3, a[1] >> 3, fill);
  }
}

void GyverGFX::bezier16(int* arr, uint8_t size, uint8_t dense, uint8_t fill = 1) {
  int a[size * 2];
  for (int i = 0; i < (1 << dense); i++) {
    for (int j = 0; j < size * 2; j++) a[j] = arr[j];
    for (int j = (size - 1) * 2 - 1; j > 0; j -= 2)
      for (int k = 0; k <= j; k++)
        a[k] = a[k] + (((a[k + 2] - a[k]) * i) >> dense);
    dot(a[0], a[1], fill);
  }
}
