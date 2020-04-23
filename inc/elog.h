// ------------------------------------------------------------------------
// @brief:  easy log
// @file:   elog.h
// @author: qinhj@lsec.cc.ac.cn
// @date:   2020/04/23
// ------------------------------------------------------------------------
// @Note:   Currently, the output is hard coded to stdout.
// ------------------------------------------------------------------------

#ifndef EASY_LOG_H
#define EASY_LOG_H

#ifdef _MSC_VER

#include <Windows.h>// need for: STD_OUTPUT_HANDLE, ...
// 0: black; 1: blue; 2: green; 3: shallow green; 4: red
// 5: purple; 6: yellow; 7: white; 8: gray; 9: light blue
// ... F: high white
#define set_console_color(color) \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color)

#define COLOR_RED       0x0C
#define COLOR_WHITE     0x0F
#define COLOR_YELLOW    0x06

#else  //!_MSC_VER

#include <stdio.h>  // need for: fprintf
#define set_console_color(color) \
    fprintf(stdout, "\033[%dm", color)

#define COLOR_RED       31
#define COLOR_WHITE     0   // 37
#define COLOR_YELLOW    33

#endif /* _MSC_VER */

#define LOG_COLOR_ERROR COLOR_RED
#define LOG_COLOR_WARN  COLOR_YELLOW
#define LOG_COLOR_INFO  COLOR_WHITE

#define log_error(...)      do {        \
    set_console_color(LOG_COLOR_ERROR); \
    fprintf(stdout, "[E] " __VA_ARGS__);\
    set_console_color(LOG_COLOR_INFO);  \
} while (0)
#define log_warn(...)       do {        \
    set_console_color(LOG_COLOR_WARN);  \
    fprintf(stdout, "[W] " __VA_ARGS__);\
    set_console_color(LOG_COLOR_INFO);  \
} while (0)
#define log_info(...)       fprintf(stdout, "[I] " __VA_ARGS__)
#define log_debug(...)      fprintf(stdout, "[D] " __VA_ARGS__)
#define log_verbose(...)    fprintf(stdout, "[V] " __VA_ARGS__)

#endif  /* EASY_LOG_H */