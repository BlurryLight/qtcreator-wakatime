#include "waka_options.h"
#include "waka_constants.h"

#include <coreplugin/icore.h>

namespace QtCreatorWakatime {
namespace Internal {

namespace
{
const char GROUP[]                              = "General";
const char IS_DEBUG[]                           = "isDebug";
const char IS_ENABLED[]                         = "isEnabled";
const char IN_STATUS_BAR[]                      = "inStatusBar";
const char API_KEY[]                            = "API_KEY";
const char IGNORE_PATERN[]                      = "IGNORE_PATERN";
}

WakaOptions::WakaOptions(QObject *parent) : QObject (parent)
{
    read();
}

void WakaOptions::read()
{
    QSettings *s = Core::ICore::settings();
    s->beginGroup(Constants::SETTINGS_GROUP);
    s->beginGroup(GROUP);
    _isDebug = s->value(IS_DEBUG, false).toBool();
    _isEnabled = s->value(IS_ENABLED, false).toBool();
    _inStatusBar = s->value(IN_STATUS_BAR, false).toBool();
    _apiKey = s->value(API_KEY, QString()).toString();
    _ignorePatern = s->value(IGNORE_PATERN, QString()).toString();
    s->endGroup();
    s->endGroup();
}

void WakaOptions::save()
{
    QSettings *s = Core::ICore::settings();
    s->beginGroup(Constants::SETTINGS_GROUP);
    s->beginGroup(GROUP);
    s->setValue(IS_DEBUG, _isDebug);
    s->setValue(IS_ENABLED, _isEnabled);
    s->setValue(IN_STATUS_BAR, _inStatusBar);
    s->setValue(API_KEY, _apiKey);
    s->setValue(IGNORE_PATERN, _ignorePatern);
    s->endGroup();
    s->endGroup();
}

bool WakaOptions::isDebug() const { return _isDebug; }
bool WakaOptions::isEnabled() const { return _isEnabled; }
bool WakaOptions::inStatusBar() const { return _inStatusBar; }
bool WakaOptions::hasKey() const { return !_apiKey.isEmpty(); }
QString WakaOptions::apiKey() const { return _apiKey; }
QString WakaOptions::ignorePatern() const { return _ignorePatern; }

void WakaOptions::setDebug(bool val) { _isDebug = val; }
void WakaOptions::setEnabled(bool val) { _isEnabled = val; }
void WakaOptions::setStatusBar(bool val) { _inStatusBar = val; emit inStatusBarChanged(); }
void WakaOptions::setApiKey(const QString &val) { _apiKey = val; emit apiKeyChanged(); }
void WakaOptions::setIgnorePatern(const QString &val) { _ignorePatern = val; emit ignorePaternChanged(); }

} // namespace Internal
} // namespace QtCreatorWakatime
