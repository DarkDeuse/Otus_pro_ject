#ifndef WEBAPIREQUESTPROCESSOR_H
#define WEBAPIREQUESTPROCESSOR_H

#include <QUrl>
#include <QString>
#include <QHash>
class WebApiAuthParams;
class WebApiDbAccess;
class WebApiAuthentication;
class WebApiPrjectTreeAccess;

class DbElement;
class User;
class Store;
class Floor;
class Zone;
class Entrance;
class ZoneEntranceRef;
class WebApiQueryParams;
class WebApiDocument;
class WebApiRequest;
class WebApiRequestMethod;




class WebApiRequestProcessor {
public:



    WebApiRequestProcessor(WebApiDbAccess *dbAccess);
    ~WebApiRequestProcessor();

    bool isUrlAplicable(const QUrl &url);
    bool urlEq(QString url1, QString url2);


    unsigned processHttpRequest(const WebApiRequest &req, QByteArray &res, QList<const WebApiRequestMethod *> &allowMethods);

private:
    bool takeObjId(QStringList &path, QString &id, WebApiDocument &resDoc);
    void saveStore(QStringList path, const User &user, const Store *store, const WebApiQueryParams &qParams, WebApiDocument &resDoc);
    void saveFloor(QStringList path, const User &user, const Floor *floor, const WebApiQueryParams &qParams, WebApiDocument &resDoc);
    void saveZone(QStringList path, const User &user, const Zone *zone, const WebApiQueryParams &qParams, WebApiDocument &resDoc);
    void saveEntrance(QStringList path, const User &user, const Entrance *entrance, const WebApiQueryParams &qParams, WebApiDocument &resDoc);
    void saveEntranceRef(QStringList path, const User &user, const ZoneEntranceRef *entranceRef, const WebApiQueryParams &qParams, WebApiDocument &resDoc);


    WebApiDbAccess *m_dbAccess;
    WebApiAuthentication *m_auth;
    WebApiPrjectTreeAccess *m_prjTree;
};

#endif
