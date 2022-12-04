#pragma once

#include <iostream>
#include <chrono>

#ifdef ENABLE_DEBUG_MACRO
#  define LogError(msg, ...) fprintf(stdout, "\x1B[31m[Error]\033[0m \x1B[90m" __FILE__ ":%d\033[0m " msg "\n", __LINE__, ##__VA_ARGS__);
#  define LogInfo(msg, ...) fprintf(stdout, "\x1B[33m[Info]\033[0m \x1B[90m" __FILE__ ":%d\033[0m " msg "\n", __LINE__, ##__VA_ARGS__);
#  define DebugAssert(cond, msg) if(cond) { LogError(msg); std::exit(EXIT_FAILURE); }
#else
#  define LogError(msg, ...) 
#  define LogInfo(msg, ...) 
#  define DebugAssert(cond, msg)
#endif

class ScopeTimer {
private:
    std::chrono::time_point<std::chrono::system_clock> start, end;

public:
    ScopeTimer();
    ~ScopeTimer();
};