#include "waka_plugin.h"
#include "waka_constants.h"
#include "waka_options.h"
#include "waka_options_page.h"

#include <coreplugin/icore.h>
#include <coreplugin/icontext.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/statusbarmanager.h>
#include <coreplugin/messagemanager.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projecttree.h>
#include <projectexplorer/project.h>

#include <extensionsystem/iplugin.h>
#include <texteditor/texteditor.h>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QToolButton>
#include <QTimer>

namespace Wakatime {
namespace Internal {

WakaPlugin::WakaPlugin()
{
    // Create your members
}

WakaPlugin::~WakaPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool WakaPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    
    _req_url = std::make_unique<QUrl>();
    _wakaOptions.reset(new WakaOptions);
    new WakaOptionsPage(_wakaOptions, this);

    _netManager = new QNetworkAccessManager();
    connect(_netManager, &QNetworkAccessManager::finished, this, &WakaPlugin::onNetReply);
    connect(_wakaOptions.data(), &WakaOptions::apiKeyChanged, this,
            &WakaPlugin::onApiKeyChanged);
    connect(_wakaOptions.data(), &WakaOptions::ignorePaternChanged, this,
            &WakaPlugin::onIgnorePaternChanged);
    connect(_wakaOptions.data(), &WakaOptions::inStatusBarChanged, this,
            &WakaPlugin::onInStatusBarChanged);

    connect(Core::EditorManager::instance(), &Core::EditorManager::aboutToSave, this, &WakaPlugin::onAboutToSave);
    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorAboutToChange, this, &WakaPlugin::onEditorAboutToChange);
    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged, this, &WakaPlugin::onEditorChanged);

    onApiKeyChanged();
    onIgnorePaternChanged();
    onInStatusBarChanged();

    QTC_ASSERT(!_wakaOptions->isDebug(), Core::MessageManager::write(QString("Waka plugin initialized!")));

    return true;
}

void WakaPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag WakaPlugin::aboutToShutdown()
{
  // Save settings
  // Disconnect from signals that are not needed during shutdown
  // Hide UI (if you add UI that is not in the main window directly)

  // don't do that. Potential race condition.
  //    trySendHeartbeat(Core::EditorManager::currentDocument()->filePath().toString(),
  //    true);
  QTC_ASSERT(!_wakaOptions->isDebug(), Core::MessageManager::write(QString(
                                           "Plugin is going to shutdown\n")));
  return SynchronousShutdown;
}

void WakaPlugin::trySendHeartbeat(const QString &entry, bool isSaving = false)
{
  thread_local QJsonObject heartbeat{
      {"entity", QString()},
      {"entity_type", QString("file")},
      {"category", QString("coding")},
      {"time", 0},
      {"project", QString("")},
      {"exclude", QString("")},
      {"branch", QString("master")},
      {"plugin", QString("QtCreator-wakatime/0.1.0")},
      {"is_write", false},
      {"is_debugging", false},
      {"lineno", 1},
      {"language", QString("C++")}};

  QTC_ASSERT(_wakaOptions->isEnabled(),
             QTC_ASSERT(!_wakaOptions->isDebug(),
                        Core::MessageManager::write(
                            "Wakatime reporting explicitly disabled!"));
             return;);
  QTC_ASSERT(_wakaOptions->hasKey(),
             Core::MessageManager::write(
                 "API key not set! Wakatime reporting disabled!");
             return;);

  qint64 curr_time = time(nullptr);
  if (curr_time - _lastTime < _cooldownTime && !isSaving &&
      _lastEntry == entry) {
    QTC_ASSERT(!_wakaOptions->isDebug(),
               Core::MessageManager::write(
                   QString("Heartbeat NOT send dt => %1, is_write => %2")
                       .arg(curr_time - _lastTime)
                       .arg(isSaving)));
    return;
    }

    heartbeat["entity"] = _lastEntry = entry;
    heartbeat["time"] = _lastTime = curr_time;
    heartbeat["project"] = ProjectExplorer::ProjectTree::currentProject()->displayName();
    heartbeat["is_write"] = isSaving;
    heartbeat["is_debugging"] = _wakaOptions->isDebug();
    heartbeat["exclude"] = _ignore_patern;
    heartbeat["lineno"] = Core::EditorManager::currentEditor()->currentLine();

    QJsonDocument jdoc(heartbeat);
    QByteArray heartbeat_json = jdoc.toJson();

    QNetworkRequest request;
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);
    request.setUrl(*_req_url.get());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString useragent;
    useragent = QString("%1-%2-Qt Creator wakatime Qt %3")
                    .arg(QSysInfo::kernelType(), QSysInfo::kernelVersion(),
                         QT_VERSION_STR);
    request.setHeader(QNetworkRequest::UserAgentHeader, useragent);
    _netManager->post(request, heartbeat_json);

    QTC_ASSERT(!_wakaOptions->isDebug(), Core::MessageManager::write(QString("Heartbeat send => %1 ").arg(QString::fromUtf8(heartbeat_json))));

    if(_wakaOptions->inStatusBar())
    {
        _heartBeatButton->setDisabled(false);
        QTimer::singleShot(200, [this]()
        {
            _heartBeatButton->setDisabled(true);
        });
    }
}

void WakaPlugin::onIgnorePaternChanged()
{
    _ignore_patern = _wakaOptions->ignorePatern();
}

void WakaPlugin::onApiKeyChanged()
{
    _req_url->setUrl(_urlPrefix + _wakaOptions->apiKey());
}

void WakaPlugin::onInStatusBarChanged()
{
    if(_heartBeatButton.data())
    {
        Core::StatusBarManager::destroyStatusBarWidget(_heartBeatButton.data());
    }
    if(_wakaOptions->inStatusBar())
    {
        _heartBeatButton = new QToolButton();
        _heartBeatButton->setIcon(QIcon(":/heartbeat.png"));
        _heartBeatButton->setDisabled(true);

        Core::StatusBarManager::addStatusBarWidget(_heartBeatButton, Core::StatusBarManager::RightCorner);
    }
}

void WakaPlugin::onNetReply(QNetworkReply *reply) {
  QTC_ASSERT(!_wakaOptions->isDebug(),
             Core::MessageManager::write(
                 QString("Network reply => %1")
                     .arg(QString::fromUtf8(reply->readAll()))));
  int status =
      reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  QTC_ASSERT(!_wakaOptions->isDebug(),
             Core::MessageManager::write(
                 QString("Network reply code => %1").arg(QString(status))));
}

void WakaPlugin::onEditorAboutToChange(Core::IEditor *editor)
{
    if(!editor)
        return;

    disconnect(TextEditor::TextEditorWidget::currentTextEditorWidget(), &TextEditor::TextEditorWidget::textChanged, this, &WakaPlugin::onEditorStateChanged);
}

void WakaPlugin::onEditorChanged(Core::IEditor *editor)
{
    if(!editor)
        return;

    connect(TextEditor::TextEditorWidget::currentTextEditorWidget(), &TextEditor::TextEditorWidget::textChanged, this, &WakaPlugin::onEditorStateChanged);
    trySendHeartbeat(editor->document()->filePath().toString());
}

void WakaPlugin::onAboutToSave(Core::IDocument *document)
{
    trySendHeartbeat(document->filePath().toString(), true);
}

void WakaPlugin::onEditorStateChanged()
{
    trySendHeartbeat(Core::EditorManager::currentDocument()->filePath().toString());
}

} // namespace Internal
} // namespace QtCreatorWakatime
