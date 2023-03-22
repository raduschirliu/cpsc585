#include "engine/core/debug/Log.h"

#include <fmt/format.h>
#include <vivid/vivid.h>

namespace ansi = vivid::ansi;

namespace debug
{

namespace detail
{
void LogPrint(fmt::string_view format, fmt::format_args args)
{
    fmt::vprint(format, args);
    fmt::print("\n");
}

void LogInfoPrefix()
{
    fmt::print("[{}{:^8}{}]  ", ansi::white, "INFO", ansi::reset);
}

void LogDebugPrefix()
{
    fmt::print("[{}{:^8}{}]  ", ansi::green, "DEBUG", ansi::reset);
}

void LogWarnPrefix()
{
    fmt::print("[{}{:^8}{}]  ", ansi::yellow, "WARN", ansi::reset);
}

void LogErrorPrefix()
{
    fmt::print("[{}{:^8}{}]  ", ansi::red, "ERROR", ansi::reset);
}
}  // namespace detail

}  // namespace debug