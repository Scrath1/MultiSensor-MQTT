#ifndef GLOBAL_OBJECTS_H
#define GLOBAL_OBJECTS_H
#include <Preferences.h>

#include <vector>

#include "RamLogger.h"
#include "filesystem/Filesystem.h"
#include "global.h"
#include "sensors/Sensor.h"
#include "settings.h"

extern RamLogger<RAMLOGGER_MAX_MESSAGE_COUNT, RAMLOGGER_MAX_STRING_LENGTH, RAMLOGGER_MAX_TIMESTAMP_STR_LEN> ramLogger;
extern Filesystem* const filesystem;
extern std::vector<Sensor*> sensors;
extern settings_t settings;
extern Preferences preferences;

#endif  // GLOBAL_OBJECTS_H