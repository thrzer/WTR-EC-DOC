#pragma once

#include <string>
#include "lvgl/lvgl.h"

class LvglApp
{
public:
    enum class Type {
        Normal,
        NoGUI // 暂时不支持
    };

    struct AppProperty {
        // Lvgl
        const std::string name; // 应用的名字
        const Type type;

        // Common
        void *argument; // 启动应用的参数

        // FreeRTOS
        bool use_thread      = false; // 开启这个选项后，会在一个线程中执行 Run()
        uint16_t stack_depth = 512;   // 栈大小，单位：word (4 bytes)
        uint16_t priority    = 3;     // FreeRTOS 优先级，正常优先级是3，值越高优先级越高。范围：[0, configMAX_PRIORITIES]
    };

public:
    AppProperty app_property;
    lv_obj_t *app_main_page = nullptr;

    LvglApp(AppProperty app_property)
        : app_property(app_property){};

    LvglApp(const std::string &name, const Type type = Type::Normal, void *argument = nullptr, bool use_thread = false, uint16_t stack_depth = 512, uint16_t priority = 3)
        : app_property{name, type, argument, use_thread, stack_depth, priority} {};

    ~LvglApp(){};

    virtual void Init(){};

    virtual int Run()
    {
        return 0;
    }

    virtual void OnLaunch(){};
    virtual void OnExit(){};
    virtual void OnSwitchToBackground(){};
    virtual void OnSwitchToForeground(){};

    uint16_t GetStackDepth() const
    {
        return app_property.stack_depth;
    }

    uint16_t GetPriority() const
    {
        return app_property.priority;
    }
};
