#pragma once

#include "lvgl/lvgl.h"
#include "lvgl_thread.h"
#include "lvgl_app.hpp"

class PageMgr
{
public:
    lv_obj_t *window = nullptr;

    void SetSize(lv_coord_t w, lv_coord_t h)
    {
        LvglLock();
        lv_obj_set_size(window, w, h);
        LvglUnlock();
    }

    void SetAlign(lv_align_t align)
    {
        LvglLock();
        lv_obj_set_align(window, align);
        LvglUnlock();
    }

    void FillParent()
    {
        SetSize(lv_pct(100), lv_pct(100));
    }

    void Init(lv_obj_t *parent)
    {
        window = lv_obj_create(parent);
        lv_obj_set_style_pad_all(window, 0, 0);
        lv_obj_set_style_border_side(window, LV_BORDER_SIDE_NONE, 0);
    }

    void DeInit()
    {
        if (window != nullptr) {
            lv_obj_del(window);
        }
    }

    lv_obj_t *NewPage()
    {
        LvglLock();
        lv_obj_t *result = lv_obj_create(window);
        lv_obj_set_size(result, lv_pct(100), lv_pct(100));
        LvglUnlock();
        return result;
    }

    bool IsMyPage(const lv_obj_t *page) const
    {
        if (page == nullptr) return false;
        return lv_obj_get_parent(page) == window;
    }

    /**
     * @brief 获取 page 的序号
     *
     * @param page
     * @return int32_t 成功返回号，失败返回 -1
     */
    int32_t GetPageIndex(const lv_obj_t *page) const
    {
        if (IsMyPage(page)) {
            return lv_obj_get_index(page);
        } else {
            return -1;
        }
    }

    bool SwitchToPage(lv_obj_t *page)
    {
        if (IsMyPage(page)) {
            LvglLock();
            lv_obj_move_foreground(page);
            LvglUnlock();
            return true;
        }
        return false;
    }

    bool SwitchToPage(int index)
    {
        auto page = lv_obj_get_child(window, index);
        if (page == nullptr) {
            return false;
        }

        LvglLock();
        lv_obj_move_foreground(page);
        LvglUnlock();
        return true;
    }

    bool SwitchToApp(LvglApp &app)
    {
        return SwitchToPage(app.app_main_page);
    }

    void DelPage(lv_obj_t *page)
    {
        LvglLock();
        lv_obj_del(page);
        LvglUnlock();
    }

    bool DelPage(int32_t index)
    {
        LvglLock();
        auto page = lv_obj_get_child(window, index);

        if (page == nullptr) {
            LvglUnlock();
            return false;
        }

        lv_obj_del(page);
        LvglUnlock();
        return true;
    }

    void MovePage(lv_obj_t *page, lv_obj_t *parent)
    {
        LvglLock();
        lv_obj_set_parent(page, parent);
        LvglUnlock();
    }

    void MovePage(lv_obj_t *page, PageMgr &window_mgr)
    {
        LvglLock();
        lv_obj_set_parent(page, window_mgr.window);
        LvglUnlock();
    }

    ~PageMgr()
    {
        DeInit();
    }
};
