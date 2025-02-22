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

#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <lcdgfx/lcdgfx.h>
#include <robomaster/robomaster.h>

struct DisplayData {
    robomaster::BlasterMode blaster_mode = robomaster::BLASTER_MODE_IR;
    std::array<int16_t, 4> esc_data = { 0, 0, 0, 0 };
    uint8_t battery_percent = 0;
};

class Display {
    DisplaySSD1306_128x64_I2C ssd1306;
    static std::string format(int number);

public:
    Display();
    ~Display() = default;

    void prepare_display();
    void update_display(DisplayData data);
};

#endif //DISPLAY_H
