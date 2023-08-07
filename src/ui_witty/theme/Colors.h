#ifndef COLORS_H
#define COLORS_H

#include <Wt/WColor.h>

class Colors {
public:
    static auto blue(uint8_t alpha = 255) { return Wt::WColor("#375a7f" + toHex(alpha)); }
    static auto indigo(uint8_t alpha = 255) { return Wt::WColor("#6610f2" + toHex(alpha)); }
    static auto purple(uint8_t alpha = 255) { return Wt::WColor("#6f42c1" + toHex(alpha)); }
    static auto pink(uint8_t alpha = 255) { return Wt::WColor("#e83e8c" + toHex(alpha)); }
    static auto red(uint8_t alpha = 255) { return Wt::WColor("#e74c3c" + toHex(alpha)); }
    static auto orange(uint8_t alpha = 255) { return Wt::WColor("#fd7e14" + toHex(alpha)); }
    static auto yellow(uint8_t alpha = 255) { return Wt::WColor("#f39c12" + toHex(alpha)); }
    static auto green(uint8_t alpha = 255) { return Wt::WColor("#00bc8c" + toHex(alpha)); }
    static auto teal(uint8_t alpha = 255) { return Wt::WColor("#20c997" + toHex(alpha)); }
    static auto cyan(uint8_t alpha = 255) { return Wt::WColor("#3498db" + toHex(alpha)); }
    static auto black(uint8_t alpha = 255) { return Wt::WColor("#000000" + toHex(alpha)); }
    static auto white(uint8_t alpha = 255) { return Wt::WColor("#ffffff" + toHex(alpha)); }
    static auto gray(uint8_t alpha = 255) { return Wt::WColor("#808080" + toHex(alpha)); }
    static auto grayDark(uint8_t alpha = 255) { return Wt::WColor("#303030" + toHex(alpha)); }
    static auto gray100(uint8_t alpha = 255) { return Wt::WColor("#f8f9fa" + toHex(alpha)); }
    static auto gray200(uint8_t alpha = 255) { return Wt::WColor("#ebebeb" + toHex(alpha)); }
    static auto gray300(uint8_t alpha = 255) { return Wt::WColor("#dee2e6" + toHex(alpha)); }
    static auto gray400(uint8_t alpha = 255) { return Wt::WColor("#ced4da" + toHex(alpha)); }
    static auto gray500(uint8_t alpha = 255) { return Wt::WColor("#adb5bd" + toHex(alpha)); }
    static auto gray600(uint8_t alpha = 255) { return Wt::WColor("#808080" + toHex(alpha)); }
    static auto gray700(uint8_t alpha = 255) { return Wt::WColor("#404040" + toHex(alpha)); }
    static auto gray800(uint8_t alpha = 255) { return Wt::WColor("#303030" + toHex(alpha)); }
    static auto gray900(uint8_t alpha = 255) { return Wt::WColor("#202020" + toHex(alpha)); }
    static auto primary(uint8_t alpha = 255) { return Wt::WColor("#375a7f" + toHex(alpha)); }
    static auto secondary(uint8_t alpha = 255) { return Wt::WColor("#404040" + toHex(alpha)); }
    static auto success(uint8_t alpha = 255) { return Wt::WColor("#00bc8c" + toHex(alpha)); }
    static auto info(uint8_t alpha = 255) { return Wt::WColor("#3498db" + toHex(alpha)); }
    static auto warning(uint8_t alpha = 255) { return Wt::WColor("#f39c12" + toHex(alpha)); }
    static auto danger(uint8_t alpha = 255) { return Wt::WColor("#e74c3c" + toHex(alpha)); }
    static auto light(uint8_t alpha = 255) { return Wt::WColor("#adb5bd" + toHex(alpha)); }
    static auto dark(uint8_t alpha = 255) { return Wt::WColor("#303030" + toHex(alpha)); }
    static auto background(uint8_t alpha = 255) { return Wt::WColor("#222222" + toHex(alpha)); }

    static auto withAlpha(const Wt::WColor& color, double alpha) {
        double hsl[3];
        color.toHSL(hsl);
        hsl[1] *= alpha; hsl[2] *= alpha;
        return Wt::WColor::fromHSL(hsl[0], hsl[1], hsl[2], 255);
    }

    static auto withLightness(const Wt::WColor& color, double lightness) {
        double hsl[3];
        color.toHSL(hsl);
        hsl[2] *= lightness;
        return Wt::WColor::fromHSL(hsl[0], hsl[1], hsl[2], 255);
    }

private:
    Colors();

    template <typename T>
    static Wt::WString toHex(T w, size_t hex_len = sizeof(T)<<1) {
        static const char* digits = "0123456789abcdef";
        std::string rc(hex_len, '0');
        for (size_t i = 0, j = (hex_len-1)*4 ; i < hex_len; ++i ,j-=4)
            rc[i] = digits[(w>>j) & 0x0f];
        return rc;
    }
};

#endif // COLORS_H
