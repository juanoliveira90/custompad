#ifndef GET_H
#define GET_H

#include <dirent.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <linux/input.h>

#include "controller.h"

#define BITS_PER_LONG (8 * sizeof(unsigned long))
#define ARR_SIZE(key) (((key) / BITS_PER_LONG) + 1)

void get_controllers();
void display_controllers();
struct input_event read_input(int fd, ssize_t n, struct input_event ev);

#endif