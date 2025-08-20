#include "app_mgr.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "lvgl_thread.h"

// PageMgr left_page;
// PageMgr right_page;
PageMgr MainWindow;

static void AppRunnerEntry(void *argument)
{
    auto app = (LvglApp *)argument;

    app->Run();

    vTaskDelete(nullptr);
}

void AppMgr_Init()
{
    LvglLock();
    MainWindow.Init(lv_scr_act());
    MainWindow.SetSize(lv_pct(100), lv_pct(94));
    MainWindow.SetAlign(LV_ALIGN_BOTTOM_MID);

    // left_page.Init(lv_scr_act());
    // left_page.SetSize(lv_pct(50), lv_pct(100));
    // left_page.SetAlign(LV_ALIGN_LEFT_MID);

    // right_page.Init(lv_scr_act());
    // right_page.SetSize(lv_pct(50), lv_pct(100));
    // right_page.SetAlign(LV_ALIGN_RIGHT_MID);
    LvglUnlock();
}

void AppMgr_LaunchApp(LvglApp &app, PageMgr &page_mgr, void *argument)
{
    LvglLock();
    app.app_main_page = page_mgr.NewPage();
    LvglUnlock();
    app.app_property.argument = argument;
    app.Init();
    app.OnLaunch();
    if (app.app_property.use_thread) {
        xTaskCreate(AppRunnerEntry,
                    app.app_property.name.c_str(),
                    app.app_property.stack_depth,
                    &app,
                    app.app_property.priority,
                    nullptr);
    }
}
