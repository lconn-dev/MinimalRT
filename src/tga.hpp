// 24-Bit Non-ColorMapped, Truevision TGA (TARGA) Writer

#pragma once

#include <memory>
#include <fstream>

#include "color.hpp"
#include "framebuffer.hpp"

namespace tga {

 
  class tgaImg {
  private:
    color::rgb* pixelData = nullptr;

    framebuffer::frameBuf32* frame;
  public:

    // Initialize array and set all pixels to black
    tgaImg(framebuffer::frameBuf32* fr) : frame(fr){}

    // write the tga image to a file
    // return true on success
    bool save(std::string fname) {
      fname += ".tga";
      std::ofstream fout(fname, std::ios::binary);
      bool isWritten = false;
      auto xres = frame->getXres();
      auto yres = frame->getYres();
      if (fout.good()) {
        
        // write header to file
        // for more info see http://www.paulbourke.net/dataformats/tga/
        fout.put(0); // id field sz
        fout.put(0); // no color map
        fout.put(2); // image type: 2 uncompressed truecolor image
        
        // color map info ( 5 bytes )
        fout.put(0);
        fout.put(0);
        fout.put(0);
        fout.put(0);
        
        fout.put(0);
        
        // image spec ( 10 bytes )
        fout.put(0); // xorgin
        fout.put(0);
        fout.put(0); // yorigin
        fout.put(0);
        
        fout.put((xres & 0x00FF)); // width (int split into 2 bytes)
        fout.put((xres & 0xFF00) / 256);

        fout.put((yres & 0x00FF)); // height
        fout.put((yres & 0xFF00) / 256);

        fout.put(24); // pixel size in bits, 24bit truecolor, no alpha channel
        fout.put(0); // Image Descriptor Byte.

        // write pixels in BGR color format, 24 bit
        // bottom to top, left to right
        for (uint32_t y = yres; y > 0; y--) {
          for (uint32_t x = 0; x < xres; x++) {
            color::rgb pix;
            frame->read(x, y, pix);
            fout.put((char)(pix.b * 255));
            fout.put((char)(pix.g * 255));
            fout.put((char)(pix.r * 255));
          }
        }

        fout.close();
        isWritten = true;
      }
      return isWritten;
    }

  };

}

