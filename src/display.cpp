//
// Created by Vinzenz Weist on 15.02.25.
//

#include "intelli/display.h"

Display::Display(): ssd1306(DisplaySSD1306_128x64_I2C(-1, {-1, 0x3D, -1, -1, 400000})) { }

std::string Display::format(const int number) {
    std::string str = std::to_string(number);
    while (str.length() < 4) { str += " "; } return str;
}

void Display::prepare_display() {
    ssd1306.setFixedFont(ssd1306xled_font6x8);
    ssd1306.begin();
    ssd1306.clear();

    ssd1306.drawWindow(0, 0, 0, 0, "Intelli v2.0", true);
    ssd1306.printFixed(8, 8, "Turret");
    ssd1306.printFixed(8, 16, "Battery[%]");
    ssd1306.printFixed(8, 38, "Speed Control[RPM]");
    ssd1306.drawHLine(45, 11, 75);
    ssd1306.drawVLine(76, 12, 18);
    ssd1306.drawHLine(77, 19, 84);
    ssd1306.drawVLine(10, 24, 26);
    ssd1306.drawHLine(11, 27, 19);
    ssd1306.drawVLine(10, 40, 50);
    ssd1306.drawHLine(11, 43, 19);
    ssd1306.drawHLine(11, 51, 19);
    ssd1306.drawVLine(64, 40, 50);
    ssd1306.drawHLine(65, 43, 73);
    ssd1306.drawHLine(65, 51, 73);
}

void Display::update_display(const DisplayData data) {
    const auto battery = format(data.battery_percent);
    const auto fr_rpm = "FR:" + format(data.esc_data[0]);
    const auto fl_rpm = "FL:" + format(-data.esc_data[1]);
    const auto rl_rpm = "RL:" + format(-data.esc_data[2]);
    const auto rr_rpm = "RR:" + format(data.esc_data[3]);

    ssd1306.printFixed(85, 16, data.blaster_mode == robomaster::BLASTER_MODE_IR ? "[IFR]" : "[GEL]");
    ssd1306.printFixed(20, 30, battery.c_str());
    ssd1306.printFixed(20, 44, fl_rpm.c_str());
    ssd1306.printFixed(20, 52, fr_rpm.c_str());
    ssd1306.printFixed(74, 44, rl_rpm.c_str());
    ssd1306.printFixed(74, 52, rr_rpm.c_str());
}