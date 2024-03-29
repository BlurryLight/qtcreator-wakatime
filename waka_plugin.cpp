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
   Core::MessageManager::writeDisrupting(QString(str));
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

    _cliGettingThread->start();

    //Heartbeat sending signal slot combo
    connect(this,&WakaPlugin::sendHeartBeat,
            _cliGetter,&CliGetter::startHearBeat);
    //for showing prompts
    connect(_cliGetter,&CliGetter::promptMessage,this,&ShowMessagePrompt);

    //check if has wakatime-cli in path
    //if not then try download it based of the users operating system
    if(!checkIfWakaCLIExist()){
        _cliGetter->connect(_cliGettingThread,&QThread::started,
                            _cliGetter,&CliGetter::startGettingAssertUrl);
        connect(_cliGetter,&CliGetter::doneSettingWakaTimeCli,
                [plugin = this](){
            plugin->_cliIsSetup=true;
            emit plugin->doneGettingCliAndSettingItUp();
        });
    }else{
        emit this->doneGettingCliAndSettingItUp();
    }
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

    connect(_wakaOptions.data(), &WakaOptions::inStatusBarChanged,
            this, &WakaPlugin::onInStatusBarChanged);

    connect(Core::EditorManager::instance(), &Core::EditorManager::aboutToSave,
            this, &WakaPlugin::onAboutToSave);
    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorAboutToChange,
            this, &WakaPlugin::onEditorAboutToChange);
    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged,
            this, &WakaPlugin::onEditorChanged);

    onInStatusBarChanged();

    QTC_ASSERT(!_wakaOptions->isDebug(),ShowMessagePrompt("Waka plugin initialized!"));
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
  return SynchronousShutdown;
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

    disconnect(TextEditor::TextEditorWidget::currentTextEditorWidget(),
               &TextEditor::TextEditorWidget::textChanged,
               this, &WakaPlugin::onEditorStateChanged);
}

void WakaPlugin::onEditorChanged(Core::IEditor *editor)
{
    if(!editor)
        return;

    connect(TextEditor::TextEditorWidget::currentTextEditorWidget(), &TextEditor::TextEditorWidget::textChanged, this, &WakaPlugin::onEditorStateChanged);
    emit this->sendHeartBeat(editor->document()->filePath().toString());
}

void WakaPlugin::onAboutToSave(Core::IDocument *document)
{
    //emit signal for sending here.
    emit sendHeartBeat(document->filePath().toString());
}

void WakaPlugin::onEditorStateChanged()
{
    emit sendHeartBeat(Core::EditorManager::currentDocument()->filePath().toString());
}

} // namespace Internal
} // namespace QtCreatorWakatime
