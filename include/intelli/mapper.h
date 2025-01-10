//
// Created by Vinzenz Weist on 07.01.25.
//

#ifndef MAPPER_H
#define MAPPER_H

#include <cstdint>
#include <robomaster/data.h>

class Mapper {
public:
    /**
     * Convert an input value from a pre-defined range to a new one.
     * @param input takes input from -32767...32767
     * @return converted range from 0...100
     */
    static int8_t map_trigger(int input);

    /**
     * Convert an input value from a pre-defined range to a new one.
     * @param input takes input from -32767...32767
     * @param inverted invert the range
     * @return converted range from -0.25...-1.0 and from 1.0...0.25
     */
    static double map_analog(int input, bool inverted = false);

    /**
     * Update display interface
     * @param display reference to SSD1306 display object
     * @param blaster blaster type
     * @param progress progress from 0...100
     * @param battery the battery state from the RoboMaster
     */
    static void update_display(DisplaySSD1306_128x64_I2C &display, const std::string& blaster, int8_t progress, const robomaster::StateBattery &battery);
};

#endif // MAPPER_H
