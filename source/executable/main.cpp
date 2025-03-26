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

#include <xcrsf/crossfire.h>
#include <robomaster/robomaster.h>
#include <robomaster/definitions.h>

#include "intelli/mapper.h"
#include "intelli/display.h"

auto prompt = R"(
       ___       _       _ _ _         ____    ___
      |_ _|_ __ | |_ ___| | (_) __   _|___ \  / _ \
       | || '_ \| __/ _ \ | | | \ \ / / __) || | | |
       | || | | | ||  __/ | | |  \ V / / __/ | |_| |
      |___|_| |_|\__\___|_|_|_|   \_/ |_____(_)___/

+--------------------------------------------------------+
| Intelli v2.0 for RoboMaster S1, made for Raspberry Pi. |
| Build on top of `robomaster` library. SSD1306 support. |
| https://github.com/Vinz1911/robomaster                 |
+--------------------------------------------------------+
)";

void single_completion(bool condition, auto&& completion) {
    static bool is_called = false; is_called = condition && (is_called || (completion(), true));
}

void display_thread(const robomaster::RoboMaster& robomaster, Display& display, const DisplayData& data) {
    while (robomaster.is_running()) { display.update_display(data); std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
}

int main() {
    auto display = Display(); auto robomaster = robomaster::RoboMaster(); auto crossfire = crossfire::XCrossfire("/dev/ttyAMA10");
    auto blaster_mode = robomaster::BLASTER_MODE_IR; auto display_data = DisplayData(); std::printf("%s\n", prompt);

    if (!robomaster.init() || !crossfire.open_port()) { std::printf("[Intelli v2.0]: initialization failed!\n"); return -1; }
    display.prepare_display(); std::thread(display_thread, std::ref(robomaster), std::ref(display), std::ref(display_data)).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    robomaster.set_chassis_mode(robomaster::CHASSIS_MODE_ENABLE);
    robomaster.set_led_mode(robomaster::LED_MODE_STATIC, robomaster::LED_MASK_ALL, 128, 0, 255);

    printf("[Intelli v2.0]: robomaster ready!\n");
    while (crossfire.is_paired() && robomaster.is_running()) {
        auto channels = crossfire.get_channels();

        const auto throttle = Mapper::map_range(channels[1], 992, 1811, 0, 500);
        const auto reversed = Mapper::map_range(channels[1], 174, 992, 250, 0);
        const auto steering_left = Mapper::map_range(channels[3], 174, 992, 0.20, 1.0, 1.0);
        const auto steering_right = Mapper::map_range(channels[3], 992, 1811, 1.0, 0.20, 1.0);

        const auto gimbal_yaw = Mapper::map_range(channels[0], 174, 1811, -300, 300);
        const auto gimbal_pitch = Mapper::map_range(channels[2], 174, 1811, -150, 150);

        single_completion(channels[10] == 1792, [&robomaster]{ robomaster.set_gimbal_recenter(200, 200); });
        single_completion(channels[11] == 1792, [&robomaster]{ robomaster.set_gimbal_velocity(0, 0); });
        single_completion(channels[9] == 1792, [&blaster_mode, &robomaster] { robomaster.set_blaster_mode(blaster_mode); });

        blaster_mode = channels[4] == 191 ? robomaster::BLASTER_MODE_IR : robomaster::BLASTER_MODE_GEL;
        if (channels[8] == 1792) { robomaster.set_blaster_mode(blaster_mode); }

        const auto speed = reversed > 0 ? static_cast<int16_t>(-reversed) : static_cast<int16_t>(throttle);
        robomaster.set_chassis_rpm(
            static_cast<int16_t>(speed * steering_right),
            static_cast<int16_t>(speed * steering_left),
            static_cast<int16_t>(speed * steering_left),
            static_cast<int16_t>(speed * steering_right)
        );
        robomaster.set_gimbal_degree(static_cast<int16_t>(gimbal_pitch), static_cast<int16_t>(gimbal_yaw));

        const auto state = robomaster.get_state(); display_data.blaster_mode = blaster_mode;
        if (state.is_active) { display_data.esc_data = state.esc.speed; display_data.battery_percent = state.battery.percent; }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (robomaster.is_running()) { robomaster.set_chassis_rpm(0, 0, 0, 0); }
    printf("[Intelli v2.0]: system stopped!\n"); return 0;
}