#include "cligetter.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <coreplugin/coreplugin.h>
#include <coreplugin/messagemanager.h>
#include <waka_plugin.h>
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#include <QDir>
#include <QDirIterator>

namespace Wakatime {
namespace Internal {

CliGetter::CliGetter(){
    _netMan =  new QNetworkAccessManager(this);
    _sslConfig = QSslConfiguration::defaultConfiguration();
    _sslConfig.setProtocol(QSsl::TlsV1_3);

    connect(this,&CliGetter::doneGettingAssetsUrl,
            this,&CliGetter::startGettingZipDownloadUrl);
    connect(this,&CliGetter::doneGettingZipDownloadUrl,
            this,&CliGetter::startDownloadingZip);
    connect(this,&CliGetter::doneDownloadingZip,
            this,&CliGetter::startUnzipping);
}

void CliGetter::startHearBeat(QString file){
    if(_wakaCliExecutablePath.isEmpty()){
        //get the executable path
        auto dir = WakaPlugin::getWakaCLILocation();
        auto iterator = QDirIterator(dir.absolutePath());
        while(iterator.hasNext()){
            auto f = iterator.next();
            if(iterator.fileInfo().isFile()){
                _wakaCliExecutablePath=f;
            }
        }
    }

    QString exec (_wakaCliExecutablePath+" --plugin QtCreator-wakatime/"+WAKATIME_PLUGIN_VERSION
                  +" --entity "+file);
    //run the hearbeat here
    system(exec.toStdString().c_str());
}

void CliGetter::startUnzipping(QString location){
    QString fileExists("File exists @: ");
    fileExists+=location;
    emit promptMessage(fileExists);

    QuaZip zip(location);
    if(!zip.open(QuaZip::Mode::mdUnzip)){
        emit promptMessage("Error, couldn't read zip file");
    }

    //create directory where to store unzipped files in
    auto waka_extracted_dir = WakaPlugin::getWakaCLILocation();
    if(!waka_extracted_dir.exists()){
        waka_extracted_dir.mkpath(waka_extracted_dir.path());
    }


    QString msg("Starting Extracting files");
    emit promptMessage(msg);

    QuaZipFile file(&zip);
    for(bool success=zip.goToFirstFile();success;success=zip.goToNextFile()){
        //get file name
        QuaZipFileInfo fileinfo;
        file.getFileInfo(&fileinfo);
        QFile f(waka_extracted_dir.path()+QDir::separator()+fileinfo.name);
        f.open(QIODevice::WriteOnly);

        file.open(QIODevice::ReadOnly);
        f.write(file.readAll());
        f.setPermissions(QFile::Permission::ExeUser | QFile::Permission::ReadUser | QFile::Permission::WriteUser);

        file.close();
        f.flush();
        f.close();
    }

    msg="Done Extracting files";
    emit promptMessage(msg);

    zip.close();
    //delete the zipped file
    QFile zipFile(location);
    zipFile.remove();

    emit doneSettingWakaTimeCli();
}

void CliGetter::startDownloadingZip(QString url){
    QString msg = "WAKATIME URL_DOWNLOAD: "+url;
    QString random_number=QString::fromStdString(std::to_string(rand()));
    QString tempDir = QDir::tempPath();
    QString wakatime_cli_zip = tempDir+QDir::separator()+"/wakatime-cli"+random_number+".zip";

    auto req = QNetworkRequest(url);
    req.setSslConfiguration(_sslConfig);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,true);
    auto reply = _netMan->get(req);

    reply->connect(reply,&QNetworkReply::finished,[cli=this,wakatime_cli_zip,reply](){
        QFile wakafile(wakatime_cli_zip);
        if(wakafile.open(QFile::WriteOnly)){
            wakafile.write(reply->readAll());
            wakafile.flush();
            wakafile.close();

            emit cli->doneDownloadingZip(wakatime_cli_zip);
        }
    });
    emit promptMessage(msg);
}


void CliGetter::startGettingZipDownloadUrl(QString url){
    auto req = QNetworkRequest(url);
    req.setSslConfiguration(_sslConfig);
    auto reply = _netMan->get(req);
    reply->connect(reply,&QNetworkReply::finished,[cli=this,reply]()
    {
        auto jsonDoc = QJsonDocument::fromJson(reply->readAll());
        //parse all download links for one that match OSInfo object
        QJsonArray arr=jsonDoc.array();
        for(const QJsonValue &val:arr){
            QString downloadUrl = val["browser_download_url"].toString();
            //check os
            if(cli->_osInfo._os==0){//fix for windows, since fails in github actions
                //only has 64bit and 32bit
                if(cli->_osInfo._arch==OSArch::AMD64){
                    if(downloadUrl.contains("windows-amd64")){
                        emit cli->doneGettingZipDownloadUrl(downloadUrl);
                    }
                }else if(cli->_osInfo._arch==OSArch::I386){
                    if(downloadUrl.contains("windows-386")){
                        emit cli->doneGettingZipDownloadUrl(downloadUrl);
                    }
                }
            }else if(cli->_osInfo._os==OSType::LINUX){
                // only has amd64, arm64, i386, and arm
                if(cli->_osInfo._arch==OSArch::AMD64){
                    if(downloadUrl.contains("linux-amd64")){
                        emit cli->doneGettingZipDownloadUrl(downloadUrl);
                    }
                }else if(cli->_osInfo._arch==OSArch::ARM64){
                    if(downloadUrl.contains("linux-arm64")){
                        emit cli->doneGettingZipDownloadUrl(downloadUrl);
                    }
                }else if(cli->_osInfo._arch==OSArch::ARM){
                    if(downloadUrl.contains("linux-arm")){
                        emit cli->doneGettingZipDownloadUrl(downloadUrl);
                    }
                }else if(cli->_osInfo._arch==OSArch::I386){
                    if(downloadUrl.contains("linux-386")){
                        emit cli->doneGettingZipDownloadUrl(downloadUrl);
                    }
                }
            }else if(cli->_osInfo._os==OSType::MACOS){
                //only has amd64 and arm64
                if(cli->_osInfo._arch==OSArch::AMD64){
                    if(downloadUrl.contains("darwin-amd64")){
                        emit cli->doneGettingZipDownloadUrl(downloadUrl);
                    }
                }else if(cli->_osInfo._arch==OSArch::ARM64){
                    if(downloadUrl.contains("darwin-arm64")){
                        emit cli->doneGettingZipDownloadUrl(downloadUrl);
                    }
                }
            }
        }
    });
}

void CliGetter::startGettingAssertUrl(){
    // dummy in case OS is unsupported
    _osInfo = OSInfo{OSType::UNKOWN,OSArch::AMD64};
    //get architecture of OS
    std::string arch = QSysInfo::buildCpuArchitecture().toStdString();
#ifdef Q_OS_WINDOWS

    _osInfo._os = 0;
    if(arch == "x86_64"){
        //_osInfo = OSInfo{0, OSArch::AMD64};//enum problems with msvc github actions
        _osInfo._arch = OSArch::AMD64;
    }else if(arch == "i386"){
        //_osInfo = OSInfo{0, OSArch::I386};//enum problems with msvc github actions
		_osInfo._arch = OSArch::I386;
    }
#endif
#ifdef Q_OS_LINUX
    if(arch == "x86_64"){
        _osInfo = OSInfo{OSType::LINUX, OSArch::AMD64};
    }else if(arch == "i386"){
        _osInfo = OSInfo{OSType::LINUX, OSArch::I386};
    }else if(arch == "arm"){
        _osInfo = OSInfo{OSType::LINUX, OSArch::ARM};
    }else if(arch == "arm64"){
        _osInfo = OSInfo{OSType::LINUX, OSArch::ARM64};
    }
#endif
#ifdef Q_OS_DARWIN
    if(arch == "x86_64"){
        _osInfo = OSInfo{OSType::MACOS, OSArch::AMD64};
    }else if(arch == "arm64"){
        _osInfo = OSInfo{OSType::MACOS, OSArch::ARM64};
    }
#endif

    auto request = QNetworkRequest(Wakatime::Constants::WAKATIME_RELEASE_URL);
    request.setSslConfiguration(_sslConfig);
    auto reply = _netMan->get(request);
    connect(reply,&QNetworkReply::finished,[cli=this,reply](){
        if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isValid()){
            auto jsonDoc = QJsonDocument::fromJson(reply->readAll());
            auto assert_url = jsonDoc["assets_url"].toString();
            emit cli->doneGettingAssetsUrl(assert_url);
        }else{
            //if we reach here means there was an error
            QString msg = "Sorry, couldn't connect to ";
            msg += reply->url().toString();
            WakaPlugin::ShowMessagePrompt(msg);
        }
    });
    QSslSocket::supportsSsl()?emit promptMessage("SSL support exists"):
                              emit promptMessage("SSL support not exists");return;
}

} // namespace Internal
} // namespace Wakatime
