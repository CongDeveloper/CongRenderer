#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <cstdint>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

#pragma pack(push,1)
struct TGA_Header {
    std::uint8_t  idlength{};
    std::uint8_t  colormaptype{};
    std::uint8_t  datatypecode{};
    std::uint16_t colormaporigin{};
    std::uint16_t colormaplength{};
    std::uint8_t  colormapdepth{};
    std::uint16_t x_origin{};
    std::uint16_t y_origin{};
    std::uint16_t width{};
    std::uint16_t height{};
    std::uint8_t  bitsperpixel{};
    std::uint8_t  imagedescriptor{};
};
#pragma pack(pop)

struct Color32 {
    std::uint8_t bgra[4] = { 0,0,0,0 };
    std::uint8_t bytespp = { 0 };

    Color32() = default;
    Color32(const std::uint8_t R, const std::uint8_t G, const std::uint8_t B, const std::uint8_t A = 255) : bgra{ B,G,R,A }, bytespp(4) { }
    Color32(const std::uint8_t v) : bgra{ v,0,0,0 }, bytespp(1) { }

    Color32(const std::uint8_t* p, const std::uint8_t bpp) : bgra{ 0,0,0,0 }, bytespp(bpp) {
        for (int i = 0; i < bpp; i++)
            bgra[i] = p[i];
    }

    std::uint8_t& r() { return bgra[2]; }
    std::uint8_t& g() { return bgra[1]; }
    std::uint8_t& b() { return bgra[0]; }
    std::uint8_t& a() { return bgra[3]; }

    std::uint8_t& operator[](const int i) { return bgra[i]; }

    Color32 operator *(const double intensity) const {
        Color32 res = *this;
        double clamped = std::max(0., std::min(intensity, 1.));
        for (int i = 0; i < 4; i++) res.bgra[i] = bgra[i] * clamped;
        return res;
    }

    Color32 operator +(Color32& c1) {
        auto r = (uint8_t)clamp(this->r() + c1.r(), 0, 255);
        auto g = (uint8_t)clamp(this->g() + c1.g(), 0, 255);
        auto b = (uint8_t)clamp(this->b() + c1.b(), 0, 255);
        auto a = (uint8_t)clamp(this->a() + c1.a(), 0, 255);
        return Color32(r, g, b, a);
    }
};

enum Format { GRAYSCALE = 1, RGB = 3, RGBA = 4 };

class TGAImage {
protected:
    std::vector<std::uint8_t> data;
    int width;
    int height;
    int bytespp;

    bool   load_rle_data(std::ifstream& in);
    bool unload_rle_data(std::ofstream& out) const;
public:

    TGAImage();
    TGAImage(const int w, const int h, const Format format);
    bool  read_tga_file(const std::string filename);
    bool write_tga_file(const std::string filename, const bool vflip = true, const bool rle = true) const;
    void flip_horizontally();
    void flip_vertically();
    void scale(const int w, const int h);
    Color32 get(const int x, const int y) const;
    void set(const int x, const int y, const Color32& c);
    int get_width() const;
    int get_height() const;
    int get_bytespp();
    std::uint8_t* buffer();
    void clear();
};

#endif //__IMAGE_H__

