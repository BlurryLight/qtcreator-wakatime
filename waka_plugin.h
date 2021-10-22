#pragma once

#include "waka_global.h"
#include "waka_constants.h"
#include "cligetter.h"

#include <extensionsystem/iplugin.h>

#include <QPointer>
#include <QFile>
#include <memory>

class QNetworkAccessManager;
class QNetworkReply;
class QToolButton;

namespace Core {
    class IEditor;    
    class IDocument;
}

namespace Wakatime {
namespace Internal {

class WakaOptions;
class WakaOptionsPage;
class CliGetter;

// For using OSInfo
using namespace Wakatime::Constants;

class WakaPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Wakatime.json")

public:

    WakaPlugin();
    ~WakaPlugin();
    static void ShowMessagePrompt(const QString str);

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

    void trySendHeartbeat(const QString &entry, bool isSaving);

    QNetworkAccessManager *getNetworkManager() const{
        return _netManager;
    }

    const OSInfo &getOsInfo() const{
        return this->_os_running_on;
    }
private:
    bool checkIfWakaCLIExist();

private slots:
    void onEditorAboutToChange(Core::IEditor *editor);
    void onEditorChanged(Core::IEditor *editor);
    void onAboutToSave(Core::IDocument *document);
    void onEditorStateChanged();

    void onIgnorePaternChanged();
    void onApiKeyChanged();
    void onInStatusBarChanged();

    void onNetReply(QNetworkReply *reply);


private:
    qint64 _lastTime = 0;
    QString _lastEntry{""};

    QString _location_of_cli;//location where wakatime-cli is to be stored
    OSInfo _os_running_on;//mark the os qtcreator is running in

    CliGetter *_cliGetter;

    QString _ignore_patern;
    std::unique_ptr<QUrl> _req_url;
    QPointer<QToolButton> _heartBeatButton;
    QNetworkAccessManager *_netManager;
    QSharedPointer<WakaOptions> _wakaOptions;

    const int64_t _cooldownTime = 120;
    const QString _urlPrefix{
        "https://wakatime.com/api/v1/users/current/heartbeats?api_key="};
};


} // namespace Internal
} // namespace QtCreatorWakatime
