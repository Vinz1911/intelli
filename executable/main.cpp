#include <sense/dualsense.h>
#include <sense/constants.h>
#include <robomaster/robomaster.h>
#include <robomaster/definitions.h>

#include "intelli/mapper.h"
#include "intelli/display.h"

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
    auto display = Display();
    auto robomaster = robomaster::RoboMaster();
    auto dualsense = sense::DualSense();
    auto blaster_mode = robomaster::BLASTER_MODE_IR;
    auto display_data = DisplayData();

    if (!robomaster.init() || !dualsense.set_open()) { std::printf("[Intelli v2.0]: initialization failed\n"); return -1; }
    display.prepare_display();
    std::thread(display_thread, std::ref(robomaster), std::ref(display), std::ref(display_data)).detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    robomaster.set_chassis_mode(robomaster::CHASSIS_MODE_ENABLE);
    robomaster.set_led(robomaster::LED_MODE_STATIC, robomaster::LED_MASK_ALL, 128, 0, 255);

    printf("[Intelli v2.0]: successfully started...\n");
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
        if (state.esc.has_data) { display_data.esc_data = state.esc.speed; }
        if (state.battery.has_data) { display_data.battery_percent = state.battery.percent; }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(25)); robomaster.set_chassis_mode(robomaster::CHASSIS_MODE_DISABLE); return 0;
}