//
// Created by Vinzenz Weist on 15.02.25.
//

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
