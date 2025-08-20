#pragma once

#include "LvglApp/lvgl_app.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "lvgl_thread.h"
#include "dashboard.hpp"

class TestApp : public LvglApp
{
private:
    /* data */
public:
    using LvglApp::LvglApp; // 使用基类的构造函数

    Dashboard *dashboard = nullptr;

    virtual int Run() override
    {
        double value = 0;
        while (true) {
            value += 0.1;
            dashboard->SetMsgValue(value);
            vTaskDelay(100);
        }
    }

    void Init() override
    {
        app_property.use_thread = true;
        auto button             = lv_btn_create(app_main_page);
        lv_obj_center(button);
        lv_obj_set_size(button, 80, 50);
        if (app_property.argument != nullptr) {
            auto label = lv_label_create(button);
            lv_label_set_text(label, (const char *)app_property.argument);
            lv_obj_center(label);
        }
        dashboard = new Dashboard(2);
        dashboard->Init(app_main_page, 200, "This is long title", "This is long message");
    }
    ~TestApp()
    {
        if (dashboard != nullptr)
            delete dashboard;
    }
};

extern TestApp test_app;
extern TestApp test_app2;
