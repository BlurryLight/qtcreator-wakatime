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
    static QDir getWakaCLILocation();

private:
    bool checkIfWakaCLIExist();

private slots:
    void onEditorAboutToChange(Core::IEditor *editor);
    void onEditorChanged(Core::IEditor *editor);
    void onAboutToSave(Core::IDocument *document);
    void onEditorStateChanged();

    void onInStatusBarChanged();

    void onNetReply(QNetworkReply *reply){};
    void onDoneSettingUpCLI();

signals:
    void doneGettingCliAndSettingItUp();
    void sendHeartBeat(QString file);

private:
    qint64 _lastTime = 0;
    QString _lastEntry{""};

    CliGetter *_cliGetter;//managing accessing wakatime-cli
    bool _cliIsSetup;

    QString _ignore_patern;
    QThread *_cliGettingThread;
    std::unique_ptr<QUrl> _req_url;
    QPointer<QToolButton> _heartBeatButton;
    QSharedPointer<WakaOptions> _wakaOptions;

    const int64_t _cooldownTime = 120;
    const QString _urlPrefix{
        "https://wakatime.com/api/v1/users/current/heartbeats?api_key="};
};


} // namespace Internal
} // namespace QtCreatorWakatime
