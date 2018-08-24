#include "Graphics.h"
#include "glcdfont.h"
#include <math.h>
#include <SSD1306.h>
#include <stdlib.h>

#ifndef pgm_read_byte
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
 #define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
 #define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
 #define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
 #define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

void GraphicsInit(int16_t w, int16_t h)
{
    WIDTH = w;
    HEIGHT = h;
    _width    = WIDTH;
    _height   = HEIGHT;
    rotation  = 0;
    cursor_y  = cursor_x    = 0;
    textsize  = 1;
    textcolor = textbgcolor = 0xFFFF;
    wrap      = true;
    _cp437    = false;
    //gfxFont   = NULL;
}

/**************************************************************************/
/*!
   @brief    Write a line.  Bresenham's algorithm - thx wikpedia
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void GraphicsWriteLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) 
{
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) 
    {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) 
    {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) 
    {
        ystep = 1;
    } 
    else 
    {
        ystep = -1;
    }

    for (; x0<=x1; x0++) 
    {
        if (steep) 
        {
            drawPixel(y0, x0, color);
        } 
        else 
        {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) 
        {
            y0 += ystep;
            err += dx;
        }
    }
}

/**************************************************************************/
/*!
   @brief    Write a pixel, overwrite in subclasses if startWrite is defined!
    @param   x   x coordinate
    @param   y   y coordinate
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void GraphicsWritePixel(int16_t x, int16_t y, uint16_t color)
{
    drawPixel(x, y, color);
}

/**************************************************************************/
/*!
   @brief    Write a perfectly vertical line, overwrite in subclasses if startWrite is defined!
    @param    x   Top-most x coordinate
    @param    y   Top-most y coordinate
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void GraphicsWriteFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) 
{
    drawFastVLine(x, y, h, color);
}

/**************************************************************************/
/*!
   @brief    Write a perfectly horizontal line, overwrite in subclasses if startWrite is defined!
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void GraphicsWriteFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) 
{
    // Overwrite in subclasses if startWrite is defined!
    // Example: writeLine(x, y, x+w-1, y, color);
    // or writeFillRect(x, y, w, 1, color);
    drawFastHLine(x, y, w, color);
}

/**************************************************************************/
/*!
   @brief    Write a rectangle completely with one color, overwrite in subclasses if startWrite is defined!
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void GraphicsWriteFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    // Overwrite in subclasses if desired!
    fillRect(x,y,w,h,color);
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly vertical line (this is often optimized in a subclass!)
    @param    x   Top-most x coordinate
    @param    y   Top-most y coordinate
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
    GraphicsWriteLine(x, y, x, y+h-1, color);
}

/**************************************************************************/
/*!
   @brief    Draw a perfectly horizontal line (this is often optimized in a subclass!)
    @param    x   Left-most x coordinate
    @param    y   Left-most y coordinate
    @param    w   Width in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) 
{
    GraphicsWriteLine(x, y, x+w-1, y, color);
}

/**************************************************************************/
/*!
   @brief    Fill a rectangle completely with one color. Update in subclasses if desired!
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
   @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) 
{
    for (int16_t i=x; i<x+w; i++) 
    {
        drawFastVLine(i, y, h, color);
    }
}

/**************************************************************************/
/*!
   @brief    Fill the screen completely with one color. Update in subclasses if desired!
    @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillScreen(uint16_t color) 
{
    fillRect(0, 0, _width, _height, color);
}

/**************************************************************************/
/*!
   @brief    Draw a line
    @param    x0  Start point x coordinate
    @param    y0  Start point y coordinate
    @param    x1  End point x coordinate
    @param    y1  End point y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) 
{
    if(x0 == x1)
    {
        if(y0 > y1) _swap_int16_t(y0, y1);
        drawFastVLine(x0, y0, y1 - y0 + 1, color);
    } 
    else if(y0 == y1)
    {
        if(x0 > x1) _swap_int16_t(x0, x1);
        drawFastHLine(x0, y0, x1 - x0 + 1, color);
    } 
    else
    {
        GraphicsWriteLine(x0, y0, x1, y1, color);
    }
}

/**************************************************************************/
/*!
   @brief    Draw a circle outline
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) 
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0  , y0+r, color);
    drawPixel(x0  , y0-r, color);
    drawPixel(x0+r, y0  , color);
    drawPixel(x0-r, y0  , color);

    while (x<y) 
    {
        if (f >= 0) 
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);
        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}

/**************************************************************************/
/*!
   @brief    Quarter-circle drawer, used to do circles and roundrects
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    cornername  Mask bit #1 or bit #2 to indicate which quarters of the circle we're doing
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) 
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y) 
    {
        if (f >= 0) 
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;
        if (cornername & 0x4) 
        {
            drawPixel(x0 + x, y0 + y, color);
            drawPixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) 
        {
            drawPixel(x0 + x, y0 - y, color);
            drawPixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) 
        {
            drawPixel(x0 - y, y0 + x, color);
            drawPixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) 
        {
            drawPixel(x0 - y, y0 - x, color);
            drawPixel(x0 - x, y0 - y, color);
        }
    }
}

/**************************************************************************/
/*!
   @brief    Draw a circle with filled color
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) 
{
    drawFastVLine(x0, y0-r, 2*r+1, color);
    fillCircleHelper(x0, y0, r, 3, 0, color);
}


/**************************************************************************/
/*!
   @brief    Quarter-circle drawer with fill, used to do circles and roundrects
    @param    x0   Center-point x coordinate
    @param    y0   Center-point y coordinate
    @param    r   Radius of circle
    @param    cornername  Mask bit #1 or bit #2 to indicate which quarters of the circle we're doing
    @param    delta  Offset from center-point, used for round-rects
    @param    color 16-bit 5-6-5 Color to fill with
*/
/**************************************************************************/
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color) {

    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    while (x<y)
     {
        if (f >= 0) 
        {
            y--;
            ddF_y += 2;
            f     += ddF_y;
        }
        x++;
        ddF_x += 2;
        f     += ddF_x;

        if (cornername & 0x1) 
        {
            drawFastVLine(x0+x, y0-y, 2*y+1+delta, color);
            drawFastVLine(x0+y, y0-x, 2*x+1+delta, color);
        }
        if (cornername & 0x2) 
        {
            drawFastVLine(x0-x, y0-y, 2*y+1+delta, color);
            drawFastVLine(x0-y, y0-x, 2*x+1+delta, color);
        }
    }
}

/**************************************************************************/
/*!
   @brief   Draw a rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    drawFastHLine(x, y, w, color);
    drawFastHLine(x, y+h-1, w, color);
    drawFastVLine(x, y, h, color);
    drawFastVLine(x+w-1, y, h, color);
}

/**************************************************************************/
/*!
   @brief   Draw a rounded rectangle with no fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) 
{
    // smarter version
    drawFastHLine(x+r  , y    , w-2*r, color); // Top
    drawFastHLine(x+r  , y+h-1, w-2*r, color); // Bottom
    drawFastVLine(x    , y+r  , h-2*r, color); // Left
    drawFastVLine(x+w-1, y+r  , h-2*r, color); // Right
    // draw four corners
    drawCircleHelper(x+r    , y+r    , r, 1, color);
    drawCircleHelper(x+w-r-1, y+r    , r, 2, color);
    drawCircleHelper(x+w-r-1, y+h-r-1, r, 4, color);
    drawCircleHelper(x+r    , y+h-r-1, r, 8, color);
}

/**************************************************************************/
/*!
   @brief   Draw a rounded rectangle with fill color
    @param    x   Top left corner x coordinate
    @param    y   Top left corner y coordinate
    @param    w   Width in pixels
    @param    h   Height in pixels
    @param    r   Radius of corner rounding
    @param    color 16-bit 5-6-5 Color to draw/fill with
*/
/**************************************************************************/
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) 
{
    // smarter version
    fillRect(x+r, y, w-2*r, h, color);

    // draw four corners
    fillCircleHelper(x+w-r-1, y+r, r, 1, h-2*r-1, color);
    fillCircleHelper(x+r    , y+r, r, 2, h-2*r-1, color);
}

/**************************************************************************/
/*!
   @brief   Draw a triangle with no fill color
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to draw with
*/
/**************************************************************************/
void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) 
{
    drawLine(x0, y0, x1, y1, color);
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x0, y0, color);
}

/**************************************************************************/
/*!
   @brief     Draw a triangle with color-fill
    @param    x0  Vertex #0 x coordinate
    @param    y0  Vertex #0 y coordinate
    @param    x1  Vertex #1 x coordinate
    @param    y1  Vertex #1 y coordinate
    @param    x2  Vertex #2 x coordinate
    @param    y2  Vertex #2 y coordinate
    @param    color 16-bit 5-6-5 Color to fill/draw with
*/
/**************************************************************************/
void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) 
{
    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) 
    {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }
    if (y1 > y2) 
    {
        _swap_int16_t(y2, y1); _swap_int16_t(x2, x1);
    }
    if (y0 > y1) 
    {
        _swap_int16_t(y0, y1); _swap_int16_t(x0, x1);
    }

    if(y0 == y2) 
    { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if(x1 < a)      a = x1;
        else if(x1 > b) b = x1;
        if(x2 < a)      a = x2;
        else if(x2 > b) b = x2;
        drawFastHLine(a, y0, b-a+1, color);
        return;
    }

    int16_t
    dx01 = x1 - x0,
    dy01 = y1 - y0,
    dx02 = x2 - x0,
    dy02 = y2 - y0,
    dx12 = x2 - x1,
    dy12 = y2 - y1;
    int32_t
    sa   = 0,
    sb   = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if(y1 == y2) last = y1;   // Include y1 scanline
    else         last = y1-1; // Skip it

    for(y=y0; y<=last; y++) 
    {
        a   = x0 + sa / dy01;
        b   = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        drawFastHLine(a, y, b-a+1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for(; y<=y2; y++) 
    {
        a   = x1 + sa / dy12;
        b   = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if(a > b) _swap_int16_t(a,b);
        drawFastHLine(a, y, b-a+1, color);
    }
}

// TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------

// Draw a character
/**************************************************************************/
/*!
   @brief   Draw a single character
    @param    x   Bottom left corner x coordinate
    @param    y   Bottom left corner y coordinate
    @param    c   The 8-bit font-indexed character (likely ascii)
    @param    color 16-bit 5-6-5 Color to draw chraracter with
    @param    bg 16-bit 5-6-5 Color to fill background with (if same as color, no background)
    @param    size  Font magnification level, 1 is 'original' size
*/
/**************************************************************************/
void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) 
{
    if((x >= _width)            || // Clip right
           (y >= _height)           || // Clip bottom
           ((x + 6 * size - 1) < 0) || // Clip left
           ((y + 8 * size - 1) < 0))   // Clip top
            return;

        if(!_cp437 && (c >= 176)) 
            c++; // Handle 'classic' charset behavior

        for(int8_t i=0; i<5; i++ ) 
        { // Char bitmap = 5 columns
            uint8_t line = pgm_read_byte(&font[c * 5 + i]);
            for(int8_t j=0; j<8; j++, line >>= 1) 
            {
                if(line & 1) 
                {
                    if(size == 1)
                        drawPixel(x+i, y+j, color);
                    else
                        fillRect(x+i*size, y+j*size, size, size, color);
                } 
                else if(bg != color) 
                {
                    if(size == 1)
                        drawPixel(x+i, y+j, bg);
                    else
                        fillRect(x+i*size, y+j*size, size, size, bg);
                }
            }
        }
        if(bg != color)
        { // If opaque, draw vertical line for last column
            if(size == 1) 
                drawFastVLine(x+5, y, 8, bg);
            else
                fillRect(x+5*size, y, size, 8*size, bg);
        }
}
/**************************************************************************/
/*!
    @brief  Print one byte/character of data, used to support print()
    @param  c  The 8-bit ascii character to write
*/
/**************************************************************************/
void write(uint8_t c) 
{
        if(c == '\n') {                        // Newline?
            cursor_x  = 0;                     // Reset x to zero,
            cursor_y += textsize * 8;          // advance y one line
        } else if(c != '\r') {                 // Ignore carriage returns
            if(wrap && ((cursor_x + textsize * 6) > _width)) { // Off right?
                cursor_x  = 0;                 // Reset x to zero,
                cursor_y += textsize * 8;      // advance y one line
            }
            drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
            cursor_x += textsize * 6;          // Advance x one char
        }
}

/**************************************************************************/
/*!
    @brief  Set text cursor location
    @param  x    X coordinate in pixels
    @param  y    Y coordinate in pixels
*/
/**************************************************************************/
void setCursor(int16_t x, int16_t y) 
{
    cursor_x = x;
    cursor_y = y;
}

/**************************************************************************/
/*!
    @brief  Get text cursor X location
    @returns    X coordinate in pixels
*/
/**************************************************************************/
int16_t getCursorX(void) 
{
    return cursor_x;
}

/**************************************************************************/
/*!
    @brief      Get text cursor Y location
    @returns    Y coordinate in pixels
*/
/**************************************************************************/
int16_t getCursorY(void) 
{
    return cursor_y;
}

/**************************************************************************/
/*!
    @brief   Set text 'magnification' size. Each increase in s makes 1 pixel that much bigger.
    @param  s  Desired text size. 1 is default 6x8, 2 is 12x16, 3 is 18x24, etc
*/
/**************************************************************************/
void setTextSize(uint8_t s)
{
    textsize = (s > 0) ? s : 1;
}

/**************************************************************************/
/*!
    @brief   Set text font color with transparant background
    @param   c   16-bit 5-6-5 Color to draw text with
*/
/**************************************************************************/
void setTextColor(uint16_t c) {
    // For 'transparent' background, we'll set the bg
    // to the same as fg instead of using a flag
    textcolor = textbgcolor = c;
}

/**************************************************************************/
/*!
    @brief      Whether text that is too long should 'wrap' around to the next line.
    @param  w Set true for wrapping, false for clipping
*/
/**************************************************************************/
void setTextWrap(bool w) 
{
    wrap = w;
}

/**************************************************************************/
/*!
    @brief      Get rotation setting for display
    @returns    0 thru 3 corresponding to 4 cardinal rotations
*/
/**************************************************************************/
uint8_t getRotation(void) 
{
    return rotation;
}

/**************************************************************************/
/*!
    @brief      Set rotation setting for display
    @param  x   0 thru 3 corresponding to 4 cardinal rotations
*/
/**************************************************************************/
void setRotation(uint8_t x) 
{
    rotation = (x & 3);
    switch(rotation) 
    {
        case 0:
        case 2:
            _width  = WIDTH;
            _height = HEIGHT;
            break;
        case 1:
        case 3:
            _width  = HEIGHT;
            _height = WIDTH;
            break;
    }
}

/**************************************************************************/
/*!
    @brief Enable (or disable) Code Page 437-compatible charset.
    There was an error in glcdfont.c for the longest time -- one character
    (#176, the 'light shade' block) was missing -- this threw off the index
    of every character that followed it.  But a TON of code has been written
    with the erroneous character indices.  By default, the library uses the
    original 'wrong' behavior and old sketches will still work.  Pass 'true'
    to this function to use correct CP437 character values in your code.
    @param  x  Whether to enable (True) or not (False)
*/
/**************************************************************************/
void cp437(bool x) 
{
    _cp437 = x;
}

/**************************************************************************/
/*!
    @brief      Get width of the display, accounting for the current rotation
    @returns    Width in pixels
*/
/**************************************************************************/
int16_t width(void)
{
    return _width;
}

/**************************************************************************/
/*!
    @brief      Get height of the display, accounting for the current rotation
    @returns    Height in pixels
*/
/**************************************************************************/
int16_t height(void)
{
    return _height;
}

/**************************************************************************/
/*!
    @brief      Invert the display (ideally using built-in hardware command)
    @param   i  True if you want to invert, false to make 'normal'
*/
/**************************************************************************/
void invertDisplay(bool i)
{
    // Do nothing, must be subclassed if supported by hardware
}

void drawPixel(int16_t x, int16_t y, uint16_t color) 
{
    int16_t t;
    if(buffer)
     {
        if((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) 
            return;

        switch(rotation) 
        {
            case 1:
                t = x;
                x = WIDTH  - 1 - y;
                y = t;
                break;
            case 2:
                x = WIDTH  - 1 - x;
                y = HEIGHT - 1 - y;
                break;
            case 3:
                t = x;
                x = y;
                y = HEIGHT - 1 - t;
                break;
        }

        buffer[x + y * WIDTH] = color;
    }
}