#ifndef GyverGFX_h
#define GyverGFX_h
#include <Arduino.h>

#define GFX_CLEAR 0
#define GFX_FILL 1
#define GFX_STROKE 2
#define GFX_SWAP(x,y) if (x > y) x = x ^ y ^ (y = x);

class GyverGFX {
  public:
    virtual void dot(int x, int y, uint8_t fill = 1);
    void fastLineH(int y, int x0, int x1, uint8_t fill = 1);
    void fastLineV(int x, int y0, int y1, uint8_t fill = 1);
    void line(int x0, int y0, int x1, int y1, uint8_t fill = 1);
    void rect(int x0, int y0, int x1, int y1, uint8_t fill = GFX_STROKE);
    void roundRect(int x0, int y0, int x1, int y1, uint8_t fill = GFX_STROKE);
    void circle(int x, int y, int radius, uint8_t fill = 0);
    void bezier(uint8_t* arr, uint8_t size, uint8_t dense, uint8_t fill = 1);
    void bezier16(int* arr, uint8_t size, uint8_t dense, uint8_t fill = 1);
  private:
};
#endif
