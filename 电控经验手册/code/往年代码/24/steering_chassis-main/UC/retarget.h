//
// Created by tony on 2023/4/12.
//

#ifndef FRESHMANPROJECT_RETARGET_H
#define FRESHMANPROJECT_RETARGET_H

#include "stm32f4xx_hal.h"
#include <sys/stat.h>
#include <stdio.h>

void RetargetInit(UART_HandleTypeDef *huart);

int _isatty(int fd);

int _write(int fd, char *ptr, int len);

int _close(int fd);

int _lseek(int fd, int ptr, int dir);

int _read(int fd, char *ptr, int len);

int _fstat(int fd, struct stat *st);

#endif //FRESHMANPROJECT_RETARGET_H
