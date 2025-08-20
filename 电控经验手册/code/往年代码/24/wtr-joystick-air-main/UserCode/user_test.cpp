#include "user_test.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "driver_mpu9250_basic.h"
#include "adc.h"
#include "lvgl_thread.h"
#include "lvgl/lvgl.h"
#include <sstream>
#include "HighPrecisionTime/high_precision_time.h"
#include "Mavlink/wtr_mavlink.h"
#include <iomanip>
#include "LvglApp/app_mgr.hpp"
#include "test_app.hpp"
#include "App/dashboard.hpp"
#include "Joystick/joystick_define.hpp"
#include "Encoder/encoder_define.hpp"
#include "as69_config.hpp"
#include "TouchScreen/GT911/gt911_define.hpp"
#include "Button/buttons.h"

#define Led_Pin  GPIO_PIN_1
#define Led_Port GPIOA

using namespace std;

static TickType_t last_3_points_time = 0;

void TestThreadEntry(void *argument)
{
    (void)argument;

    // auto joystickl_dashboard = dashboard_mgr.NewDashboard(250, "JoystickL");
    // auto joystickr_dashboard = dashboard_mgr.NewDashboard(251, "JoystickR");
    // auto encoder_dashboard   = dashboard_mgr.NewDashboard(252, "Encoder");
    // auto button_dashboard    = dashboard_mgr.NewDashboard(253, "Buttons(0-10 bit)");
    // auto button2_dashboard   = dashboard_mgr.NewDashboard(254, "Buttons(11-21 bit)");

    // stringstream sstr;
    // sstr.precision(4);
    // sstr.setf(std::ios::fixed);

    uint32_t PreviousWakeTime = xTaskGetTickCount();

    // extern volatile uint32_t MavTotalBytesSent;
    // extern volatile uint32_t MavTotalBytesGot;

    while (true) {
        // time_dashboard->SetMsg(xTaskGetTickCount() / 1000.0);
        // sstr.str("");
        // sstr << JoystickL.Pos().x << "," << JoystickL.Pos().y;
        // joystickl_dashboard->SetMsg(sstr.str());
        // sstr.str("");
        // sstr << JoystickR.Pos().x << "," << JoystickR.Pos().y;
        // joystickr_dashboard->SetMsg(sstr.str());
        // sstr.str("");
        // sstr << KnobEncoderL.Count() << "," << KnobEncoderR.Count();
        // encoder_dashboard->SetMsg(sstr.str());

        // sstr.str("");
        // for (size_t i = 1; i <= 11; i++) {
        //     sstr << (int)Buttons_Read(i);
        // }
        // button_dashboard->SetMsg(sstr.str());

        // sstr.str("");
        // for (size_t i = 12; i <= 22; i++) {
        //     sstr << (int)Buttons_Read(i);
        // }
        // button2_dashboard->SetMsg(sstr.str());

        if (TouchScreen.NumberOfTouchPoint() == 3) {
            if (xTaskGetTickCount() - last_3_points_time > 500) {
                MainWindow.SwitchToPage(0);
            }
            last_3_points_time = xTaskGetTickCount();
        }

        vTaskDelayUntil(&PreviousWakeTime, 100);
    }
}
