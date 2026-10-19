#pragma once 

#include <algorithm>
#include <cstdint>
#include <ostream>

namespace bmath {

class Color {
    uint32_t m_Color{0};

public:
    constexpr explicit Color() : m_Color(0) {}

    constexpr explicit Color(uint32_t color) : m_Color(color) {}

    constexpr explicit Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : m_Color((static_cast<uint32_t>(r) << 24) |
                  (static_cast<uint32_t>(g) << 16) |
                  (static_cast<uint32_t>(b) << 8)  |
                  static_cast<uint32_t>(a)) {}

    constexpr explicit Color(int r, int g, int b, int a = 255)
        : Color(static_cast<uint8_t>(std::clamp(r, 0, 255)),
                static_cast<uint8_t>(std::clamp(g, 0, 255)),
                static_cast<uint8_t>(std::clamp(b, 0, 255)),
                static_cast<uint8_t>(std::clamp(a, 0, 255))) {}

    constexpr explicit Color(float r, float g, float b, float a = 1.0f)
        : Color(static_cast<uint8_t>(std::clamp(r, 0.0f, 1.0f) * 255.0f + 0.5f),
                static_cast<uint8_t>(std::clamp(g, 0.0f, 1.0f) * 255.0f + 0.5f),
                static_cast<uint8_t>(std::clamp(b, 0.0f, 1.0f) * 255.0f + 0.5f),
                static_cast<uint8_t>(std::clamp(a, 0.0f, 1.0f) * 255.0f + 0.5f)) {}

    // ==== Raw Value ====
    constexpr uint32_t rgba() const noexcept { return m_Color; }
    constexpr explicit operator uint32_t() const noexcept { return m_Color; }

    // ==== Access Methods in uint8_t ====
    constexpr uint8_t r() const noexcept { return static_cast<uint8_t>((m_Color >> 24) & 0xFF); }
    constexpr uint8_t g() const noexcept { return static_cast<uint8_t>((m_Color >> 16) & 0xFF); }
    constexpr uint8_t b() const noexcept { return static_cast<uint8_t>((m_Color >> 8) & 0xFF); }
    constexpr uint8_t a() const noexcept { return static_cast<uint8_t>(m_Color & 0xFF); }

    constexpr void r(uint8_t val) noexcept {
        m_Color = (m_Color & 0x00FFFFFFu) | (static_cast<uint32_t>(val) << 24);
    }
    constexpr void g(uint8_t val) noexcept {
        m_Color = (m_Color & 0xFF00FFFFu) | (static_cast<uint32_t>(val) << 16);
    }
    constexpr void b(uint8_t val) noexcept {
        m_Color = (m_Color & 0xFFFF00FFu) | (static_cast<uint32_t>(val) << 8);
    }
    constexpr void a(uint8_t val) noexcept {
        m_Color = (m_Color & 0xFFFFFF00u) | static_cast<uint32_t>(val);
    }

    // ==== Access Methods in float [0.0, 1.0] ====
    constexpr float rf() const noexcept { return static_cast<float>(r()) / 255.0f; }
    constexpr float gf() const noexcept { return static_cast<float>(g()) / 255.0f; }
    constexpr float bf() const noexcept { return static_cast<float>(b()) / 255.0f; }
    constexpr float af() const noexcept { return static_cast<float>(a()) / 255.0f; }

    constexpr void rf(float val) noexcept {
        r(static_cast<uint8_t>(std::clamp(val, 0.0f, 1.0f) * 255.0f + 0.5f));
    }
    constexpr void gf(float val) noexcept {
        g(static_cast<uint8_t>(std::clamp(val, 0.0f, 1.0f) * 255.0f + 0.5f));
    }
    constexpr void bf(float val) noexcept {
        b(static_cast<uint8_t>(std::clamp(val, 0.0f, 1.0f) * 255.0f + 0.5f));
    }
    constexpr void af(float val) noexcept {
        a(static_cast<uint8_t>(std::clamp(val, 0.0f, 1.0f) * 255.0f + 0.5f));
    }

    // ==== Comparisons ====
    constexpr bool operator==(const Color& other) const noexcept = default;

    // ==== Predefined Colors ====
    static constexpr Color black() noexcept { return Color(0x000000FF); }
    static constexpr Color white() noexcept { return Color(0xFFFFFFFF); }
    static constexpr Color red()   noexcept { return Color(0xFF0000FF); }
    static constexpr Color green() noexcept { return Color(0x00FF00FF); }
    static constexpr Color blue()  noexcept { return Color(0x0000FFFF); }
};

inline std::ostream& operator<<(std::ostream& os, const Color& c) {
    os << "Color(" << static_cast<int>(c.r()) << ", "
                   << static_cast<int>(c.g()) << ", "
                   << static_cast<int>(c.b()) << ", "
                   << static_cast<int>(c.a()) << ")";
    return os;
}

} // namespace bmath
