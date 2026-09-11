#pragma once

#include "Types.h"
#include <format>
// #include "Engine/Engine.h"

// #define LOG(Verbosity, ...)                                     \
// Engine::GetConsolePanel()->AddLog(ELogVerbosity::Verbosity, __VA_ARGS__);


#define LOG(Category, Verbosity, ...) \
    FLogger::Log( \
        ELogCategory::Category, \
        ELogVerbosity::Verbosity, \
        __VA_ARGS__ \
    )


enum class ELogCategory
{
    Engine,
    World,
    Renderer,
    Object,
    Editor
};

enum class ELogVerbosity
{
    Input,
    Info,
    Warning,
    Error
};

struct FLogMessage
{
    ELogCategory Category;
    ELogVerbosity Verbosity;
    FString Message;
};

struct FLogData
{
    ELogCategory Category;
    ELogVerbosity Verbosity;
    FString Message;
};

class FLogger
{
public:

    template<typename... Args>
    static void Log(
        ELogCategory Category,
        ELogVerbosity Verbosity,
        std::format_string<Args...> Format,
        Args&&... args)
    {
        FLogMessage LogMessage;

        LogMessage.Category = Category;
        LogMessage.Verbosity = Verbosity;

        LogMessage.Message =
            std::format(
                Format,
                std::forward<Args>(args)...
            );

        Logs.push_back(std::move(LogMessage));

        // 이후 Output Device들에게 전달
    }

    static const TArray<FLogMessage>& GetLogs()
    {
        return Logs;
    }

    static void Clear()
    {
        Logs.clear();
    }


private:

    inline static TArray<FLogMessage> Logs;
};