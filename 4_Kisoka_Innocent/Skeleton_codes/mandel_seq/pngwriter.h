#ifndef PNGWRITER_H
#define PNGWRITER_H

#include <png.h>

typedef struct {
    int nWidth;
    int nHeight;
    png_bytepp pPixels;
} png_data;

png_data* png_create(int nWidth, int nHeight);
void png_plot(png_data *pData, int x, int y, int r, int g, int b);
void png_write(png_data* pPng, const char* filename);

#endif // PNGWRITER_H
