#ifndef WEBAPIAUTHENTICATION_H
#define WEBAPIAUTHENTICATION_H

#include "webapi/api/doc/WebApiError.h"

class WebApiAuthParams;
class WebApiDbAccess;
class WebApiDocument;
class Database;
class User;

class WebApiAuthentication {
public:
    WebApiAuthentication();

    QString issuer() const;


    bool login(const Database &db, const WebApiDocument &reqDoc, WebApiDocument &respDoc);
    WebApiError check(const Database &db, const WebApiAuthParams &params, const User *&resUser);
private:
    static const int time_sec_guard = 15 *60;
    QByteArray m_key;
};

#endif
