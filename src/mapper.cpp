//
// Created by Vinzenz Weist on 07.01.25.
//

#include <string>
#include <lcdgfx/lcdgfx.h>
#include <bits/stl_algo.h>
#include "intelli/mapper.h"

#include <robomaster/data.h>

int8_t Mapper::map_trigger(const int input) {
    const double converted = (input + 32767.0) * 100.0 / 65534.0;
    return static_cast<int8_t>(std::clamp(converted, 0.0, 100.0));
}

double Mapper::map_analog(const int input, const bool inverted) {
    constexpr int min_input = -32767, max_input = 32767, offset = 5000; if (input < min_input || input > max_input) { return 0; }
    if (input < -offset) { return inverted ? -0.25 + (input - min_input) * -0.75 / (-offset - min_input) : -1.0 + (input - min_input) * 0.75 / (-offset - min_input); }
    if (input > offset) { return inverted ? 1.0 - (input - offset) * 0.75 / (max_input - offset) : 0.25 + (input - offset) * 0.75 / (max_input - offset); } return 0;
}

void Mapper::update_display(DisplaySSD1306_128x64_I2C &display, const std::string& blaster, const int8_t progress, const robomaster::StateBattery &battery) {
    display.printFixed(8, 12, ("Blaster: " + blaster).c_str());
    if (battery.has_data) { char buffer_battery[16]; std::snprintf(buffer_battery, sizeof(buffer_battery), "Battery: %3d%%", battery.percent); display.printFixed(8, 20, buffer_battery); }
    char buffer_throttle[16]; std::snprintf(buffer_throttle, sizeof(buffer_throttle), "Throttle: %3d%%", progress); display.printFixed(8, 28, buffer_throttle);
}