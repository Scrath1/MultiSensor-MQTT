#include "global_objects.h"

#include "cfg.h"
#ifdef ARDUINO
#include "filesystem/LittleFilesystem.h"
#else
#include "filesystem/DesktopFilesystem.h"
#endif  // ARDUINO

// global RamLogger object and its buffer
char ramLoggerBuffer[RAMLOGGER_MAX_STRING_LENGTH * RAMLOGGER_MAX_MESSAGE_COUNT];
RamLogger ramLogger{ramLoggerBuffer, sizeof(ramLoggerBuffer) / sizeof(ramLoggerBuffer[0]), RAMLOGGER_MAX_MESSAGE_COUNT};

#ifdef ARDUINO
LittleFilesystem lfs;
// Pointer to filesystem object
Filesystem* const filesystem = &lfs;
#else
DesktopFilesystem dfs;
// Pointer to filesystem object
Filesystem* const filesystem = &dfs;
#endif  // ARDUINO

// Vector containing pointers to sensors
std::vector<Sensor*> sensors;

// Global settings object
settings_t settings;
// Preferences object is used to store information
// that should not get exposed directly in a file
// like passwords
Preferences preferences;