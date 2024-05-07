#ifndef GLOBAL_OBJECTS_H
#define GLOBAL_OBJECTS_H
#include <vector>
#include <Preferences.h>
#include "RamLogger.h"
#include "global.h"
#include "filesystem/Filesystem.h"
#include "sensors/Sensor.h"
#include "settings.h"

extern RamLogger ramLogger;
extern Filesystem* const filesystem;
extern std::vector<Sensor*> sensors;
extern settings_t settings;
extern Preferences preferences;

#endif // GLOBAL_OBJECTS_H