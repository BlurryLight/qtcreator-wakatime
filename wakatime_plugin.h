#pragma once

#include "wakatime_global.h"

#include <extensionsystem/iplugin.h>

class QNetworkAccessManager;
class QNetworkReply;

namespace Core {
    class IEditor;    
    class IDocument;
}

namespace QtCreatorWakatime {

class WakatimeOptionsPage;
namespace Internal {

class QtCreatorWakatimePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QtCreatorWakatime.json")

public:
    QtCreatorWakatimePlugin();
    ~QtCreatorWakatimePlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

    void trySendHeartbeat(const QString &file, bool isSaving);

private slots:
    void triggerAction();

    void onEditorChanged(Core::IEditor *editor);
    void onAboutToSave(Core::IDocument *document);

    void onNetReply(QNetworkReply *reply);

private:
    bool isDebug = true;    
    bool isEnabled = true;
    qint64 lastTime = 0;
    QString lastEntity = "";
    QNetworkAccessManager *netManager;
    QString apiKey;

    WakatimeOptionsPage* settings;

    const int64_t cooldownTime = 120;    
    const QString urlPrefix = "https://wakatime.com/api/v1/users/current/heartbeats?api_key=";
};

} // namespace Internal
} // namespace QtCreatorWakatime
