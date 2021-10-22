#include "cligetter.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <coreplugin/coreplugin.h>
#include <coreplugin/messagemanager.h>
#include <waka_plugin.h>

namespace Wakatime {
namespace Internal {

CliGetter::CliGetter(const OSInfo &_info):_osInfo(_info){
    _netMan =  new QNetworkAccessManager(this);
    _sslConfig = QSslConfiguration::defaultConfiguration();
    _sslConfig.setProtocol(QSsl::TlsV1_3);

    connect(this,&CliGetter::doneGettingAssetsUrl,
            this,&CliGetter::startGettingZipDownloadUrl);
    connect(this,&CliGetter::doneGettingZipDownloadUrl,
            this,&CliGetter::startDownloadingZip);
}

void CliGetter::startDownloadingZip(QString url){
    QString msg = "URL_DOWNLOAD: "+url;
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
            if(cli->_osInfo._os==OSType::WINDOWS){
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
    QSslSocket::supportsSsl()?emit promptMessage("SSL support exists"):
                              emit promptMessage("SSL support not exists");
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
}

} // namespace Internal
} // namespace Wakatime
