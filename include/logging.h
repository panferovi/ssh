#pragma once

#include <string.h>

void print_log(char* str, ...);

#define log(fmt, ...) print_log("%s:%d " fmt, __func__, __FILE__, __LINE__, ##__VA_ARGS__)
#define log_info(fmt, ...) log("[INFO] " fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) log("[ERROR] " fmt, ##__VA_ARGS__)
#define log_perror(fmt, ...) log_error("(%d) %s: " fmt, errno, strerror(errno), ##__VA_ARGS__)