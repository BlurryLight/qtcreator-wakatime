#include "wakatime_plugin.h"
#include "wakatime_constants.h"
#include "wakatime_options_page.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projecttree.h>
#include <projectexplorer/project.h>
#include <extensionsystem/iplugin.h>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAction>
#include <QMessageBox>
#include <QMainWindow>
#include <QMenu>
#include <QJsonObject>
#include <QJsonDocument>

namespace QtCreatorWakatime {
namespace Internal {

QtCreatorWakatimePlugin::QtCreatorWakatimePlugin()
{
    // Create your members
}

QtCreatorWakatimePlugin::~QtCreatorWakatimePlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool QtCreatorWakatimePlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    
    //settings = new WakatimeOptionsPage();
    //IPlugin::addAutoReleasedObject(settings);

    netManager = new QNetworkAccessManager();
    connect(netManager, &QNetworkAccessManager::finished, this, &QtCreatorWakatimePlugin::onNetReply);

    connect(Core::EditorManager::instance(), &Core::EditorManager::aboutToSave, this, &QtCreatorWakatimePlugin::onAboutToSave);
    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged, this, &QtCreatorWakatimePlugin::onEditorChanged);

    return true;
}

void QtCreatorWakatimePlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag QtCreatorWakatimePlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)

    trySendHeartbeat(Core::EditorManager::currentDocument()->filePath().toString(), true);
    netManager->deleteLater();
    return SynchronousShutdown;
}

void QtCreatorWakatimePlugin::trySendHeartbeat(const QString &entity, bool isSaving = false)
{
    //https://github.com/wakatime/wakatime/blob/master/wakatime/arguments.py
    thread_local QJsonObject heartbeat
    {
        { "entity", QString() },
        { "entity_type", QString("file") },
        { "category", QString("coding") },
        //{ "hostname", QString("") },
        { "time", 0 },
        { "project", QString("") },
        { "branch", QString("master") },
        { "plugin", QString("Wakatime Plugin") },
        //{ "language", QString("C++") },
        { "is_write", false },
        { "is_debugging", false },
        { "lineno", 1}
    };

    if(!isEnabled)
    {
        qDebug() << "Wakatime reporting explicitly disabled!";
        return;
    }

    qint64 curr_time = time(nullptr);
    if(curr_time - lastTime < cooldownTime && !isSaving)
        return;

    heartbeat["entity"] = lastEntity = entity;
    heartbeat["time"] = lastTime = curr_time;
    heartbeat["project"] = ProjectExplorer::ProjectTree::currentProject()->displayName();
    heartbeat["is_write"] = isSaving;
    heartbeat["lineno"] = Core::EditorManager::currentEditor()->currentLine();

    QJsonDocument jdoc(heartbeat);
    QByteArray heartbeat_json = jdoc.toJson();

    if(isDebug)
    {
        qDebug() << "Sending heartbeat => " << heartbeat_json;
    }

    QNetworkRequest request;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);
    request.setUrl(QUrl(urlPrefix + apiKey));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, "Qt");
    netManager->post(request, heartbeat_json);
}

void QtCreatorWakatimePlugin::onNetReply(QNetworkReply *reply)
{
    if(isDebug)
    {
        qDebug() << "Network reply => " << reply->readAll();
    }
}

void QtCreatorWakatimePlugin::triggerAction()
{
}

void QtCreatorWakatimePlugin::onEditorChanged(Core::IEditor *editor)
{
    trySendHeartbeat(editor->document()->filePath().toString());
}

void QtCreatorWakatimePlugin::onAboutToSave(Core::IDocument *document)
{
    trySendHeartbeat(document->filePath().toString(), true);
}

} // namespace Internal
} // namespace QtCreatorWakatime
