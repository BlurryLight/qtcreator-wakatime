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
    CliGetter(const OSInfo &info);

    const QSslConfiguration getSslConfiguration()const;
public slots:
    void startGettingAssertUrl();
    void startGettingZipDownloadUrl(QString url);
    void startDownloadingZip(QString url);
signals:
    void doneGettingAssetsUrl(QString &url);
    void doneGettingZipDownloadUrl(QString &url);
    void promptMessage(QString url);
};

} // namespace Internal
} // namespace Wakatime

#endif // WAKATIME_INTERNAL_CLIGETTER_H
