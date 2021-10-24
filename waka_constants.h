#pragma once

#include <QString>

namespace Wakatime {
namespace Constants {

constexpr char SETTINGS_GROUP[] = "Wakatime";
constexpr char OPTION_CATEGORY[] = "Wakatime";
constexpr char OPTION_ID[] = "Wakatime";
const QString WAKATIME_RELEASE_URL =
        "https://api.github.com/repos/wakatime/wakatime-cli/releases/latest";

enum OSType{
    WINDOWS=0,LINUX,MACOS,UNKOWN
};

enum OSArch{
    AMD64=0,ARM64,ARM,I386
};

struct OSInfo
{
    OSType _os;
    OSArch _arch;
};
} // namespace QtCreatorWakatime
} // namespace Constants
