#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./stb_image_write.h"

void sobel_filter(uint8_t* img, int w, int h) {
  int kernel_x[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1}
  };

  int kernel_y[3][3] = {
    {-1, -2, -1},
    {0, 0, 0},
    {1, 2, 1}
  };

  uint8_t* out = (uint8_t*) malloc(w * h * sizeof(uint8_t));

  for (int y = 1; y < h - 1; y++) {
    for (int x = 1; x < w - 1; x++) {
      int gx = 0;
      int gy = 0;

      for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
          int index = (y + j) * w + (x + i);
          gx += img[index] * kernel_x[j+1][i+1];
          gy += img[index] * kernel_y[j+1][i+1];
        }
      }

      int index = y * w + x;
      out[index] = (uint8_t) (abs(gx) + abs(gy));
    }
  }

  // Copy the output back to the input image
  memcpy(img, out, w * h * sizeof(uint8_t));
  free(out);
}

void sobel(uint8_t* inputImg) {
  

  int w, h, c;
  uint8_t* img = stbi_load(inputImg, &w, &h, &c, 1);
  if (!img) {
    printf("Error loading image %s\n", inputImg);
    return 1;
  }

  sobel_filter(img, w, h);

  stbi_write_png(inputImg, w, h, 1, img, w);

  stbi_image_free(img);

}


// int main(int argc, char** argv) {
//   sobel("imagenrecibida0.jpg");

//   return 0;
// }