#pragma once

#include <fmt/core.h>

namespace debug
{

/**
 * Do not directly use anything in this namespace from outside of this logging
 * file
 */
namespace detail
{

void LogPrint(fmt::string_view format, fmt::format_args args);

void LogInfoPrefix();
void LogDebugPrefix();
void LogWarnPrefix();
void LogErrorPrefix();

}  // namespace detail

template <class Str, class... Args>
void LogInfo(const Str &format, Args &&...args)
{
    detail::LogInfoPrefix();
    detail::LogPrint(format, fmt::make_format_args(args...));
}

template <class Str, class... Args>
void LogDebug(const Str &format, Args &&...args)
{
    detail::LogDebugPrefix();
    detail::LogPrint(format, fmt::make_format_args(args...));
}

template <class Str, class... Args>
void LogWarn(const Str &format, Args &&...args)
{
    detail::LogWarnPrefix();
    detail::LogPrint(format, fmt::make_format_args(args...));
}

template <class Str, class... Args>
void LogError(const Str &format, Args &&...args)
{
    detail::LogErrorPrefix();
    detail::LogPrint(format, fmt::make_format_args(args...));
}

}  // namespace debug
