#pragma once

#include <Arduino.h>

namespace crackbot::ui {

using StatusFn = void (*)(const String &msg);
using LineFn = void (*)(const String &line);
using ClearFn = void (*)();
using ProgressFn = void (*)(const String &msg);

void init(StatusFn status, LineFn line, ClearFn clear, ProgressFn progress = nullptr);
void setStatus(const String &msg);
void println(const String &line);
void clear();
void progress(const String &msg);

}  // namespace crackbot::ui
