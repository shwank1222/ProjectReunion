#pragma once

#include "Runtime/Launch/Resources/Version.h"

#define RL_UE_VERSION_AT_LEAST(Major, Minor) \
    ((ENGINE_MAJOR_VERSION > (Major)) || (ENGINE_MAJOR_VERSION == (Major) && ENGINE_MINOR_VERSION >= (Minor)))

#define RL_UE_5_8_OR_LATER RL_UE_VERSION_AT_LEAST(5, 8)
