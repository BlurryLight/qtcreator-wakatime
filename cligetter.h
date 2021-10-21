#ifndef WAKATIME_INTERNAL_CLIGETTER_H
#define WAKATIME_INTERNAL_CLIGETTER_H

#include <QObject>
#include <QNetworkAccessManager>
#include "waka_constants.h"

namespace Wakatime {
namespace Internal {

class CliGetter: public QObject
{
    Q_OBJECT

    QNetworkAccessManager *_netMan;
    QSslConfiguration _sslConfig;
public:
    CliGetter();

    const QSslConfiguration getSslConfiguration()const;
public slots:
    void startGettingAssertUrl();
    void startGettingZipDownloadUrl(QString url);
signals:
    void doneGettingAssetsUrl(QString &url);
    void doneGettingZipDownloadUrl(QString &url);
};

} // namespace Internal
} // namespace Wakatime

#endif // WAKATIME_INTERNAL_CLIGETTER_H
