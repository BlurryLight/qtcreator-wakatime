#include "cligetter.h"
#include <QJsonDocument>
#include <QNetworkReply>
#include <coreplugin/coreplugin.h>
#include <coreplugin/messagemanager.h>

namespace Wakatime {
namespace Internal {

CliGetter::CliGetter(QObject *parent,
                     QNetworkAccessManager *networkMan)
    :QObject(parent),_netMan(networkMan){

    connect(this,&CliGetter::doneGettingAssetsUrl,
            this,&CliGetter::startGettingZipDownloadUrl);
}


const QSslConfiguration CliGetter::getSslConfiguration()const{
    qDebug()<<"SSL support"<<QSslSocket::supportsSsl();
    qDebug()<<"SSL Build version"<<QSslSocket::sslLibraryBuildVersionString();
    auto sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_3);
    return sslConfig;
}

void CliGetter::startGettingZipDownloadUrl(QString url){
    auto req = QNetworkRequest(url);
    req.setSslConfiguration(getSslConfiguration());
    auto reply = _netMan->get(req);
    reply->connect(reply,&QNetworkReply::finished,[cli=this,reply]()
    {
        auto jsonDoc = QJsonDocument::fromJson(reply->readAll());
        qDebug()<<"Goten: "<<jsonDoc;
    });
    qDebug()<<"WE GOT HERE";
}

void CliGetter::startGettingAssertUrl(){
    auto request = QNetworkRequest(Wakatime::Constants::WAKATIME_RELEASE_URL);
    request.setSslConfiguration(getSslConfiguration());
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
            Core::MessageManager::writeDisrupting(msg);
        }
    });
}

} // namespace Internal
} // namespace Wakatime
