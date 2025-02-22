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

#include <sense/dualsense.h>
#include <sense/constants.h>
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
    while (robomaster.is_running()) {
        display.update_display(data);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

int main() {
    auto display = Display(); auto robomaster = robomaster::RoboMaster(); auto dualsense = sense::DualSense();
    auto blaster_mode = robomaster::BLASTER_MODE_IR; auto display_data = DisplayData(); std::printf("%s\n", prompt);

    if (!robomaster.init()) { std::printf("[Intelli v2.0]: initialization failed!\n"); return -1; }
    display.prepare_display(); std::thread(display_thread, std::ref(robomaster), std::ref(display), std::ref(display_data)).detach();
    printf("[Intelli v2.0]: successfully started!\n");

    while (robomaster.is_running()) {
        while (!dualsense.is_active()) {
            printf("[Intelli v2.0]: wait for remote to pair...\n");
            robomaster.set_chassis_mode(robomaster::CHASSIS_MODE_DISABLE);
            robomaster.set_led(robomaster::LED_MODE_BREATHE, robomaster::LED_MASK_ALL, 128, 0, 255, 500, 500);
            if (dualsense.set_open()) { break; }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }

        printf("[Intelli v2.0]: system has started!\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        robomaster.set_chassis_mode(robomaster::CHASSIS_MODE_ENABLE);
        robomaster.set_led(robomaster::LED_MODE_STATIC, robomaster::LED_MASK_ALL, 128, 0, 255);
        dualsense.set_led(32, 0, 255);

        while (dualsense.is_active() && robomaster.is_running()) {
            auto axis = dualsense.get_axis(); auto buttons = dualsense.get_buttons();
            const auto throttle = Mapper::map_trigger(axis[sense::AXIS_RIGHT_TRIGGER]);
            const auto reversed = Mapper::map_trigger(axis[sense::AXIS_LEFT_TRIGGER]);

            const auto steering = Mapper::map_analog(axis[sense::AXIS_LEFT_THUMB_X], true);
            const auto gimbal_z = Mapper::map_analog(axis[sense::AXIS_RIGHT_THUMB_X]);
            const auto gimbal_y = Mapper::map_analog(axis[sense::AXIS_RIGHT_THUMB_Y]);

            single_completion(buttons[sense::BUTTON_SHARE], [&robomaster]{ robomaster.set_gimbal_recenter(200, 200); });
            single_completion(buttons[sense::BUTTON_OPTIONS], [&robomaster]{ robomaster.set_gimbal_velocity(0, 0); });
            single_completion(buttons[sense::BUTTON_SHOULDER_RIGHT], [&blaster_mode, &robomaster] { robomaster.set_blaster(blaster_mode); });
            single_completion(buttons[sense::BUTTON_TRIANGLE], [&blaster_mode] { blaster_mode = blaster_mode == robomaster::BLASTER_MODE_IR ? robomaster::BLASTER_MODE_GEL : robomaster::BLASTER_MODE_IR; });
            if (buttons[sense::BUTTON_SHOULDER_LEFT]) { robomaster.set_blaster(blaster_mode); }

            const auto speed = reversed > 0 ? static_cast<int16_t>(-reversed * 1.0) : static_cast<int16_t>(throttle * 5.0);
            robomaster.set_chassis_rpm(
                steering > 0.0f ? static_cast<int16_t>(speed * steering) : speed,
                steering > 0.0f ? static_cast<int16_t>(speed) : steering < 0.0f ? static_cast<int16_t>(speed * -steering) : static_cast<int16_t>(speed),
                steering > 0.0f ? static_cast<int16_t>(speed) : steering < 0.0f ? static_cast<int16_t>(speed * -steering) : static_cast<int16_t>(speed),
                steering > 0.0f ? static_cast<int16_t>(speed * steering) : speed
            );
            robomaster.set_gimbal_degree(static_cast<int16_t>(100 * -gimbal_y), static_cast<int16_t>(200 * gimbal_z));

            const auto state = robomaster.get_state(); display_data.blaster_mode = blaster_mode;
            if (state.is_active) { display_data.esc_data = state.esc.speed; display_data.battery_percent = state.battery.percent; }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    if (robomaster.is_running()) { robomaster.set_chassis_mode(robomaster::CHASSIS_MODE_DISABLE); }
    printf("[Intelli v2.0]: system has exited!\n"); return 0;
}