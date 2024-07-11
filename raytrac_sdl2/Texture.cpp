// Texture.cpp

#include "Texture.h"
#include <iostream>

Texture::Texture(const std::string& filename) {
    SDL_Surface* surface = IMG_Load(filename.c_str());
    if (!surface) {
        std::cerr << "Error loading image: " << filename << ", SDL Error: " << IMG_GetError() << std::endl;
        return;
    }

    width = surface->w;
    height = surface->h;
    pixels.resize(width * height);

    SDL_LockSurface(surface);
    Uint8* pixelData = static_cast<Uint8*>(surface->pixels);
    SDL_PixelFormat* format = surface->format;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Uint8 r, g, b, a;
            Uint32 pixel;

            switch (format->BitsPerPixel) {
            case 8: // 8-bit paletteli format
            {
                Uint8* p = pixelData + (y * surface->pitch) + x;
                pixel = *p;
                SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
            }
            break;
            case 15: // 15-bit format (5-5-5)
            case 16: // 16-bit format (5-6-5 veya 5-5-5-1)
            {
                Uint16* p = reinterpret_cast<Uint16*>(pixelData + (y * surface->pitch));
                pixel = p[x];
                SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
            }
            break;
            case 24: // 24-bit format
            {
                Uint8* p = pixelData + (y * surface->pitch) + (x * 3);
                if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                    pixel = (p[0] << 16) | (p[1] << 8) | p[2];
                }
                else {
                    pixel = p[0] | (p[1] << 8) | (p[2] << 16);
                }
                SDL_GetRGB(pixel, format, &r, &g, &b);
                a = 255; // 24-bit formatlar genellikle alfa kanalý içermez
            }
            break;
            case 32: // 32-bit format
            {
                Uint32* p = reinterpret_cast<Uint32*>(pixelData + (y * surface->pitch));
                pixel = p[x];
                SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
            }
            break;
            default:
                std::cerr << "Unsupported pixel format: " << static_cast<int>(format->BitsPerPixel) << " bits per pixel." << std::endl;
                SDL_UnlockSurface(surface);
                SDL_FreeSurface(surface);
                return; // Desteklenmeyen format durumunda fonksiyondan çýk
            }

            // Debug output to check pixel values
            // std::cout << "Pixel (" << x << ", " << y << "): R=" << static_cast<int>(r) << ", G=" << static_cast<int>(g) << ", B=" << static_cast<int>(b) << ", A=" << static_cast<int>(a) << std::endl;

            float normalized_r = r / 255.0f;
            float normalized_g = g / 255.0f;
            float normalized_b = b / 255.0f;
            float normalized_a = a / 255.0f;

            // Alfa deðerini de kullanmak isterseniz:
            // pixels[y * width + x] = Vec4(normalized_r, normalized_g, normalized_b, normalized_a);
            // Eðer Vec3 kullanmaya devam etmek istiyorsanýz:
            pixels[y * width + x] = Vec3(normalized_r, normalized_g, normalized_b);
        }
    }


    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);
}

Vec3 Texture::get_color(double u, double v) const {
    u = std::clamp(u, 0.0, 1.0);
    v = std::clamp(v, 0.0, 1.0);

    double x = u * (width - 1);
    double y = (1 - v) * (height - 1);  // Flip y-coordinate if necessary

    int x0 = std::floor(x);
    int x1 = std::min(x0 + 1, width - 1);
    int y0 = std::floor(y);
    int y1 = std::min(y0 + 1, height - 1);

    double tx = x - x0;
    double ty = y - y0;

    Vec3 c00 = pixels[y0 * width + x0];
    Vec3 c10 = pixels[y0 * width + x1];
    Vec3 c01 = pixels[y1 * width + x0];
    Vec3 c11 = pixels[y1 * width + x1];

    Vec3 c0 = c00 * (1 - tx) + c10 * tx;
    Vec3 c1 = c01 * (1 - tx) + c11 * tx;   
    return c0 * (1 - ty) + c1 * ty;
}


Texture::~Texture() {
    // SDL_image'in kullandýðý kaynaklarý temizle
    IMG_Quit();
}