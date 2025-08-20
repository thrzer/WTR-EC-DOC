#include "dashboard.hpp"
#include <sstream>

static bool has_inited_style_ = false;
static lv_style_t dashboard_obj_style_;
static lv_style_t msg_label_style_;

static void InitStyles()
{
    if (has_inited_style_ == false) {
        has_inited_style_ = true;

        lv_style_init(&dashboard_obj_style_);
        lv_style_set_pad_all(&dashboard_obj_style_, 4);

        lv_style_init(&msg_label_style_);
        lv_style_set_align(&msg_label_style_, LV_ALIGN_BOTTOM_RIGHT);
        lv_style_set_width(&msg_label_style_, lv_pct(100));
        lv_style_set_text_align(&msg_label_style_, LV_TEXT_ALIGN_RIGHT);
    }
}

void Dashboard::Init(lv_obj_t *parent, lv_coord_t size_w, std::string title, std::string msg)
{
    LvglLock();
    InitStyles();

    dashboard_obj = lv_obj_create(parent);
    lv_obj_clear_flag(dashboard_obj, LV_OBJ_FLAG_SCROLLABLE); // 禁止滚动
    lv_obj_set_size(dashboard_obj, size_w, 80);
    lv_obj_add_style(dashboard_obj, &dashboard_obj_style_, 0);

    title_flex_ = lv_obj_create(dashboard_obj);
    lv_obj_clear_flag(title_flex_, LV_OBJ_FLAG_SCROLLABLE); // 禁止滚动
    lv_obj_set_flex_flow(title_flex_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(title_flex_, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_width(title_flex_, lv_pct(100));
    lv_obj_set_height(title_flex_, 40);
    lv_obj_set_style_pad_all(title_flex_, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(title_flex_, 0, LV_PART_MAIN);
    lv_obj_set_style_border_width(title_flex_, 0, LV_PART_MAIN);
    lv_obj_set_align(title_flex_, LV_ALIGN_TOP_MID);
    lv_obj_set_style_pad_column(title_flex_, 5, LV_PART_MAIN);

    id_arc_ = lv_arc_create(title_flex_);
    lv_arc_set_bg_angles(id_arc_, 0, 360);
    lv_arc_set_angles(id_arc_, arc_start_angle_, arc_start_angle_ + arc_angle_);
    lv_obj_set_style_arc_width(id_arc_, 2, LV_PART_MAIN);
    lv_obj_set_style_arc_width(id_arc_, 2, LV_PART_INDICATOR);
    lv_obj_remove_style(id_arc_, NULL, LV_PART_KNOB);  // make the arc non-adjustable
    lv_obj_clear_flag(id_arc_, LV_OBJ_FLAG_CLICKABLE); // make the arc non-adjustable
    lv_obj_set_size(id_arc_, 40, 40);

    id_label_ = lv_label_create(id_arc_);
    lv_label_set_text(id_label_, std::to_string((int)id_).c_str());
    lv_obj_align_to(id_label_, id_arc_, LV_ALIGN_CENTER, 0, 0);

    title_label_ = lv_label_create(title_flex_);

    msg_label_ = lv_label_create(dashboard_obj);

    lv_obj_add_style(msg_label_, &msg_label_style_, 0);

    // lv_obj_set_width(msg_label_, lv_pct(100));
    // lv_obj_align(msg_label_, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
    lv_label_set_long_mode(msg_label_, LV_LABEL_LONG_SCROLL);

    LvglUnlock();

    SetTitle(title);
    SetMsg(msg);
}

void Dashboard::SetMsg(const std::string &msg)
{
    LvglLock();
    msg_ = msg;
    StepArc();
    lv_label_set_text_static(msg_label_, msg_.c_str());
    LvglUnlock();
}

DashboardMgr dashboard_mgr("dashboard_mgr");
