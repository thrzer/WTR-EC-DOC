#include "mavlink_thread.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "Lib/Mavlink/wtr_mavlink.h"
#include "main.h"
#include "Button/buttons.h"
#include "Button/matrix_keyboard.h"
#include "Joystick/joystick_define.hpp"
#include "Encoder/encoder_define.hpp"
#include "As69/as69_device.hpp"
#include "Led/led_define.hpp"
#include "App/dashboard.hpp"
#include "sstream"

using namespace std;

static mavlink_joystick_air_t msg_to_send_ = {};
static SemaphoreHandle_t mutex_;
static volatile bool run_control_      = false;
static volatile bool thread_run_state_ = false;

static mavlink_joystick_air_led_t msg_led_;
static TickType_t led_until_tick_ = 0;

volatile bool got_msg_dashboard_set_title_ = false;
static mavlink_joystick_air_dashboard_set_title_t msg_dashboard_set_title_;
volatile bool got_msg_dashboard_set_msg_ = false;
static mavlink_joystick_air_dashboard_set_msg_t msg_dashboard_set_msg_;
volatile bool got_msg_dashboard_del_ = false;
static mavlink_joystick_air_dashboard_del_t msg_dashboard_del_;

volatile bool got_msg_joystick_air_ = false;
static mavlink_joystick_air_t msg_joystick_air_;

static void SendMsg()
{
    msg_to_send_.buttons = 0;
    for (size_t i = 0; i < 20; i++) {
        msg_to_send_.buttons |= Buttons_Read(i + 1) << i;
    }

    msg_to_send_.switchs = 0;
    msg_to_send_.switchs |= Buttons_Read(Switch_L);
    msg_to_send_.switchs |= Buttons_Read(Switch_R) << 1;

    Joystick::Point_t pos;
    pos                       = JoystickL.Pos();
    msg_to_send_.joystickL[0] = pos.x;
    msg_to_send_.joystickL[1] = pos.y;

    pos                       = JoystickR.Pos();
    msg_to_send_.joystickR[0] = pos.x;
    msg_to_send_.joystickR[1] = pos.y;

    msg_to_send_.knobs[0] = KnobEncoderL.Count();
    msg_to_send_.knobs[1] = KnobEncoderR.Count();

    mavlink_msg_joystick_air_send_struct(MAVLINK_COMM_0, &msg_to_send_);
}

static void ProcessLed()
{
    static bool is_led_on_ = false;
    if (xTaskGetTickCount() < led_until_tick_) {
        if (is_led_on_ == false) {
            is_led_on_ = true;
            KeyboardLed.Power(true);
        }
        KeyboardLed.SetColor(msg_led_.r, msg_led_.g, msg_led_.b, msg_led_.lightness);
    } else {
        if (is_led_on_ == true) {
            is_led_on_ = false;
            KeyboardLed.Power(false);
        }
    }
}

static void ProcessDashboard()
{
    if (got_msg_dashboard_set_msg_) {
        got_msg_dashboard_set_msg_           = false;
        auto &msg                            = msg_dashboard_set_msg_;
        msg.message[sizeof(msg.message) - 1] = '\0'; // 字符串安全
        auto dashboard                       = dashboard_mgr.GetDashboard(msg.id);
        if (dashboard == nullptr) {
            // 如果不存在就创建一个
            dashboard = dashboard_mgr.NewDashboard(msg.id, "", msg.message);
        } else {
            dashboard->SetMsg(msg.message);
        }
    }

    if (got_msg_dashboard_set_title_) {
        got_msg_dashboard_set_title_     = false;
        auto &msg                        = msg_dashboard_set_title_;
        msg.title[sizeof(msg.title) - 1] = '\0'; // 字符串安全
        auto dashboard                   = dashboard_mgr.GetDashboard(msg.id);
        if (dashboard == nullptr) {
            // 如果不存在就创建一个
            dashboard = dashboard_mgr.NewDashboard(msg.id, msg.title);
        } else {
            dashboard->SetTitle(msg.title);
        }
    }

    if (got_msg_dashboard_del_) {
        got_msg_dashboard_del_ = false;
        dashboard_mgr.DelDashboard(msg_dashboard_del_.id);
    }
}

static void ProcessMsgJoystickAir()
{
    if (got_msg_joystick_air_) {
        got_msg_joystick_air_ = false;
        auto &msg             = msg_joystick_air_;
        Dashboard *dashboard;

        dashboard = dashboard_mgr.GetDashboard(250);
        if (dashboard == nullptr) {
            // 如果不存在就创建一个
            dashboard = dashboard_mgr.NewDashboard(250, "JoystickL");
        }
        dashboard->SetMsgValue(msg.joystickL[0], msg.joystickL[1]);

        dashboard = dashboard_mgr.GetDashboard(251);
        if (dashboard == nullptr) {
            // 如果不存在就创建一个
            dashboard = dashboard_mgr.NewDashboard(251, "JoystickR");
        }
        dashboard->SetMsgValue(msg.joystickR[0], msg.joystickR[1]);

        dashboard = dashboard_mgr.GetDashboard(252);
        if (dashboard == nullptr) {
            // 如果不存在就创建一个
            dashboard = dashboard_mgr.NewDashboard(252, "Encoder");
        }
        dashboard->SetMsgValue(msg.knobs[0], msg.knobs[1]);

        dashboard = dashboard_mgr.GetDashboard(253);
        if (dashboard == nullptr) {
            // 如果不存在就创建一个
            dashboard = dashboard_mgr.NewDashboard(253, "Btn");
        }
        stringstream sstr;
        for (size_t i = 0; i <= 19; i++) {
            sstr << ((msg.buttons >> i) & 1);
        }
        dashboard->SetMsg(sstr.str());

        dashboard = dashboard_mgr.GetDashboard(254);
        if (dashboard == nullptr) {
            // 如果不存在就创建一个
            dashboard = dashboard_mgr.NewDashboard(254, "Switchs");
        }

        sstr.str("");
        for (size_t i = 0; i <= 1; i++) {
            sstr << ((msg.switchs >> i) & 1);
        }
        dashboard->SetMsg(sstr.str());
    }
}

void MavlinkThreadEntry(void *argument)
{
    (void)argument;
    thread_run_state_ = true;

    wtrMavlink_StartReceiveIT(MAVLINK_COMM_0);

    while (run_control_ == true) {
        Buttons_Scan();
        SendMsg();
        ProcessLed();
        ProcessMsgJoystickAir();
        ProcessDashboard();

        vTaskDelay(20);
    }

    wtrMavlink_StopReceiveIT(MAVLINK_COMM_0);

    thread_run_state_ = false;
    vTaskDelete(nullptr);
}

void InitMavlinkThread()
{
    mutex_ = xSemaphoreCreateMutex();
    wtrMavlink_BindChannel(&huart1, MAVLINK_COMM_0);
}

void StartMavlinkThread()
{
    xSemaphoreTake(mutex_, portMAX_DELAY);
    if (run_control_ == false) {
        run_control_ = true;
        xTaskCreate(MavlinkThreadEntry, "MavlinkThread", 1024, nullptr, 3, nullptr);
    }
    xSemaphoreGive(mutex_);
}

void StopMavlinkThread()
{
    xSemaphoreTake(mutex_, portMAX_DELAY);
    run_control_ = false;

    // 等待结束
    while (thread_run_state_ == true) {
        vTaskDelay(1);
    }

    xSemaphoreGive(mutex_);
}

/**
 * @brief 接收到完整消息且校验通过后会调用这个函数。在这个函数里调用解码函数就可以向结构体写入收到的消息
 *
 * @param msg 接收到的消息
 * @return
 */
void wtrMavlink_MsgRxCpltCallback(mavlink_message_t *msg)
{
    switch (msg->msgid) {
        case MAVLINK_MSG_ID_JOYSTICK_AIR_LED:
            mavlink_msg_joystick_air_led_decode(msg, &msg_led_);
            led_until_tick_ = xTaskGetTickCountFromISR() + msg_led_.duration;
            break;
        case MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_TITLE:
            mavlink_msg_joystick_air_dashboard_set_title_decode(msg, &msg_dashboard_set_title_);
            got_msg_dashboard_set_title_ = true;
            break;
        case MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_SET_MSG:
            mavlink_msg_joystick_air_dashboard_set_msg_decode(msg, &msg_dashboard_set_msg_);
            got_msg_dashboard_set_msg_ = true;
            break;
        case MAVLINK_MSG_ID_JOYSTICK_AIR_DASHBOARD_DEL:
            mavlink_msg_joystick_air_dashboard_del_decode(msg, &msg_dashboard_del_);
            got_msg_dashboard_del_ = true;
            break;
        case MAVLINK_MSG_ID_JOYSTICK_AIR:
            mavlink_msg_joystick_air_decode(msg, &msg_joystick_air_);
            got_msg_joystick_air_ = true;
            break;
        // ......
        default:
            break;
    }
}