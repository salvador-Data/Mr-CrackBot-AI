#include "crackbot/ui_bridge.h"

namespace crackbot::ui {

static StatusFn gStatus = nullptr;
static LineFn gLine = nullptr;
static ClearFn gClear = nullptr;
static ProgressFn gProgress = nullptr;

void init(StatusFn status, LineFn line, ClearFn clear, ProgressFn progress) {
  gStatus = status;
  gLine = line;
  gClear = clear;
  gProgress = progress;
}

void setStatus(const String &msg) {
  if (gStatus) gStatus(msg);
}

void println(const String &line) {
  if (gLine) gLine(line);
}

void clear() {
  if (gClear) gClear();
}

void progress(const String &msg) {
  if (gProgress) gProgress(msg);
  else if (gStatus) gStatus(msg);
}

}  // namespace crackbot::ui
