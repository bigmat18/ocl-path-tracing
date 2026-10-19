#pragma once 

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <core/massert.hpp>

namespace bmath {

template<typename T, std::size_t...Ds>
class Tensor {
public:
    static constexpr std::size_t TotalSize = (1 * ... * Ds);

protected:
    static_assert(((Ds > 0) && ...), "Matrix dimensions must be strictly positive");

    std::array<T, TotalSize> m_Data{};

public:
    constexpr Tensor() = default;

    constexpr explicit Tensor(const std::array<T, TotalSize>& data) : m_Data(data) {}

    template<typename... Args>
        requires (sizeof...(Args) == TotalSize && sizeof...(Args) > 1 && (std::is_convertible_v<Args, T> && ...))
    constexpr explicit Tensor(Args&&... args)
        : m_Data{static_cast<T>(std::forward<Args>(args))...} {}

    constexpr explicit Tensor(const T& fill_value) {
        m_Data.fill(fill_value);
    }

    constexpr T& operator[](std::size_t i) {
        massert(i < TotalSize, "Index out of bounds in Tensor");
        return m_Data[i];
    }

    constexpr const T& operator[](std::size_t i) const {
        massert(i < TotalSize, "Index out of bounds in Tensor");
        return m_Data[i];
    }

    // ===== Tensor x Tensor operations =====   
    constexpr Tensor<T, Ds...>& operator+=(const Tensor<T, Ds...>& t) {
        for (std::size_t i = 0; i < TotalSize; ++i)
            this->m_Data[i] += t.m_Data[i];

        return *this;
    }

    constexpr Tensor<T, Ds...>& operator-=(const Tensor<T, Ds...>& t) {
        for (std::size_t i = 0; i < TotalSize; ++i)
            this->m_Data[i] -= t.m_Data[i];
        return *this;
    }

    constexpr Tensor<T, Ds...>& operator^=(const Tensor<T, Ds...>& t) {
        for (std::size_t i = 0; i < TotalSize; ++i)
            this->m_Data[i] *= t.m_Data[i];
        return *this;
    }

    constexpr Tensor<T, Ds...>& operator/=(const Tensor<T, Ds...>& t) {
        for (std::size_t i = 0; i < TotalSize; ++i)
            this->m_Data[i] /= t.m_Data[i];
        return *this;
    }

    // ===== Tensor x Scalar operations =====
    constexpr Tensor<T, Ds...>& operator+=(const T& v) {
        for (std::size_t i = 0; i < TotalSize; ++i)
            this->m_Data[i] += v;

        return *this;
    }

    constexpr Tensor<T, Ds...>& operator-=(const T& v) {
        for (std::size_t i = 0; i < TotalSize; ++i)
            this->m_Data[i] -= v;
        return *this;
    }

    constexpr Tensor<T, Ds...>& operator*=(const T& v) {
        for (std::size_t i = 0; i < TotalSize; ++i)
            this->m_Data[i] *= v;
        return *this;
    }

    constexpr Tensor<T, Ds...>& operator/=(const T& v) {
        for (std::size_t i = 0; i < TotalSize; ++i)
            this->m_Data[i] /= v;
        return *this;
    }

    constexpr Tensor<T, Ds...> operator-() const {
        Tensor<T, Ds...> result;
        for (std::size_t i = 0; i < TotalSize; ++i)
            result.m_Data[i] = -this->m_Data[i];
        return result;
    }

    // ===== Binary arithmetic operators with Tensor =====
    friend constexpr Tensor<T, Ds...> operator+(Tensor<T, Ds...> a, const Tensor<T, Ds...>& b) {
        a += b;
        return a;
    }

    friend constexpr Tensor<T, Ds...> operator-(Tensor<T, Ds...> a, const Tensor<T, Ds...>& b) {
        a -= b;
        return a;
    }

    friend constexpr Tensor<T, Ds...> operator^(Tensor<T, Ds...> a, const Tensor<T, Ds...>& b) {
        a ^= b;
        return a;
    }

    friend constexpr Tensor<T, Ds...> operator/(Tensor<T, Ds...> a, const Tensor<T, Ds...>& b) {
        a /= b;
        return a;
    }

    // ===== Binary arithmetic operators with scalar =====
    friend constexpr Tensor<T, Ds...> operator+(Tensor<T, Ds...> a, const T& v) {
        a += v;
        return a;
    }

    friend constexpr Tensor<T, Ds...> operator+(const T& v, Tensor<T, Ds...> a) {
        a += v;
        return a;
    }

    friend constexpr Tensor<T, Ds...> operator-(Tensor<T, Ds...> a, const T& v) {
        a -= v;
        return a;
    }

    friend constexpr Tensor<T, Ds...> operator-(const T& v, const Tensor<T, Ds...>& a) {
        Tensor<T, Ds...> result;
        for (std::size_t i = 0; i < TotalSize; ++i)
            result.m_Data[i] = v - a.m_Data[i];
        return result;
    }

    friend constexpr Tensor<T, Ds...> operator*(Tensor<T, Ds...> a, const T& v) {
        a *= v;
        return a;
    }

    friend constexpr Tensor<T, Ds...> operator*(const T& v, Tensor<T, Ds...> a) {
        a *= v;
        return a;
    }

    friend constexpr Tensor<T, Ds...> operator/(Tensor<T, Ds...> a, const T& v) {
        a /= v;
        return a;
    }

    friend constexpr Tensor<T, Ds...> operator/(const T& v, const Tensor<T, Ds...>& a) {
        Tensor<T, Ds...> result;
        for (std::size_t i = 0; i < TotalSize; ++i)
            result.m_Data[i] = v / a.m_Data[i];
        return result;
    }

    friend constexpr bool operator==(const Tensor<T, Ds...>& a, const Tensor<T, Ds...>& b) noexcept {
        return a.m_Data == b.m_Data;
    }
};

}
