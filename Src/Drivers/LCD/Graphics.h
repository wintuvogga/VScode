#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

int16_t WIDTH, HEIGHT;
int16_t _width, _height, cursor_x, cursor_y;   
uint16_t textcolor, textbgcolor; 
uint8_t textsize, rotation;   
bool wrap, _cp437;

void GraphicsInit(int16_t w, int16_t h);
void GraphicsWriteLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void GraphicsWritePixel(int16_t x, int16_t y, uint16_t color);
void GraphicsWriteFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void GraphicsWriteFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void GraphicsWriteFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void drawPixel(int16_t x, int16_t y, uint16_t color);
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void fillScreen(uint16_t color);
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornernam, int16_t delta, uint16_t color);
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
void write(uint8_t);
void setCursor(int16_t x, int16_t y);
int16_t getCursorX(void);
int16_t getCursorY(void);
void setTextSize(uint8_t s);
void setTextColor(uint16_t c);
void setTextWrap(bool w);
uint8_t getRotation(void);
void setRotation(uint8_t r);
void invertDisplay(bool i);
void cp437(bool x);

int16_t height(void);
int16_t width(void);


// get current cursor position (get rotation safe maximum values, using: width() for x, height() for y)


void charBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
  

#endif