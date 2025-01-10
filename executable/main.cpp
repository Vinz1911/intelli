#include <lcdgfx/lcdgfx.h>
#include <sense/dualsense.h>
#include <sense/constants.h>
#include <robomaster/robomaster.h>
#include <robomaster/definitions.h>

#include "intelli/mapper.h"

void single_completion(bool condition, auto&& completion) {
    static bool is_called = false; is_called = condition && (is_called || (completion(), true));
}

int main() {
    auto display = DisplaySSD1306_128x64_I2C(-1, { -1, 0x3D, -1, -1, 1000000 });
    auto robomaster = robomaster::RoboMaster();
    auto dualsense = sense::DualSense();
    auto blaster = robomaster::INFRARED;

    display.setFixedFont(ssd1306xled_font6x8); display.begin(); display.clear(); display.drawWindow(0, 0, 0, 0, "RoboPi v2.0", true);
    if (!robomaster.init() || !dualsense.set_open()) { std::printf("[Error]: initialization failed\n"); return -1; }
    robomaster.set_torque(true); robomaster.set_led(robomaster::STATIC, robomaster::LED_MASK_ALL, 128, 0, 255);

    while (dualsense.is_active() && robomaster.is_running()) {
        auto axis = dualsense.get_axis(); auto buttons = dualsense.get_buttons();
        const auto throttle = Mapper::map_trigger(axis[sense::AXIS_RIGHT_TRIGGER]);
        const auto reversed = Mapper::map_trigger(axis[sense::AXIS_LEFT_TRIGGER]);

        const auto steering = Mapper::map_analog(axis[sense::AXIS_LEFT_THUMB_X], true);
        const auto gimbal_z = Mapper::map_analog(axis[sense::AXIS_RIGHT_THUMB_X]);
        const auto gimbal_y = Mapper::map_analog(axis[sense::AXIS_RIGHT_THUMB_Y]);

        single_completion(buttons[sense::BUTTON_SHOULDER_RIGHT], [&blaster, &robomaster] { robomaster.set_blaster(blaster); });
        single_completion(buttons[sense::BUTTON_TRIANGLE], [&blaster] { blaster = blaster == robomaster::INFRARED ? robomaster::GELBEADS : robomaster::INFRARED; });

        const auto speed = reversed > 0 ? static_cast<int16_t>(-reversed * 1.0) : static_cast<int16_t>(throttle * 5.0);
        robomaster.set_wheel_rpm(
            steering > 0.0f ? static_cast<int16_t>(speed * steering) : speed,
            steering > 0.0f ? static_cast<int16_t>(speed) : steering < 0.0f ? static_cast<int16_t>(speed * -steering) : static_cast<int16_t>(speed),
            steering > 0.0f ? static_cast<int16_t>(speed) : steering < 0.0f ? static_cast<int16_t>(speed * -steering) : static_cast<int16_t>(speed),
            steering > 0.0f ? static_cast<int16_t>(speed * steering) : speed
        );
        robomaster.set_gimbal(static_cast<int16_t>(100 * -gimbal_y), static_cast<int16_t>(200 * gimbal_z));

        const auto state = robomaster.get_state(); const auto info = dualsense.get_device_info();
        Mapper::update_display(display, blaster == robomaster::GELBEADS ? "GEL" : "IFR", throttle, state.battery);
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
    }
    robomaster.set_brake(); std::this_thread::sleep_for(std::chrono::milliseconds(2000)); robomaster.set_torque(false); return 0;
}