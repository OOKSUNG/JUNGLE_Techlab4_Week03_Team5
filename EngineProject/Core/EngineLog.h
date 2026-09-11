#pragma once

#include "Engine/Engine.h"

#define LOG(Verbosity, ...)                                     \
do                                                              \
{                                                               \
    if (FConsolePanel* ConsolePanel = Engine::GetConsolePanel())\
    {                                                           \
        ConsolePanel->AddLog(                                   \
            ELogVerbosity::Verbosity,                           \
            __VA_ARGS__                                         \
        );                                                      \
    }                                                           \
} while (0)


// Engine::GetConsolePanel()->AddLog(ELogVerbosity::Verbosity, __VA_ARGS__);