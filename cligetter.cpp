#include "cligetter.h"
#include <QJsonDocument>
#include <QNetworkReply>
#include <coreplugin/coreplugin.h>
#include <coreplugin/messagemanager.h>

namespace Wakatime {
namespace Internal {

CliGetter::CliGetter(QObject *parent,
                     QNetworkAccessManager *networkMan)
    :QObject(parent),_netMan(networkMan){}


const QSslConfiguration CliGetter::getSslConfiguration()const{
    auto sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_3);
    return sslConfig;
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

            //disconnect signal with this lambda
            //networkManager->disconnect(networkManager,&QNetworkAccessManager::finished,nullptr,nullptr);

            //create new connection
            //networkManager->connect(networkManager,&QNetworkAccessManager::finished,[&plugin](QNetworkReply *reply){
            //   if(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).isValid()){
            //       auto jsonDoc = QJsonDocument::fromJson(reply->readAll());
            //       qDebug()<<"GOTEN: "<<jsonDoc;
            //   }
            //});
            //then get zip for download urls
            //auto request = QNetworkRequest(jsonDoc["assets_url"].toString());
            //request.setSslConfiguration(getSslConfiguration());
            qDebug()<<"WE GOT HERE";
            //networkManager->get(request);
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
