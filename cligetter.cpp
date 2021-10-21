#include "cligetter.h"
#include <QJsonDocument>
#include <QNetworkReply>
#include <coreplugin/coreplugin.h>
#include <coreplugin/messagemanager.h>

namespace Wakatime {
namespace Internal {

CliGetter::CliGetter()
    {
    _netMan =  new QNetworkAccessManager(this);
    qDebug()<<"SSL support"<<QSslSocket::supportsSsl();
    _sslConfig = QSslConfiguration::defaultConfiguration();
    _sslConfig.setProtocol(QSsl::TlsV1_3);

    connect(this,&CliGetter::doneGettingAssetsUrl,
            this,&CliGetter::startGettingZipDownloadUrl);
}


void CliGetter::startGettingZipDownloadUrl(QString url){
    auto req = QNetworkRequest(url);
    req.setSslConfiguration(_sslConfig);
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
            Core::MessageManager::writeDisrupting(msg);
        }
    });
}

} // namespace Internal
} // namespace Wakatime
