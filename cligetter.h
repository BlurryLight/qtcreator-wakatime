#ifndef WAKATIME_INTERNAL_CLIGETTER_H
#define WAKATIME_INTERNAL_CLIGETTER_H

#include <QObject>
#include <QNetworkAccessManager>
#include "waka_constants.h"

namespace Wakatime {
namespace Internal {

// For using OSInfo
using namespace Wakatime::Constants;

class CliGetter: public QObject
{
    Q_OBJECT

    QNetworkAccessManager *_netMan;
    QSslConfiguration _sslConfig;
    OSInfo _osInfo;
public:
    CliGetter();

    const QSslConfiguration getSslConfiguration()const;
public slots:
    void startGettingAssertUrl();
    void startGettingZipDownloadUrl(QString url);
    void startDownloadingZip(QString url);
    void startUnzipping(QString file);
signals:
    void doneGettingAssetsUrl(QString url);
    void doneGettingZipDownloadUrl(QString url);
    void doneDownloadingZip(QString file);
    void promptMessage(QString url);
    void doneSettingWakaTimeCli(QString location);
};

} // namespace Internal
} // namespace Wakatime

#endif // WAKATIME_INTERNAL_CLIGETTER_H
