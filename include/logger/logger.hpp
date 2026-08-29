#pragma once 

#include <spdlog/spdlog.h>

#define     ARK_LEVEL_TRACE     0
#define     ARK_LEVEL_DEBUG     1
#define     ARK_LEVEL_INFO      2
#define     ARK_LEVEL_WARN      3
#define     ARK_LEVEL_ERROR     4
#define     ARK_LEVEL_CRITICAL  5
#define     ARK_LEVEL_OFF       6

#define ARK_LEVEL_NAME_TRACE spdlog::string_view_t("trace", 5)
#define ARK_LEVEL_NAME_DEBUG spdlog::string_view_t("debug", 5)
#define ARK_LEVEL_NAME_INFO spdlog::string_view_t("info", 4)
#define ARK_LEVEL_NAME_WARNING spdlog::string_view_t("warning", 7)
#define ARK_LEVEL_NAME_ERROR spdlog::string_view_t("error", 5)
#define ARK_LEVEL_NAME_CRITICAL spdlog::string_view_t("critical", 8)
#define ARK_LEVEL_NAME_OFF spdlog::string_view_t("off", 3)

namespace Ark 
{
    enum ELogLevel : int 
    {
        Trace = ARK_LEVEL_TRACE,
        Debug = ARK_LEVEL_DEBUG,
        Info = ARK_LEVEL_INFO,
        Warn = ARK_LEVEL_WARN,
        Err = ARK_LEVEL_ERROR,
        Critical = ARK_LEVEL_CRITICAL,
        Off = ARK_LEVEL_OFF,
        _max
    };

    class ULogger
    {

        static ULogger _logger;

        ULogger() = default;
        ~ULogger() = default;

        ULogger(ULogger&) = delete;
        ULogger(ULogger&&) = delete;

        ULogger& operator=(ULogger&) = delete;
        ULogger& operator=(ULogger&&) = delete;

    public:

        ULogger& Get() noexcept;

    };
}
