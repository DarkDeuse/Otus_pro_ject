#ifndef WEBAPIREQUEST_H
#define WEBAPIREQUEST_H

#include <QUrl>
#include <QHash>
#include <QString>
#include "WebApiAuthParams.h"
#include "WebApiRequestOriginInfo.h"
class WebApiRequestMethod;

class WebApiRequest {
public:
    WebApiRequest() : method(Q_NULLPTR) {}

    QUrl url;
    QHash<QString, QString> args;
    const WebApiRequestMethod *method;
    WebApiAuthParams auth;
    WebApiRequestOriginInfo origin;
    QByteArray data;
};

#endif
