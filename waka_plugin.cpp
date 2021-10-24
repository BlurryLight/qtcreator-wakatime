#include "waka_plugin.h"
#include "waka_constants.h"
#include "waka_options.h"
#include "waka_options_page.h"

#include <cstring>

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
#include <QThread>

namespace Wakatime {
namespace Internal {

WakaPlugin::WakaPlugin():_cliIsSetup(false){}

WakaPlugin::~WakaPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

void WakaPlugin::ShowMessagePrompt(const QString str){

#if IDE_LESS_15_VERSION==1
   Core::MessageManager::write(str);
#else
   Core::MessageManager::writeDisrupting(QString(str);
#endif
}

QDir WakaPlugin::getWakaCLILocation(){
    QString default_path = QDir::homePath()+"/.wakatime/wakatime-cli";
    return default_path;
}

bool WakaPlugin::checkIfWakaCLIExist(){
    return getWakaCLILocation().exists();
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

    _cliGetter = new CliGetter();

    _cliGettingThread = new QThread(this);
    _cliGetter->moveToThread(_cliGettingThread);

    connect(this,&WakaPlugin::doneGettingCliAndSettingItUp,
            this,&WakaPlugin::onDoneSettingUpCLI);

    //check if has wakatime-cli in path
    bool waka_cli_found = checkIfWakaCLIExist();
    //if not then try download it based of the users operating system
    if(waka_cli_found==false){
        _cliGetter->connect(_cliGettingThread,&QThread::started,
                            _cliGetter,&CliGetter::startGettingAssertUrl);
        connect(_cliGetter,&CliGetter::promptMessage,this,&ShowMessagePrompt);
        connect(_cliGetter,&CliGetter::doneSettingWakaTimeCli,
                [plugin = this](){
            plugin->_cliIsSetup=true;
            emit plugin->doneGettingCliAndSettingItUp();
        });
    }else{
        emit this->doneGettingCliAndSettingItUp();
    }
    _cliGettingThread->start();
    return true;
}

void WakaPlugin::onDoneSettingUpCLI(){
    ShowMessagePrompt("WakatimeCLI setup");

    //check if is latest version
    //check if user has asked for updated version
    //if so, then try update the version of wakatime-cli

    _req_url = std::make_unique<QUrl>();
    _wakaOptions.reset(new WakaOptions);
    new WakaOptionsPage(_wakaOptions, this);

    //connect(_wakaOptions.data(), &WakaOptions::inStatusBarChanged,
    //        this, &WakaPlugin::onInStatusBarChanged);

    //connect(Core::EditorManager::instance(), &Core::EditorManager::aboutToSave,
    //        this, &WakaPlugin::onAboutToSave);
    //connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorAboutToChange,
    //        this, &WakaPlugin::onEditorAboutToChange);
    //connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged,
    //        this, &WakaPlugin::onEditorChanged);

    //onApiKeyChanged();
    //onInStatusBarChanged();

    //QTC_ASSERT(!_wakaOptions->isDebug(),ShowMessagePrompt("Waka plugin initialized!"));
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
    QTC_ASSERT(!_wakaOptions->isDebug(),
               ShowMessagePrompt( "Plugin is going to shutdown\n"));
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
                        ShowMessagePrompt("Wakatime reporting explicitly disabled!"));
             return;);
  QTC_ASSERT(_wakaOptions->hasKey(),
                         ShowMessagePrompt("API key not set! Wakatime reporting disabled!"));

  qint64 curr_time = time(nullptr);
  if (curr_time - _lastTime < _cooldownTime && !isSaving &&
      _lastEntry == entry) {
    QTC_ASSERT(!_wakaOptions->isDebug(),
                             ShowMessagePrompt(QString("Heartbeat NOT send dt => %1, is_write => %2")
                       .arg(curr_time - _lastTime)
                       .arg(isSaving)));
    return;
    }

    heartbeat["entity"] = _lastEntry = entry;
    heartbeat["time"] = _lastTime = curr_time;
    if (const auto &project = ProjectExplorer::ProjectTree::currentProject())
        heartbeat["project"] = project->displayName();
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
    useragent = QString("%1-%2-Qt Creator wakatime Qt %3").arg(QSysInfo::kernelType(), QSysInfo::kernelVersion(), QStringLiteral(QT_VERSION_STR));
    request.setHeader(QNetworkRequest::UserAgentHeader, useragent);
    //TODO: remove this since we won't use it
    //_netManager->post(request, heartbeat_json);

        QTC_ASSERT(!_wakaOptions->isDebug(),
                   ShowMessagePrompt(QString("Heartbeat send => %1 ").arg(QString::fromUtf8(heartbeat_json))));

    if(_wakaOptions->inStatusBar())
    {
        _heartBeatButton->setDisabled(false);
        QTimer::singleShot(200, [this]()
        {
            _heartBeatButton->setDisabled(true);
        });
    }
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
