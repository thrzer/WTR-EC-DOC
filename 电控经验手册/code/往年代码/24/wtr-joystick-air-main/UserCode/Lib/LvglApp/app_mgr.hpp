#pragma once

#include "page_mgr.hpp"
#include "lvgl_app.hpp"

extern PageMgr MainWindow;

void AppMgr_Init();

void AppMgr_LaunchApp(LvglApp &app, PageMgr &page_mgr, void *argument = nullptr);
