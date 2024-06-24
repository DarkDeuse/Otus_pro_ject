#include "WebApiAuthentication.h"
#include "WebApiAuthParams.h"
#include "api/WebApiErrors.h"
#include "api/WebApiURLs.h"
#include "api/doc/WebApiDocument.h"
#include "api/doc/obj/WebApiRawObj.h"
#include "api/doc/obj/auth/WebApiObjAuthLoginParams.h"
#include "api/doc/obj/auth/WebApiObjAuthToken.h"
#include "/database/Database.h"
#include "/database/DatabaseInfo.h"
#include "/user/User.h"
#include "db/simplecrypt.h"
#include <gcrypt.h>
#include <QSettings>
#include <QUuid>

#include "jwt.h"
// честно признаюсь, адаптировал один из файлов аутентификации десктопной версии при помощи гугла
#define CRYPT_KEY Q_UINT64_C(0x380cf77a7abf61f1)

WebApiAuthentication::WebApiAuthentication()  {

    QSettings set;
    set.beginGroup("WebApi");
    QByteArray keyProt = set.value("Key").toByteArray();
    if (!keyProt.isEmpty()) {
        SimpleCrypt crypto(CRYPT_KEY);
        m_key = crypto.decryptToByteArray(keyProt);
    }

    if (m_key.isEmpty() || (m_key.size() < 32) || (m_key.size() > 64)) {
        m_key.resize(32);
        gcry_randomize(m_key.data(), m_key.length(), GCRY_STRONG_RANDOM);

        SimpleCrypt crypto(CRYPT_KEY);
        set.setValue("Key", crypto.encryptToByteArray(m_key));
    }
    set.endGroup();
}

QString WebApiAuthentication::issuer() const {
    return  "user";
}

bool WebApiAuthentication::login(const Database &db, const WebApiDocument &reqDoc, WebApiDocument &respDoc) {
    bool ok = false;
    User *fnd_user = Q_NULLPTR;
    for (int idx = 0; (idx < reqDoc.dataObjCount()) && !fnd_user; idx++) {
        const WebApiRawObj &rawObj = reqDoc.dataObj(idx);
        if (rawObj.objRef().isSameType(WebApiObjAuthLoginParams::objType())) {
            WebApiObjAuthLoginParams params;
            params.load(rawObj.jsonObject());

            User *user = db.activeUser(params.login());
            if (user) {
                if (user->checkPassword(params.password())) {
                    fnd_user = user;
                }
            }
        }
    }

    if (!fnd_user) {
        respDoc.addError(WebApiErrors::authFailed());
    } else {

        QString id = QUuid::createUuid().toString();
        QDateTime curTime = QDateTime::currentDateTimeUtc();
        QDateTime expTime = curTime.addDays(1);

        jwt_t *jwt = Q_NULLPTR;
        int res = jwt_new(&jwt);
        if (res == 0)
            res = jwt_add_grant(jwt, "iss", issuer().toLatin1().data());
        if (res == 0) {
            res = jwt_add_grant(jwt, "sub", fnd_user->login().toLatin1().data());
        }
        if (res == 0) {
            res = jwt_add_grant(jwt, "jti", id.toLatin1().data());
        }
        if (res == 0) {
            res = jwt_add_grant(jwt, "db.tech.ru", db.info()->uuid().toLatin1().data());
        }
        if (res == 0) {
            res = jwt_add_grant(jwt, "exp", QString::number(expTime.toMSecsSinceEpoch()/1000).toLatin1().data());
            if (res == 0)
                jwt_add_grant(jwt, "iat", QString::number(curTime.toMSecsSinceEpoch()/1000).toLatin1().data());
        }
        if (res == 0) {
            res = jwt_set_alg(jwt, JWT_ALG_HS256,
                              reinterpret_cast<const unsigned char*>(m_key.data()), m_key.size());
        }
        if (res == 0) {
            char *tokenStr = jwt_encode_str(jwt);
            WebApiObjAuthToken tokenObj(QString::fromLatin1(tokenStr), id, expTime);
            respDoc.addDataObj(tokenObj);
#ifdef Q_OS_WIN32
            jwt_free_str(tokenStr);
#else
            free(tokenStr);
#endif
            ok = true;
        } else {
            respDoc.addError(WebApiErrors::authTokenCreate());
        }

        jwt_free(jwt);
    }

    return ok;
}

WebApiError WebApiAuthentication::check(const Database &db, const WebApiAuthParams &params, const User * &resUser) {
    WebApiError err;
    if (params.scheme().isEmpty()) {
        err = WebApiErrors::authReqired();
    } else if (params.scheme().compare(WebApiURLs::authScheme(), Qt::CaseInsensitive) != 0) {
        err = WebApiErrors::authMethodNotSupported();
    } else {
        QString token = params.data();
        jwt_t *jwt = Q_NULLPTR;
        int ret = jwt_decode(&jwt, token.toLatin1().data(),
                             reinterpret_cast<const unsigned char*>(m_key.data()), m_key.size());
        if (ret != 0) {
            err = WebApiErrors::authInvalidToken();
        } else {
            const char *issuerStr = jwt_get_grant(jwt, "iss");
            const char *subjectStr = jwt_get_grant(jwt, "sub");
            const char *idStr = jwt_get_grant(jwt, "jti");
            const char *expirationTimeStr = jwt_get_grant(jwt, "exp");
            const char *createTimeStr = jwt_get_grant(jwt, "iat");
            const char *dbIdStr  = jwt_get_grant(jwt, "db.tech.ru");


            if (!issuerStr || !subjectStr || !idStr || !expirationTimeStr || !createTimeStr || !dbIdStr) {
                err = WebApiErrors::authInvalidToken();
            } else {
                if (QString(issuerStr) != issuer()) {
                    err = WebApiErrors::authInvalidToken();
                } else {
                    QDateTime expirationTime = QDateTime::fromMSecsSinceEpoch(QString(expirationTimeStr).toLongLong() * 1000);
                    QDateTime creationTime = QDateTime::fromMSecsSinceEpoch(QString(createTimeStr).toLongLong() * 1000);

                    if (QDateTime::currentDateTimeUtc().secsTo(expirationTime) < -time_sec_guard) {
                        err = WebApiErrors::authTokenExpired();
                    } else if (creationTime.secsTo(QDateTime::currentDateTimeUtc()) < -time_sec_guard) {
                        err = WebApiErrors::authInvalidToken();
                    }
                }
            }



            if (err.isSuccess()) {
                QString login = QString(subjectStr);
                QString db_uuid = QString(dbIdStr);

                if (db_uuid != db.info()->uuid()) {
                    err = WebApiErrors::authInvalidDb();
                } else {
                    User *user = db.activeUser(login);
                    if (!user) {
                        err = WebApiErrors::authFailed();
                    } else {
                        resUser = user;
                    }
                }
            }
        }


        jwt_free(jwt);
    }

    return err;
}
