#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <iostream>
#include <random>
#include <stdexcept>
#include <limits>
#include <type_traits>
#include <cmath>
#include <algorithm>

template <typename T>
class Image {
private:
    static constexpr double EPSILON = 1e-5;
    size_t width_;
    size_t height_;
    T* data_;

    // Защищенная арифметика с проверкой переполнения границ типов
    T safeAdd(T a, T b) const {
        if constexpr (std::is_same_v<T, bool>) {
            return a || b;
        }
        else {
            if (b > 0 && a > std::numeric_limits<T>::max() - b) return std::numeric_limits<T>::max();
            if (b < 0 && a < std::numeric_limits<T>::lowest() - b) return std::numeric_limits<T>::lowest();
            return a + b;
        }
    }

    T safeMul(T a, T b) const {
        if constexpr (std::is_same_v<T, bool>) {
            return a && b;
        }
        else {
            double res = static_cast<double>(a) * static_cast<double>(b);
            if (res > static_cast<double>(std::numeric_limits<T>::max())) return std::numeric_limits<T>::max();
            if (res < static_cast<double>(std::numeric_limits<T>::lowest())) return std::numeric_limits<T>::lowest();
            return static_cast<T>(res);
        }
    }

public:
    // Конструкторы, деструктор и управление памятью (Rule of Three)
    Image(size_t width = 0, size_t height = 0, bool randomFill = false)
        : width_(width), height_(height), data_(nullptr) {
        if (width_ > 0 && height_ > 0) {
            data_ = new T[width_ * height_]();
            if (randomFill) {
                std::random_device rd;
                std::mt19937 gen(rd());
                if constexpr (std::is_same_v<T, bool>) {
                    std::bernoulli_distribution dist(0.5);
                    for (size_t i = 0; i < width_ * height_; ++i) data_[i] = dist(gen);
                }
                else if constexpr (std::is_integral_v<T>) {
                    std::uniform_int_distribution<int> dist(0, 255);
                    for (size_t i = 0; i < width_ * height_; ++i) data_[i] = static_cast<T>(dist(gen));
                }
                else if constexpr (std::is_floating_point_v<T>) {
                    std::uniform_real_distribution<T> dist(0.0, 1.0);
                    for (size_t i = 0; i < width_ * height_; ++i) data_[i] = dist(gen);
                }
            }
        }
    }

    Image(const Image& other) : width_(other.width_), height_(other.height_), data_(nullptr) {
        if (width_ > 0 && height_ > 0) {
            data_ = new T[width_ * height_];
            for (size_t i = 0; i < width_ * height_; ++i) data_[i] = other.data_[i];
        }
    }

    Image& operator=(const Image& other) {
        if (this != &other) {
            T* newData = nullptr;
            if (other.width_ > 0 && other.height_ > 0) {
                newData = new T[other.width_ * other.height_];
                for (size_t i = 0; i < other.width_ * other.height_; ++i) newData[i] = other.data_[i];
            }
            delete[] data_;
            data_ = newData;
            width_ = other.width_;
            height_ = other.height_;
        }
        return *this;
    }

    ~Image() { delete[] data_; }

    size_t getWidth() const { return width_; }
    size_t getHeight() const { return height_; }

    // Безопасный доступ к элементам матрицы по индексам
    T& operator()(size_t row, size_t col) {
        if (row >= height_ || col >= width_) throw std::out_of_range("Image index out of bounds");
        return data_[row * width_ + col];
    }

    const T& operator()(size_t row, size_t col) const {
        if (row >= height_ || col >= width_) throw std::out_of_range("Image index out of bounds");
        return data_[row * width_ + col];
    }

    // Операции сравнения изображений
    bool operator==(const Image& other) const {
        if (width_ != other.width_ || height_ != other.height_) return false;
        for (size_t i = 0; i < width_ * height_; ++i) {
            if constexpr (std::is_floating_point_v<T>) {
                if (std::abs(data_[i] - other.data_[i]) > EPSILON) return false;
            }
            else {
                if (data_[i] != other.data_[i]) return false;
            }
        }
        return true;
    }

    bool operator!=(const Image& other) const {
        return !(*this == other);
    }

    // Арифметические перегрузки операторов для изображений и констант
    Image operator*(const Image& other) const {
        size_t resW = std::max(width_, other.width_);
        size_t resH = std::max(height_, other.height_);
        Image result(resW, resH, false);
        for (size_t r = 0; r < resH; ++r) {
            for (size_t c = 0; c < resW; ++c) {
                T v1 = (r < height_ && c < width_) ? (*this)(r, c) : static_cast<T>(0);
                T v2 = (r < other.height_ && c < other.width_) ? other(r, c) : static_cast<T>(0);
                result(r, c) = safeMul(v1, v2);
            }
        }
        return result;
    }

    Image operator+(const Image& other) const {
        size_t resW = std::max(width_, other.width_);
        size_t resH = std::max(height_, other.height_);
        Image result(resW, resH, false);
        for (size_t r = 0; r < resH; ++r) {
            for (size_t c = 0; c < resW; ++c) {
                T v1 = (r < height_ && c < width_) ? (*this)(r, c) : static_cast<T>(0);
                T v2 = (r < other.height_ && c < other.width_) ? other(r, c) : static_cast<T>(0);
                result(r, c) = safeAdd(v1, v2);
            }
        }
        return result;
    }

    Image operator*(T val) const {
        Image result(width_, height_, false);
        for (size_t i = 0; i < width_ * height_; ++i) {
            result.data_[i] = safeMul(data_[i], val);
        }
        return result;
    }

    Image operator+(T val) const {
        Image result(width_, height_, false);
        for (size_t i = 0; i < width_ * height_; ++i) {
            result.data_[i] = safeAdd(data_[i], val);
        }
        return result;
    }

    Image operator!() const {
        Image result(width_, height_, false);
        for (size_t i = 0; i < width_ * height_; ++i) {
            if constexpr (std::is_same_v<T, bool>) {
                result.data_[i] = !data_[i];
            }
            else {
                result.data_[i] = safeAdd(std::numeric_limits<T>::max(), safeMul(data_[i], -1));
            }
        }
        return result;
    }

    // Вычисление коэффициента заполнения матрицы
    double getFillFactor() const {
        if (width_ == 0 || height_ == 0) return 0.0;
        double sum = 0.0;
        for (size_t i = 0; i < width_ * height_; ++i) {
            sum += static_cast<double>(data_[i]);
        }
        double maxVal = std::is_same_v<T, bool> ? 1.0 : static_cast<double>(std::numeric_limits<T>::max());
        return sum / (static_cast<double>(width_ * height_) * maxVal);
    }

    // Дружественные функции для обеспечения коммутативности и вывода в поток
    friend Image operator*(T val, const Image& img) { return img * val; }
    friend Image operator+(T val, const Image& img) { return img + val; }

    friend std::ostream& operator<<(std::ostream& os, const Image& img) {
        for (size_t r = 0; r < img.height_; ++r) {
            for (size_t c = 0; c < img.width_; ++c) {
                if constexpr (std::is_same_v<T, char> || std::is_same_v<T, unsigned char>) {
                    os << static_cast<int>(img(r, c)) << "\t";
                }
                else if constexpr (std::is_same_v<T, bool>) {
                    os << (img(r, c) ? "1" : "0") << "\t";
                }
                else {
                    os << img(r, c) << "\t";
                }
            }
            os << "\n";
        }
        return os;
    }
};

// Внешняя утилита для растрового рисования закрашенной окружности
template <typename T>
void drawFilledCircle(Image<T>& img, int centerX, int centerY, int radius, T fillValue) {
    if (radius < 0) throw std::invalid_argument("Radius cannot be negative");

    int w = static_cast<int>(img.getWidth());
    int h = static_cast<int>(img.getHeight());

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int dx = x - centerX;
            int dy = y - centerY;
            if (dx * dx + dy * dy <= radius * radius) {
                img(static_cast<size_t>(y), static_cast<size_t>(x)) = fillValue;
            }
        }
    }
}

#endif
