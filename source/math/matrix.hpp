#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <type_traits>
#include <utility>

#include <core/massert.hpp>
#include <core/error.hpp>

#include "math/tensor.hpp"

namespace bmath {

template<typename T, std::size_t N, std::size_t M>
class Matrix : public Tensor<T, N, M> {
protected:
    using Tensor<T, N, M>::m_Data;

public:
    static_assert(N > 0 && M > 0, "Matrix dimensions must be strictly positive");

    static constexpr std::size_t Rows = N;
    static constexpr std::size_t Cols = M;

    using Tensor<T, N, M>::operator[];
    using Tensor<T, N, M>::operator*=;
    using Tensor<T, N, M>::operator^=;
    using Tensor<T, N, M>::TotalSize;

    constexpr Matrix() = default;

    constexpr Matrix(const Tensor<T, N, M>& t) : Tensor<T, N, M>(t) {}

    constexpr explicit Matrix(const std::array<T, N * M>& data) : Tensor<T, N, M>(data) {}

    constexpr explicit Matrix(const T& fill_value) : Tensor<T, N, M>(fill_value) {}

    template<typename... Args>
        requires (sizeof...(Args) == N * M && sizeof...(Args) > 1 && (std::is_convertible_v<Args, T> && ...))
    constexpr explicit Matrix(Args&&... args)
        : Tensor<T, N, M>(std::forward<Args>(args)...) {}

    constexpr explicit Matrix(const T& val) requires (N * M == 1)
        : Tensor<T, N, M>(val) {}

    constexpr Matrix(std::initializer_list<std::initializer_list<T>> nested_list) {
        std::size_t r = 0;
        for (const auto& row_list : nested_list) {
            if (r >= N) break;
            std::size_t c = 0;
            for (const auto& val : row_list) {
                if (c >= M) break;
                (*this)(r, c) = val;
                ++c;
            }
            ++r;
        }
    }

    constexpr Matrix(std::initializer_list<T> flat_list) {
        std::size_t i = 0;
        for (const auto& val : flat_list) {
            if (i >= N * M) break;
            this->m_Data[i++] = val;
        }
    }

    static constexpr Matrix identity() requires (N == M) {
        Matrix res{};
        for (std::size_t i = 0; i < N; ++i) {
            res(i, i) = static_cast<T>(1);
        }
        return res;
    }

    static constexpr Matrix zeros() noexcept { return Matrix{}; }
    static constexpr Matrix ones() noexcept { return Matrix{static_cast<T>(1)}; }

    static constexpr std::size_t rows() noexcept { return N; }
    static constexpr std::size_t cols() noexcept { return M; }
    static constexpr bool is_square() noexcept { return N == M; }

    constexpr T& operator()(std::size_t r, std::size_t c) {
        massert(r < N && c < M, "Matrix index out of bounds");
        return this->m_Data[r * M + c];
    }

    constexpr const T& operator()(std::size_t r, std::size_t c) const {
        massert(r < N && c < M, "Matrix index out of bounds");
        return this->m_Data[r * M + c];
    }

    constexpr T& operator[](std::size_t r, std::size_t c) {
        massert(r < N && c < M, "Matrix index out of bounds");
        return this->m_Data[r * M + c];
    }

    constexpr const T& operator[](std::size_t r, std::size_t c) const {
        massert(r < N && c < M, "Matrix index out of bounds");
        return this->m_Data[r * M + c];
    }

    constexpr T& at(std::size_t r, std::size_t c) {
        massert(r < N && c < M, "Matrix index out of bounds");
        return this->m_Data[r * M + c];
    }

    constexpr const T& at(std::size_t r, std::size_t c) const {
        massert(r < N && c < M, "Matrix index out of bounds");
        return this->m_Data[r * M + c];
    }

    // ===== Matrix x Matrix Multiplication (Linear Algebra: A x B) =====

    template<std::size_t P>
    constexpr Matrix<T, N, P> operator*(const Matrix<T, M, P>& other) const {
        Matrix<T, N, P> result{};
        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t k = 0; k < M; ++k) {
                const T aik = (*this)(i, k);
                for (std::size_t j = 0; j < P; ++j) {
                    result(i, j) += aik * other(k, j);
                }
            }
        }
        return result;
    }

    constexpr Matrix& operator*=(const Matrix<T, M, M>& other) requires (N == M) {
        *this = *this * other;
        return *this;
    }

    // ===== Transpose =====

    constexpr Matrix<T, M, N> transpose() const noexcept {
        Matrix<T, M, N> result{};
        for (std::size_t i = 0; i < N; ++i) {
            for (std::size_t j = 0; j < M; ++j) {
                result(j, i) = (*this)(i, j);
            }
        }
        return result;
    }

    // ===== Determinant =====

    constexpr T determinant() const requires (N == M) && (N == 1) {
        return (*this)(0, 0);
    }

    constexpr T determinant() const requires (N == M) && (N == 2) {
        return (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
    }

    constexpr T determinant() const requires (N == M) && (N == 3) {
        return (*this)(0, 0) * ((*this)(1, 1) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 1))
             - (*this)(0, 1) * ((*this)(1, 0) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 0))
             + (*this)(0, 2) * ((*this)(1, 0) * (*this)(2, 1) - (*this)(1, 1) * (*this)(2, 0));
    }

    constexpr T determinant() const requires (N == M) && (N == 4) {
        const T s0 = (*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1);
        const T s1 = (*this)(0, 0) * (*this)(1, 2) - (*this)(1, 0) * (*this)(0, 2);
        const T s2 = (*this)(0, 0) * (*this)(1, 3) - (*this)(1, 0) * (*this)(0, 3);
        const T s3 = (*this)(0, 1) * (*this)(1, 2) - (*this)(1, 1) * (*this)(0, 2);
        const T s4 = (*this)(0, 1) * (*this)(1, 3) - (*this)(1, 1) * (*this)(0, 3);
        const T s5 = (*this)(0, 2) * (*this)(1, 3) - (*this)(1, 2) * (*this)(0, 3);

        const T c5 = (*this)(2, 2) * (*this)(3, 3) - (*this)(3, 2) * (*this)(2, 3);
        const T c4 = (*this)(2, 1) * (*this)(3, 3) - (*this)(3, 1) * (*this)(2, 3);
        const T c3 = (*this)(2, 1) * (*this)(3, 2) - (*this)(3, 1) * (*this)(2, 2);
        const T c2 = (*this)(2, 0) * (*this)(3, 3) - (*this)(3, 0) * (*this)(2, 3);
        const T c1 = (*this)(2, 0) * (*this)(3, 2) - (*this)(3, 0) * (*this)(2, 2);
        const T c0 = (*this)(2, 0) * (*this)(3, 1) - (*this)(3, 0) * (*this)(2, 1);

        return s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
    }

    constexpr T determinant() const requires (N == M) && (N > 4) {
        Matrix<T, N, N> a = *this;
        T det = static_cast<T>(1);

        for (std::size_t i = 0; i < N; ++i) {
            std::size_t pivot = i;
            for (std::size_t j = i + 1; j < N; ++j) {
                using std::abs;
                if (abs(a(j, i)) > abs(a(pivot, i))) {
                    pivot = j;
                }
            }

            using std::abs;
            if (abs(a(pivot, i)) == static_cast<T>(0)) {
                return static_cast<T>(0);
            }

            if (pivot != i) {
                for (std::size_t k = 0; k < N; ++k) {
                    std::swap(a(i, k), a(pivot, k));
                }
                det = -det;
            }

            det *= a(i, i);
            const T pivot_val = a(i, i);

            for (std::size_t j = i + 1; j < N; ++j) {
                const T factor = a(j, i) / pivot_val;
                for (std::size_t k = i + 1; k < N; ++k) {
                    a(j, k) -= factor * a(i, k);
                }
            }
        }
        return det;
    }

    // ===== Inverse =====

    constexpr core::Result<Matrix<T, 1, 1>> inverse() const requires (N == M) && (N == 1) {
        REQUIRE((*this)(0, 0) != static_cast<T>(0), "Matrix is singular: determinant is zero", 1);
        Matrix<T, 1, 1> res{};
        res(0, 0) = static_cast<T>(1) / (*this)(0, 0);
        return res;
    }

    constexpr core::Result<Matrix<T, 2, 2>> inverse() const requires (N == M) && (N == 2) {
        const T det = determinant();
        REQUIRE(det != static_cast<T>(0), "Matrix is singular: determinant is zero", 1);
        const T inv_det = static_cast<T>(1) / det;
        Matrix<T, 2, 2> res{};
        res(0, 0) =  (*this)(1, 1) * inv_det;
        res(0, 1) = -(*this)(0, 1) * inv_det;
        res(1, 0) = -(*this)(1, 0) * inv_det;
        res(1, 1) =  (*this)(0, 0) * inv_det;
        return res;
    }

    constexpr core::Result<Matrix<T, 3, 3>> inverse() const requires (N == M) && (N == 3) {
        const T det = determinant();
        REQUIRE(det != static_cast<T>(0), "Matrix is singular: determinant is zero", 1);
        const T inv_det = static_cast<T>(1) / det;
        Matrix<T, 3, 3> res{};
        res(0, 0) = ((*this)(1, 1) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 1)) * inv_det;
        res(0, 1) = ((*this)(0, 2) * (*this)(2, 1) - (*this)(0, 1) * (*this)(2, 2)) * inv_det;
        res(0, 2) = ((*this)(0, 1) * (*this)(1, 2) - (*this)(0, 2) * (*this)(1, 1)) * inv_det;

        res(1, 0) = ((*this)(1, 2) * (*this)(2, 0) - (*this)(1, 0) * (*this)(2, 2)) * inv_det;
        res(1, 1) = ((*this)(0, 0) * (*this)(2, 2) - (*this)(0, 2) * (*this)(2, 0)) * inv_det;
        res(1, 2) = ((*this)(0, 2) * (*this)(1, 0) - (*this)(0, 0) * (*this)(1, 2)) * inv_det;

        res(2, 0) = ((*this)(1, 0) * (*this)(2, 1) - (*this)(1, 1) * (*this)(2, 0)) * inv_det;
        res(2, 1) = ((*this)(0, 1) * (*this)(2, 0) - (*this)(0, 0) * (*this)(2, 1)) * inv_det;
        res(2, 2) = ((*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0)) * inv_det;
        return res;
    }

    constexpr core::Result<Matrix<T, 4, 4>> inverse() const requires (N == M) && (N == 4) {
        const T s0 = (*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1);
        const T s1 = (*this)(0, 0) * (*this)(1, 2) - (*this)(1, 0) * (*this)(0, 2);
        const T s2 = (*this)(0, 0) * (*this)(1, 3) - (*this)(1, 0) * (*this)(0, 3);
        const T s3 = (*this)(0, 1) * (*this)(1, 2) - (*this)(1, 1) * (*this)(0, 2);
        const T s4 = (*this)(0, 1) * (*this)(1, 3) - (*this)(1, 1) * (*this)(0, 3);
        const T s5 = (*this)(0, 2) * (*this)(1, 3) - (*this)(1, 2) * (*this)(0, 3);

        const T c5 = (*this)(2, 2) * (*this)(3, 3) - (*this)(3, 2) * (*this)(2, 3);
        const T c4 = (*this)(2, 1) * (*this)(3, 3) - (*this)(3, 1) * (*this)(2, 3);
        const T c3 = (*this)(2, 1) * (*this)(3, 2) - (*this)(3, 1) * (*this)(2, 2);
        const T c2 = (*this)(2, 0) * (*this)(3, 3) - (*this)(3, 0) * (*this)(2, 3);
        const T c1 = (*this)(2, 0) * (*this)(3, 2) - (*this)(3, 0) * (*this)(2, 2);
        const T c0 = (*this)(2, 0) * (*this)(3, 1) - (*this)(3, 0) * (*this)(2, 1);

        const T det = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
        REQUIRE(det != static_cast<T>(0), "Matrix is singular: determinant is zero", 1);
        const T inv_det = static_cast<T>(1) / det;

        Matrix<T, 4, 4> res{};
        res(0, 0) =  ((*this)(1, 1) * c5 - (*this)(1, 2) * c4 + (*this)(1, 3) * c3) * inv_det;
        res(0, 1) = (-(*this)(0, 1) * c5 + (*this)(0, 2) * c4 - (*this)(0, 3) * c3) * inv_det;
        res(0, 2) =  ((*this)(3, 1) * s5 - (*this)(3, 2) * s4 + (*this)(3, 3) * s3) * inv_det;
        res(0, 3) = (-(*this)(2, 1) * s5 + (*this)(2, 2) * s4 - (*this)(2, 3) * s3) * inv_det;

        res(1, 0) = (-(*this)(1, 0) * c5 + (*this)(1, 2) * c2 - (*this)(1, 3) * c1) * inv_det;
        res(1, 1) =  ((*this)(0, 0) * c5 - (*this)(0, 2) * c2 + (*this)(0, 3) * c1) * inv_det;
        res(1, 2) = (-(*this)(3, 0) * s5 + (*this)(3, 2) * s2 - (*this)(3, 3) * s1) * inv_det;
        res(1, 3) =  ((*this)(2, 0) * s5 - (*this)(2, 2) * s2 + (*this)(2, 3) * s1) * inv_det;

        res(2, 0) =  ((*this)(1, 0) * c4 - (*this)(1, 1) * c2 + (*this)(1, 3) * c0) * inv_det;
        res(2, 1) = (-(*this)(0, 0) * c4 + (*this)(0, 1) * c2 - (*this)(0, 3) * c0) * inv_det;
        res(2, 2) =  ((*this)(3, 0) * s4 - (*this)(3, 1) * s2 + (*this)(3, 3) * s0) * inv_det;
        res(2, 3) = (-(*this)(2, 0) * s4 + (*this)(2, 1) * s2 - (*this)(2, 3) * s0) * inv_det;

        res(3, 0) = (-(*this)(1, 0) * c3 + (*this)(1, 1) * c1 - (*this)(1, 2) * c0) * inv_det;
        res(3, 1) =  ((*this)(0, 0) * c3 - (*this)(0, 1) * c1 + (*this)(0, 2) * c0) * inv_det;
        res(3, 2) = (-(*this)(3, 0) * s3 + (*this)(3, 1) * s1 - (*this)(3, 2) * s0) * inv_det;
        res(3, 3) =  ((*this)(2, 0) * s3 - (*this)(2, 1) * s1 + (*this)(2, 2) * s0) * inv_det;

        return res;
    }

    constexpr core::Result<Matrix<T, N, N>> inverse() const requires (N == M) && (N > 4) {
        Matrix<T, N, N> a = *this;
        Matrix<T, N, N> inv = Matrix<T, N, N>::identity();

        for (std::size_t i = 0; i < N; ++i) {
            std::size_t pivot = i;
            for (std::size_t j = i + 1; j < N; ++j) {
                using std::abs;
                if (abs(a(j, i)) > abs(a(pivot, i))) {
                    pivot = j;
                }
            }

            using std::abs;
            REQUIRE(abs(a(pivot, i)) != static_cast<T>(0), "Matrix is singular: determinant is zero", 1);

            if (pivot != i) {
                for (std::size_t k = 0; k < N; ++k) {
                    std::swap(a(i, k), a(pivot, k));
                    std::swap(inv(i, k), inv(pivot, k));
                }
            }

            const T pivot_val = a(i, i);
            const T inv_pivot = static_cast<T>(1) / pivot_val;
            for (std::size_t k = 0; k < N; ++k) {
                a(i, k) *= inv_pivot;
                inv(i, k) *= inv_pivot;
            }

            for (std::size_t j = 0; j < N; ++j) {
                if (j != i) {
                    const T factor = a(j, i);
                    for (std::size_t k = 0; k < N; ++k) {
                        a(j, k) -= factor * a(i, k);
                        inv(j, k) -= factor * inv(i, k);
                    }
                }
            }
        }
        return inv;
    }
};

// ==== Free Functions: Transpose, Determinant, Inverse, Print ====

template<typename T, std::size_t N, std::size_t M>
constexpr Matrix<T, M, N> transpose(const Matrix<T, N, M>& mat) noexcept {
    return mat.transpose();
}

template<typename T, std::size_t N>
constexpr T determinant(const Matrix<T, N, N>& mat) {
    return mat.determinant();
}

template<typename T, std::size_t N>
constexpr core::Result<Matrix<T, N, N>> inverse(const Matrix<T, N, N>& mat) {
    return mat.inverse();
}

template<typename T, std::size_t N, std::size_t M>
std::ostream& operator<<(std::ostream& os, const Matrix<T, N, M>& mat) {
    os << "[\n";
    for (std::size_t i = 0; i < N; ++i) {
        os << "  [";
        for (std::size_t j = 0; j < M; ++j) {
            os << mat(i, j) << (j + 1 < M ? ", " : "");
        }
        os << "]" << (i + 1 < N ? ",\n" : "\n");
    }
    os << "]";
    return os;
}

using Mat2f = Matrix<float, 2, 2>;
using Mat3f = Matrix<float, 3, 3>;
using Mat4f = Matrix<float, 4, 4>;

using Mat2d = Matrix<double, 2, 2>;
using Mat3d = Matrix<double, 3, 3>;
using Mat4d = Matrix<double, 4, 4>;

using Mat2i = Matrix<int, 2, 2>;
using Mat3i = Matrix<int, 3, 3>;
using Mat4i = Matrix<int, 4, 4>;

}
