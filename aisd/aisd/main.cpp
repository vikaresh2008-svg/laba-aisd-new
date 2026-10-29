#include <iostream>
#include <exception>
#include "Image.hpp"

int main() {
    try {
        std::cout << "=== 1. Demonstrating constructors and operator () ===" << std::endl;
        Image<short> img1(5, 5, true);
        std::cout << "Random Image (short):\n" << img1;

        Image<short> img2(3, 3, false);
        img2(0, 0) = 10; img2(1, 1) = 20;
        std::cout << "\nImage 2 (3x3, zeros except diagonal):\n" << img2;

        std::cout << "\n=== 2. Demonstrating equality operators ===" << std::endl;
        Image<short> img3 = img2;
        std::cout << "img2 == img3: " << (img2 == img3 ? "true" : "false") << std::endl;
        std::cout << "img1 == img2: " << (img1 == img2 ? "true" : "false") << std::endl;
        std::cout << "img1 != img2: " << (img1 != img2 ? "true" : "false") << std::endl;

        std::cout << "\n=== 3. Demonstrating addition and multiplication of images ===" << std::endl;
        Image<short> sumImg = img1 + img2;
        std::cout << "img1 + img2 (different sizes, result 5x5):\n" << sumImg;

        Image<short> mulImg = img1 * img2;
        std::cout << "img1 * img2:\n" << mulImg;

        std::cout << "\n=== 4. Demonstrating multiplication/addition by constant ===" << std::endl;
        Image<short> constAdd = img1 + static_cast<short>(10);
        std::cout << "img1 + 10:\n" << constAdd;

        Image<short> constMul = 2 * img1;
        std::cout << "2 * img1:\n" << constMul;

        std::cout << "\n=== 5. Demonstrating operator ! (inversion) ===" << std::endl;
        Image<short> inverted = !img1;
        std::cout << "!img1:\n" << inverted;

        std::cout << "\n=== 6. Calculating fill factor ===" << std::endl;
        std::cout << "Fill factor img1: " << img1.getFillFactor() << std::endl;

        std::cout << "\n=== 7. Task 3: Draw a filled circle ===" << std::endl;
        Image<short> canvas(10, 10, false);
        int cx = 4, cy = 5, r = 3;
        short color = 255;
        drawFilledCircle(canvas, cx, cy, r, color);
        std::cout << "Canvas with filled circle (center=4,5, r=3, color=255):\n" << canvas;

        std::cout << "\n=== 8. Testing exceptions (out of bounds) ===" << std::endl;
        try {
            std::cout << img1(100, 100) << std::endl;
        }
        catch (const std::out_of_range& e) {
            std::cout << "Caught exception: " << e.what() << std::endl;
        }

        std::cout << "\n=== 9. Demonstration with bool type ===" << std::endl;
        Image<bool> boolImg(3, 3, true);
        std::cout << "Bool Image (random):\n" << boolImg;
        std::cout << "Bool Image ! (inverted):\n" << !boolImg;

    }
    catch (const std::exception& e) {
        std::cout << "Fatal error: " << e.what() << std::endl;
    }
    return 0;
}