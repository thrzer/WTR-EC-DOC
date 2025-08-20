//
// Created by Liam
// 2023/12/01
//

#include "HStateMachine.h"
#include "stm32f4xx_hal_def.h"

HSM_EVENT HSM_RootHandler(HSM *This, HSM_EVENT event, void *param)
{
    return HSM_NULL;
}

HSM_STATE const HSM_ROOT =
{
    .parent = ((void *)0),
    .handler = HSM_RootHandler,
    .name = ":ROOT:",
    .level = 0
};

void HSM_STATE_Create(HSM_STATE *This, const char *name, HSM_FN handler, HSM_STATE *parent)
{
    if (((void *)0) == parent)
    {
        parent = (HSM_STATE *)&HSM_ROOT;
    }
    This->name = name;
    This->handler = handler;
    This->parent = parent;
    This->level = parent->level + 1;
    if (This->level >= HSM_MAX_DEPTH)
    {
        while(1);
    }
}

void HSM_Create(HSM *This, const char *name, HSM_STATE *initState)
{
    // Supress warning for unused variable if HSM_FEATURE_DEBUG_ENABLE is not defined
    (void)name;

    // Initialize state
    This->curState = initState;
    // Invoke ENTRY and INIT event
    This->curState->handler(This, HSM_ENTRY, 0);
    This->curState->handler(This, HSM_INIT, 0);
}

HSM_STATE *HSM_GetState(HSM *This)
{
    // This returns the current HSM state
    return This->curState;
}

__weak const char *HSM_Evt2Str(uint32_t event)
{
    return "UNKNOWN";
}

uint8_t HSM_IsInState(HSM *This, HSM_STATE *state)
{
    HSM_STATE *curState;
    // Traverse the parents to find the matching state.
    for (curState = This->curState; curState; curState = curState->parent)
    {
        if (state == curState)
        {
            // Match found, HSM is in state or parent state
            return 1;
        }
    }
    // This HSM is not in state or parent state
    return 0;
}

void HSM_Run(HSM *This, HSM_EVENT event, void *param)
{
    // This runs the state's event handler and forwards unhandled events to
    // the parent state
    HSM_STATE *state = This->curState;
    while (event)
    {
        event = state->handler(This, event, param);
        state = state->parent;
        if (event)
        {
            
        }
    }
}

void HSM_Tran(HSM *This, HSM_STATE *nextState, void *param, void (*method)(HSM *This, void *param))
{
    HSM_STATE *list_exit[HSM_MAX_DEPTH];
    HSM_STATE *list_entry[HSM_MAX_DEPTH];
    uint8_t cnt_exit = 0;
    uint8_t cnt_entry = 0;
    uint8_t idx;
    // This performs the state transition with calls of exit, entry and init
    // Bulk of the work handles the exit and entry event during transitions
    // 1) Find the lowest common parent state
    HSM_STATE *src = This->curState;
    HSM_STATE *dst = nextState;
    // 1a) Equalize the levels
    while (src->level != dst->level)
    {
        if (src->level > dst->level)
        {
            // source is deeper
            list_exit[cnt_exit++] = src;
            src = src->parent;
        }
        else
        {
            // destination is deeper
            list_entry[cnt_entry++] = dst;
            dst = dst->parent;
        }
    }
    // 1b) find the common parent
    while (src != dst)
    {
        list_exit[cnt_exit++] = src;
        src = src->parent;
        list_entry[cnt_entry++] = dst;
        dst = dst->parent;
    }
    // 2) Process all the exit events
    for (idx = 0; idx < cnt_exit; idx++)
    {
        src = list_exit[idx];
        src->handler(This, HSM_EXIT, param);
    }
    // 3) Call the transitional method hook
    if (method)
    {
        method(This, param);
    }
    // 4) Process all the entry events
    for (idx = 0; idx < cnt_entry; idx++)
    {
        dst = list_entry[cnt_entry - idx - 1];
        dst->handler(This, HSM_ENTRY, param);
    }
    // 5) Now we can set the destination state
    This->curState = nextState;
}