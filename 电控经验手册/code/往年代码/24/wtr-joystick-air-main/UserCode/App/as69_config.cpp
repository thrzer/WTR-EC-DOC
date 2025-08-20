#include "as69_config.hpp"
#include "As69/as69_device.hpp"
#include "lvgl_thread.h"
#include "MavlinkThread/mavlink_thread.hpp"

static bool has_inited_style_ = false;
static lv_style_t btn_style_;
static lv_style_t btn_inactive_style_;
static lv_style_t setting_tab_style_;

void As69Config::OnSwUpdated(bool is_sw_checked, bool apply_modify)
{
    lv_label_set_text_static(sw_cmd_label_, is_sw_checked ? "As69\nWork\nMode" : "As69\nConfig\nMode");
    lv_obj_align_to(sw_cmd_label_, sw_cmd_, LV_ALIGN_OUT_TOP_MID, 0, -10);
    if (is_sw_checked) {
        lv_obj_clear_flag(btn_readConfig_, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_style(btn_readConfig_, &btn_inactive_style_, 0);
        lv_obj_clear_flag(btn_writeConfig_, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_style(btn_writeConfig_, &btn_inactive_style_, 0);
        if (apply_modify) {
            as69_.SetMode(As69::Mode::Working);
            StartMavlinkThread();
        }

    } else {
        lv_obj_add_flag(btn_readConfig_, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_remove_style(btn_readConfig_, &btn_inactive_style_, 0);
        lv_obj_add_flag(btn_writeConfig_, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_remove_style(btn_writeConfig_, &btn_inactive_style_, 0);
        if (apply_modify) {
            StopMavlinkThread();
            as69_.SetMode(As69::Mode::Sleep);
        }
    }
}

void As69Config::InitStyles()
{
    if (has_inited_style_ == false) {
        has_inited_style_ = true;
        lv_style_init(&btn_style_);
        lv_style_set_width(&btn_style_, 140);
        lv_style_set_height(&btn_style_, 50);

        lv_style_init(&setting_tab_style_);
        lv_style_set_width(&setting_tab_style_, lv_pct(100));
        lv_style_set_height(&setting_tab_style_, 50);
        lv_style_set_pad_all(&setting_tab_style_, 4);

        lv_style_init(&btn_inactive_style_);
        lv_style_set_bg_color(&btn_inactive_style_, lv_color_make(127, 127, 127));
    }
}

lv_obj_t *As69Config::CreateTab(lv_obj_t *parent, const std::string &title)
{
    lv_obj_t *tab = lv_obj_create(parent);
    lv_obj_add_style(tab, &setting_tab_style_, 0);
    lv_obj_clear_flag(tab, LV_OBJ_FLAG_SCROLLABLE); // 禁止滚动
    lv_obj_t *label = lv_label_create(tab);
    lv_label_set_text(label, title.c_str());
    lv_obj_set_style_align(label, LV_ALIGN_LEFT_MID, 0);
    return tab;
}

lv_obj_t *As69Config::CreateDropdownTab(lv_obj_t *parent, const std::string &title, const char *options)
{
    lv_obj_t *tab = CreateTab(parent, title);

    lv_obj_t *dropdown = lv_dropdown_create(tab);
    lv_dropdown_set_options(dropdown, options);
    lv_obj_align(dropdown, LV_ALIGN_RIGHT_MID, 0, 0);

    return dropdown;
}

lv_obj_t *As69Config::Create4RollerTab(lv_obj_t *parent, const std::string &title, const char *option)
{
    lv_obj_t *tab = lv_obj_create(parent);
    lv_obj_set_size(tab, 180, lv_pct(100));
    lv_obj_set_style_pad_all(tab, 4, 0);
    lv_obj_clear_flag(tab, LV_OBJ_FLAG_SCROLLABLE); // 禁止滚动

    lv_obj_t *label = lv_label_create(tab);
    lv_label_set_text(label, title.c_str());
    lv_obj_set_style_align(label, LV_ALIGN_TOP_MID, 0);

    /*A roller on the middle with center aligned text, and auto (default) width*/
    address_roller[0] = lv_roller_create(tab);
    lv_obj_set_width(address_roller[0], 40);
    lv_roller_set_options(address_roller[0], option, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(address_roller[0], 3);
    lv_obj_align(address_roller[0], LV_ALIGN_BOTTOM_MID, -60, 0);

    address_roller[1] = lv_roller_create(tab);
    lv_obj_set_width(address_roller[1], 40);
    lv_roller_set_options(address_roller[1], option, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(address_roller[1], 3);
    lv_obj_align(address_roller[1], LV_ALIGN_BOTTOM_MID, -20, 0);

    address_roller[2] = lv_roller_create(tab);
    lv_obj_set_width(address_roller[2], 40);
    lv_roller_set_options(address_roller[2], option, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(address_roller[2], 3);
    lv_obj_align(address_roller[2], LV_ALIGN_BOTTOM_MID, 20, 0);

    address_roller[3] = lv_roller_create(tab);
    lv_obj_set_width(address_roller[3], 40);
    lv_roller_set_options(address_roller[3], option, LV_ROLLER_MODE_INFINITE);
    lv_roller_set_visible_row_count(address_roller[3], 3);
    lv_obj_align(address_roller[3], LV_ALIGN_BOTTOM_MID, 60, 0);
    return tab;
}

void As69Config::Init()
{
    LvglLock();
    InitStyles();
    lv_obj_set_style_pad_all(app_main_page, 4, 0);
    lv_obj_set_flex_flow(app_main_page, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(app_main_page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_column(app_main_page, 0, 0);
    lv_obj_set_style_pad_row(app_main_page, 0, 0);

    auto head_tab = lv_obj_create(app_main_page);
    lv_obj_clear_flag(head_tab, LV_OBJ_FLAG_SCROLLABLE); // 禁止滚动
    lv_obj_set_style_pad_all(head_tab, 4, 0);
    lv_obj_set_size(head_tab, lv_pct(100), 160);

    sw_cmd_ = lv_switch_create(head_tab);
    lv_obj_align(sw_cmd_, LV_ALIGN_LEFT_MID, 10, 40);
    if (as69_.GetMode() != As69::Mode::Sleep) {
        lv_obj_add_state(sw_cmd_, LV_STATE_CHECKED);
    }
    lv_obj_add_event_cb(sw_cmd_, As69Config::sw_event_handler, LV_EVENT_VALUE_CHANGED, this);
    sw_cmd_label_ = lv_label_create(head_tab);
    lv_obj_set_style_text_align(sw_cmd_label_, LV_TEXT_ALIGN_CENTER, 0);

    btn_tab_ = lv_obj_create(head_tab);
    lv_obj_set_style_border_side(btn_tab_, LV_BORDER_SIDE_NONE, 0);
    lv_obj_clear_flag(btn_tab_, LV_OBJ_FLAG_SCROLLABLE); // 禁止滚动
    lv_obj_set_style_pad_all(btn_tab_, 4, 0);
    lv_obj_set_size(btn_tab_, 150, 140);
    lv_obj_align(btn_tab_, LV_ALIGN_LEFT_MID, 80, 0);
    lv_obj_set_flex_flow(btn_tab_, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(btn_tab_, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(btn_tab_, 0, LV_PART_MAIN);

    btn_readConfig_ = lv_btn_create(btn_tab_);
    lv_obj_add_style(btn_readConfig_, &btn_style_, 0);
    btn_readConfigLabel_ = lv_label_create(btn_readConfig_);
    lv_label_set_text_static(btn_readConfigLabel_, "Read config");
    lv_obj_center(btn_readConfigLabel_);
    lv_obj_add_event_cb(btn_readConfig_, As69Config::readBtn_event_handler, LV_EVENT_CLICKED, this);

    btn_writeConfig_ = lv_btn_create(btn_tab_);
    lv_obj_add_style(btn_writeConfig_, &btn_style_, 0);
    btn_writeConfigLabel_ = lv_label_create(btn_writeConfig_);
    lv_label_set_text_static(btn_writeConfigLabel_, "Write config");
    lv_obj_center(btn_writeConfigLabel_);
    lv_obj_add_event_cb(btn_writeConfig_, As69Config::writeBtn_event_handler, LV_EVENT_CLICKED, this);

    OnSwUpdated(lv_obj_has_state(sw_cmd_, LV_STATE_CHECKED), false); // 这个函数会更新按钮状态，所以要在创建按钮后执行

    lv_obj_t *tab_address = Create4RollerTab(head_tab, "Address", "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\nA\nB\nC\nD\nE\nF");
    lv_obj_align(tab_address, LV_ALIGN_RIGHT_MID, -10, 0);

    dropdown_baud_rate_ = CreateDropdownTab(app_main_page, "Baud rate", "1200\n"
                                                                        "2400\n"
                                                                        "4800\n"
                                                                        "9600\n"
                                                                        "19200\n"
                                                                        "38400\n"
                                                                        "57600\n"
                                                                        "115200");

    dropdown_parity_ = CreateDropdownTab(app_main_page, "Parity", "None\n"
                                                                  "Odd\n"
                                                                  "Even");

    dropdown_channel_ = CreateDropdownTab(app_main_page, "Wireless Channel(Wireless Freq)", "0x00\n"
                                                                                            "0x01\n"
                                                                                            "0x02\n"
                                                                                            "0x03\n"
                                                                                            "0x04\n"
                                                                                            "0x05\n"
                                                                                            "0x06\n"
                                                                                            "0x07\n"
                                                                                            "0x08\n"
                                                                                            "0x09\n"
                                                                                            "0x0A\n"
                                                                                            "0x0B\n"
                                                                                            "0x0C");

    dropdown_power_ = CreateDropdownTab(app_main_page, "Send power", "20dBm\n"
                                                                     "12dBm\n"
                                                                     "10dBm\n"
                                                                     "5dBm");

    dropdown_io_config_ = CreateDropdownTab(app_main_page, "IO config", "Pull up\nPush pull");

    OnReadBtnClicked(false); // 从缓存中获取配置
    LvglUnlock();
}

As69Config InternalAs69Config(InternalAs69);
