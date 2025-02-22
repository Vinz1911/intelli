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

#include <string>
#include <bits/stl_algo.h>
#include "intelli/mapper.h"

int8_t Mapper::map_trigger(const int input) {
    const double converted = (input + 32767.0) * 100.0 / 65534.0;
    return static_cast<int8_t>(std::clamp(converted, 0.0, 100.0));
}

double Mapper::map_analog(const int input, const bool inverted) {
    constexpr int min_input = -32767, max_input = 32767, offset = 5000; if (input < min_input || input > max_input) { return 0; }
    if (input < -offset) { return inverted ? -0.25 + (input - min_input) * -0.75 / (-offset - min_input) : -1.0 + (input - min_input) * 0.75 / (-offset - min_input); }
    if (input > offset) { return inverted ? 1.0 - (input - offset) * 0.75 / (max_input - offset) : 0.25 + (input - offset) * 0.75 / (max_input - offset); } return 0;
}