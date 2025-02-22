/*
* MIT License
 *
 * Copyright (c) 2025 Vinzenz Weist
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#include <string>
#include <lcdgfx/lcdgfx.h>
#include <robomaster/robomaster.h>

/**
 * @brief model to store display data.
 */
struct DisplayData {
    /**
     * @brief the current blaster mode.
     */
    robomaster::BlasterMode blaster_mode = robomaster::BLASTER_MODE_IR;

    /**
     * @brief the speed values for all wheels.
     */
    std::array<int16_t, 4> esc_data = { 0, 0, 0, 0 };

    /**
     * @brief the current battery value [0, 100].
     */
    uint8_t battery_percent = 0;
};

/**
 * @brief basic operations for the ssd1306 display.
 */
class Display {
    /**
     * @brief instance of the display.
     */
    DisplaySSD1306_128x64_I2C ssd1306;

    /**
     * @brief format values for print.
     * @param number the number to format.
     * @return the formatted number.
     */
    static std::string format(int number);

public:
    /**
     * @brief constructor of display.
     */
    Display(/* args */);

    /**
     * @brief destructor of display.
     */
    ~Display() = default;

    /**
     * @brief prepare display.
     */
    void prepare_display();

    /**
     * @brief update the display.
     * @param data the display data.
     */
    void update_display(DisplayData data);
};