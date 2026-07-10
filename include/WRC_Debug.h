#pragma once
#include <Arduino.h>

enum DebugLevel
{
    DEBUG_NONE = 0,
    DEBUG_ERROR,
    DEBUG_WARN,
    DEBUG_INFO,
    DEBUG_VERBOSE
};

extern DebugLevel DEBUG_LEVEL;

// Macros de log
#define LOG_ERROR(fmt, ...)   if (DEBUG_LEVEL >= DEBUG_ERROR)   Serial.printf("[ERROR] " fmt "\n", ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)    if (DEBUG_LEVEL >= DEBUG_WARN)    Serial.printf("[WARN] "  fmt "\n", ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)    if (DEBUG_LEVEL >= DEBUG_INFO)    Serial.printf("[INFO] "  fmt "\n", ##__VA_ARGS__)
#define LOG_VERBOSE(fmt, ...) if (DEBUG_LEVEL >= DEBUG_VERBOSE) Serial.printf("[VERBOSE] " fmt "\n", ##__VA_ARGS__)

class WRC_Debug
{
public:
    static void handleSerialCommand();
};
