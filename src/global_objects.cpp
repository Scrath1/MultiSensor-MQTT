#include "global_objects.h"

#include "cfg.h"
#ifdef ARDUINO
    #include "filesystem/LittleFilesystem.h"
#else
    #include "filesystem/DesktopFilesystem.h"
#endif  // ARDUINO

// global RamLogger object
RamLogger<RAMLOGGER_MAX_MESSAGE_COUNT, RAMLOGGER_MAX_STRING_LENGTH, RAMLOGGER_MAX_TIMESTAMP_STR_LEN> ramLogger;

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