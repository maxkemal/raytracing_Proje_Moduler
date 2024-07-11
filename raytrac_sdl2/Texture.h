#pragma once
#include <vector>
#include <string>
#include "Vec3.h"
#include <SDL_image.h>
#include "Vec2.h"

class Texture {
private:
    std::vector<Vec3> pixels;
    int width;
    int height;
     bool m_is_loaded = false;
public:
    Texture(const std::string& filename);
    Vec3 getColor(const Vec2& uv) const {
        return get_color(uv.u, uv.v);
    }

    Vec3 get_color(double u, double v) const;
    ~Texture();
    bool is_loaded() const { return m_is_loaded; }
    
};
