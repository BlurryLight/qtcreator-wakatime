#include "waka_options.h"
#include "waka_constants.h"

#include <QDir>

#include <coreplugin/icore.h>

namespace Wakatime {
namespace Internal {

namespace
{
constexpr char SETTINGS_GROUP[]                     = "settings";
constexpr char PERSONAL_SETTINGS_GROUP[]            = "personal_settings";
constexpr char IS_DEBUG[]                           = "debug";
constexpr char IS_ENABLED[]                         = "enabled";
constexpr char IN_STATUS_BAR[]                      = "status_bar_enabled";
constexpr char API_KEY[]                            = "api_key";
constexpr char EXCLUDE[]                      		= "exclude";
}

WakaOptions::WakaOptions(QObject *parent) : QObject (parent),
    _wakatimeCFG(QDir::homePath()+QDir::separator()+".wakatime.cfg")
{
    read();
}

void WakaOptions::read()
{
    QSettings s(_wakatimeCFG,QSettings::Format::IniFormat);
    s.beginGroup(SETTINGS_GROUP);
    _isDebug = s.value(IS_DEBUG, false).toBool();
    _statusBarEnabled = s.value(IN_STATUS_BAR, false).toBool();
    _apiKey = s.value(API_KEY, QString()).toString();
    _excludePattern = s.value(EXCLUDE, QString()).toString();
    s.endGroup();

    s.beginGroup(PERSONAL_SETTINGS_GROUP);
    _isEnabled = s.value(IS_ENABLED, false).toBool();
    s.endGroup();
}

void WakaOptions::save()
{
    QSettings s(_wakatimeCFG,QSettings::Format::IniFormat);
    s.beginGroup(SETTINGS_GROUP);
    s.setValue(IS_DEBUG, _isDebug);
    s.setValue(IN_STATUS_BAR, _statusBarEnabled);
    s.setValue(API_KEY, _apiKey);
    s.setValue(EXCLUDE, _excludePattern);
    s.endGroup();

    s.beginGroup(PERSONAL_SETTINGS_GROUP);
    s.setValue(IS_ENABLED, _isEnabled);
    s.endGroup();
}

bool WakaOptions::isDebug() const { return _isDebug; }
bool WakaOptions::isEnabled() const { return _isEnabled; }
bool WakaOptions::inStatusBar() const { return _statusBarEnabled; }
bool WakaOptions::hasKey() const { return !_apiKey.isEmpty(); }
QString WakaOptions::apiKey() const { return _apiKey; }
QString WakaOptions::excludePattern() const { return _excludePattern; }

void WakaOptions::setDebug(bool val) { _isDebug = val; }
void WakaOptions::setEnabled(bool val) { _isEnabled = val; }
void WakaOptions::setStatusBar(bool val) { _statusBarEnabled = val; emit inStatusBarChanged(); }
void WakaOptions::setApiKey(const QString &val) { _apiKey = val; emit apiKeyChanged(); }
void WakaOptions::setExcludePatern(const QString &val) { _excludePattern = val; emit ignorePaternChanged(); }

} // namespace Internal
} // namespace QtCreatorWakatime
