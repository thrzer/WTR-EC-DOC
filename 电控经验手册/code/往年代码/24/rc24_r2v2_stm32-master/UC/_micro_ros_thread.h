//
// Created by tony on 23-11-2.
//

#ifndef _MICRO_ROS_THREAD_H
#define _MICRO_ROS_THREAD_H
#ifdef __cplusplus
extern "C" {
#endif


//extern void timer1_callback(rcl_timer_t *timer, int64_t last_call_time);
//extern void joint_cmd_subscribe_callback(const void *msgin);
void StartMicrorosTask(void *argument);

#ifdef __cplusplus
}
#endif
#endif //_MICRO_ROS_THREAD_H
