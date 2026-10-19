#pragma once

#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <type_traits>
#include <utility>

#include "math/matrix.hpp"

namespace bmath {

template<typename T, std::size_t N>
class Vector;

template<typename T, std::size_t N>
constexpr T dot(const Vector<T, N>& a, const Vector<T, N>& b);

template<typename T, std::size_t N>
class Vector : public Matrix<T, N, 1> {
protected:
    using Matrix<T, N, 1>::m_Data;

public:
    static_assert(N > 0, "Vector dimension must be strictly positive");

    static constexpr std::size_t Dim = N;

    using Matrix<T, N, 1>::operator[];
    using Matrix<T, N, 1>::operator*=;
    using Matrix<T, N, 1>::operator();
    using Matrix<T, N, 1>::TotalSize;

    // --- Constructors ---

    constexpr Vector() = default;

    constexpr Vector(const Matrix<T, N, 1>& m) : Matrix<T, N, 1>(m) {}

    constexpr Vector(const Tensor<T, N, 1>& t) : Matrix<T, N, 1>(t) {}

    constexpr explicit Vector(const std::array<T, N>& arr) : Matrix<T, N, 1>(arr) {}

    constexpr explicit Vector(const T& fill_value) : Matrix<T, N, 1>(fill_value) {}

    template<typename... Args>
        requires (sizeof...(Args) == N && sizeof...(Args) > 1 && (std::is_convertible_v<Args, T> && ...))
    constexpr explicit Vector(Args&&... args)
        : Matrix<T, N, 1>(std::forward<Args>(args)...) {}

    constexpr explicit Vector(const T& val) requires (N == 1)
        : Matrix<T, 1, 1>(val) {}

    constexpr Vector(std::initializer_list<T> list) : Matrix<T, N, 1>(list) {}

    T determinant() const = delete;
    core::Result<Vector> inverse() const = delete;

    // --- Component Accessors ---

    constexpr T& x() noexcept requires (N >= 1) { return (*this)[0]; }
    constexpr const T& x() const noexcept requires (N >= 1) { return (*this)[0]; }

    constexpr T& y() noexcept requires (N >= 2) { return (*this)[1]; }
    constexpr const T& y() const noexcept requires (N >= 2) { return (*this)[1]; }

    constexpr T& z() noexcept requires (N >= 3) { return (*this)[2]; }
    constexpr const T& z() const noexcept requires (N >= 3) { return (*this)[2]; }

    constexpr T& w() noexcept requires (N >= 4) { return (*this)[3]; }
    constexpr const T& w() const noexcept requires (N >= 4) { return (*this)[3]; }

    // --- Geometric Operations (Methods) ---

    constexpr T length_squared() const noexcept {
        return bmath::dot(*this, *this);
    }

    constexpr auto length() const noexcept {
        using std::sqrt;
        return sqrt(this->length_squared());
    }

    constexpr Vector normalized() const {
        Vector copy = *this;
        copy.normalize();
        return copy;
    }

    constexpr Vector& normalize() {
        const auto len = this->length();
        if (len == static_cast<decltype(len)>(0)) return *this;
        const auto inv_len = static_cast<T>(1) / len;
        *this *= inv_len;
        return *this;
    }
};

// =========================================================================
// Standalone Vector Operations
// =========================================================================

template<typename T, std::size_t N>
constexpr T dot(const Vector<T, N>& a, const Vector<T, N>& b) {
    T sum = static_cast<T>(0);
    for (std::size_t i = 0; i < N; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

template<typename T, std::size_t N>
constexpr T length_squared(const Vector<T, N>& v) noexcept {
    return v.length_squared();
}

template<typename T, std::size_t N>
constexpr auto length(const Vector<T, N>& v) noexcept {
    return v.length();
}

template<typename T, std::size_t N>
constexpr Vector<T, N> normalize(const Vector<T, N>& v) {
    return v.normalized();
}

template<typename T>
constexpr Vector<T, 3> cross(const Vector<T, 3>& a, const Vector<T, 3>& b) {
    return Vector<T, 3>{
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    };
}

template<typename T, std::size_t N>
constexpr T distance_squared(const Vector<T, N>& a, const Vector<T, N>& b) {
    const Vector<T, N> diff = a - b;
    return diff.length_squared();
}

template<typename T, std::size_t N>
constexpr auto distance(const Vector<T, N>& a, const Vector<T, N>& b) {
    const Vector<T, N> diff = a - b;
    return diff.length();
}

template<typename T, std::size_t N>
constexpr Vector<T, N> reflect(const Vector<T, N>& v, const Vector<T, N>& n) {
    return Vector<T, N>(v - n * (static_cast<T>(2) * dot(v, n)));
}

template<typename T, std::size_t R, std::size_t N>
constexpr Vector<T, R> operator*(const Matrix<T, R, N>& m, const Vector<T, N>& v) {
    return Vector<T, R>(m.operator*(v));
}

template<typename T, std::size_t N>
std::ostream& operator<<(std::ostream& os, const Vector<T, N>& v) {
    os << "(";
    for (std::size_t i = 0; i < N; ++i) {
        os << v[i] << (i + 1 < N ? ", " : "");
    }
    os << ")";
    return os;
}

// Type aliases
using Vec1f = Vector<float, 1>;
using Vec2f = Vector<float, 2>;
using Vec3f = Vector<float, 3>;
using Vec4f = Vector<float, 4>;

using Vec1d = Vector<double, 1>;
using Vec2d = Vector<double, 2>;
using Vec3d = Vector<double, 3>;
using Vec4d = Vector<double, 4>;

using Vec1i = Vector<int, 1>;
using Vec2i = Vector<int, 2>;
using Vec3i = Vector<int, 3>;
using Vec4i = Vector<int, 4>;

} // namespace bmath
