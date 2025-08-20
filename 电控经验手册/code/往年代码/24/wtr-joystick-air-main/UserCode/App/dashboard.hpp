#pragma once

#include "LvglApp/lvgl_app.hpp"
#include <string>
#include "lvgl_thread.h"
#include <map>

class Dashboard
{
public:
    lv_obj_t *dashboard_obj = nullptr;

    Dashboard(uint8_t id)
        : id_(id){};

    void Init(lv_obj_t *parent, lv_coord_t size_w, std::string title = "", std::string msg = "");

    uint8_t GetId() const
    {
        return id_;
    }

    void SetMsg(const std::string &msg);

    template <typename T>
    void SetMsgValue(T value)
    {
        LvglLock();
        msg_ = std::to_string(value);
        StepArc();
        lv_label_set_text_static(msg_label_, msg_.c_str());
        LvglUnlock();
    }

    template <typename T1, typename T2>
    void SetMsgValue(T1 value1, T2 value2)
    {
        LvglLock();
        msg_ = std::to_string(value1);
        msg_.append(",").append(std::to_string(value2));
        StepArc();
        lv_label_set_text_static(msg_label_, msg_.c_str());
        LvglUnlock();
    }

    std::string GetMsg() const
    {
        return msg_;
    }

    void SetTitle(const std::string &title)
    {
        LvglLock();
        title_ = title;
        lv_label_set_text_static(title_label_, title_.c_str());
        LvglUnlock();
    }

    std::string GetTitle() const
    {
        return title_;
    }

    ~Dashboard()
    {
        LvglLock();
        lv_obj_del(dashboard_obj);
        LvglUnlock();
    }

private:
    const uint8_t id_;
    std::string title_;
    std::string msg_;

    lv_obj_t *title_flex_;
    lv_obj_t *id_arc_;
    lv_obj_t *id_label_;
    lv_obj_t *title_label_;
    lv_obj_t *msg_label_;

    float arc_start_angle_ = 90;
    float arc_angle_       = 60;
    float arc_step_angle_  = 360.0 / 50;

    void StepArc()
    {
        arc_start_angle_ += arc_step_angle_;
        if (arc_start_angle_ > 360) arc_start_angle_ -= 360;
        lv_arc_set_angles(id_arc_, arc_start_angle_, arc_start_angle_ + arc_angle_);
    }
};

class DashboardMgr : public LvglApp
{
private:
    std::map<uint8_t, Dashboard *> dashboards;

public:
    DashboardMgr(const std::string &name)
        : LvglApp(name){};

    void Init()
    {
        lv_obj_set_flex_flow(app_main_page, LV_FLEX_FLOW_ROW_WRAP);
        lv_obj_set_flex_align(app_main_page, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
        lv_obj_set_style_pad_column(app_main_page, 0, LV_PART_MAIN);
    }

    Dashboard *NewDashboard(uint8_t id, std::string title = "", std::string msg = "")
    {
        LvglLock();
        auto iter = dashboards.find(id);

        if (iter != dashboards.end()) {
            // dashboards 中有该 id
            delete iter->second; // 删掉已经有的
            dashboards.erase(iter);
        }

        auto dashboard = new Dashboard(id);
        dashboard->Init(app_main_page, 200, title, msg);
        dashboards[id] = dashboard;
        LvglUnlock();
        return dashboard;
    }

    Dashboard *GetDashboard(uint8_t id)
    {
        LvglLock();
        auto iter = dashboards.find(id);
        LvglUnlock();
        if (iter != dashboards.end()) {
            return iter->second;
        }
        return nullptr;
    }

    void DelDashboard(uint8_t id)
    {
        LvglLock();
        auto iter = dashboards.find(id);

        if (iter != dashboards.end()) {
            // dashboards 中有该 id
            delete iter->second; // 删掉已经有的
            dashboards.erase(iter);
        }
        LvglUnlock();
    }

    ~DashboardMgr()
    {
        for (auto iter = dashboards.begin(); iter != dashboards.end(); iter++) {
            delete iter->second;
        }
    }
};

extern DashboardMgr dashboard_mgr;
