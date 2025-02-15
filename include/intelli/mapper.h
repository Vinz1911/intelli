//
// Created by Vinzenz Weist on 07.01.25.
//

#ifndef MAPPER_H
#define MAPPER_H

#include <cstdint>

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
};

#endif // MAPPER_H
