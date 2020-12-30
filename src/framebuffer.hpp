#pragma once

namespace framebuffer {

  class frameBuf32 {
  private:
    color::rgb* pixelData = nullptr;
    uint16_t xres, yres;
  public:
    
    frameBuf32(uint16_t xr, uint16_t yr) : xres(xr), yres(yr) {
      uint32_t pixAmt = xres * yres;
      pixelData = new color::rgb[pixAmt];
    }

    ~frameBuf32() {
      // Deallocate on destruction
      if (pixelData != nullptr) delete[] pixelData;
    }

    // write a pixel at a xy position into the image buffer
    void write(uint16_t x, uint16_t y, color::rgb& color) {
      pixelData[x * yres + y] = color;
    }

    // write a pixel at a xy position into the image buffer
    void read(uint16_t x, uint16_t y, color::rgb& color) {
      color = pixelData[x * yres + y];
    }

    uint16_t getXres() {
      return xres;
    }

    uint16_t getYres() {
      return yres;
    }
  
  };

}