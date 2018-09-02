#ifndef WAKATIME_OPTIONS_H
#define WAKATIME_OPTIONS_H

#include <coreplugin/dialogs/ioptionspage.h>

#include <QPointer>
#include <QWidget>


namespace Wakatime {
namespace Internal {

class WakaOptions : public QObject
{
    Q_OBJECT

public:
    WakaOptions(QObject *parent = nullptr);
    virtual ~WakaOptions() override = default;

    void read();
    void save();

    bool isDebug() const;
    bool isEnabled() const;
    bool inStatusBar() const;
    bool hasKey() const;
    QString apiKey() const;
    QString ignorePatern() const;

    void setDebug(bool val);
    void setEnabled(bool val);
    void setStatusBar(bool val);
    void setApiKey(const QString &val);
    void setIgnorePatern(const QString &val);

signals:
    void apiKeyChanged();
    void ignorePaternChanged();
    void inStatusBarChanged();

private:
    bool _isDebug = true;
    bool _isEnabled = true;
    bool _inStatusBar = false;
    QString _apiKey;
    QString _ignorePatern;
};

} // namespace Internal
} // namespace QtCreatorWakatime

#endif // WAKATIME_OPTIONS_H
