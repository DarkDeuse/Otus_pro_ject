#include "WebApiRequestProcessor.h"
#include "WebApiAuthentication.h"
#include "WebApiPrjectTreeAccess.h" //данный файл я не дописал, но идея в том, чтобы создать новую иерархию и все сохранить
#include "WebApiRequestMethod.h"
#include "api/WebApiQueryParams.h"
#include "api/WebApiURLs.h"
#include "api/WebApiErrors.h"
#include "api/doc/WebApiDocument.h"
#include "WebApiRequest.h"
#include "WebApiDbAccess.h"
#include <QtDebug>

#include "microhttpd.h"

WebApiRequestProcessor::WebApiRequestProcessor(WebApiDbAccess *dbAccess) :
    m_dbAccess(dbAccess), m_auth(new WebApiAuthentication()), m_prjTree(new WebApiPrjectTreeAccess()) {

}

WebApiRequestProcessor::~WebApiRequestProcessor() {
    delete  m_auth;
}

bool WebApiRequestProcessor::isUrlAplicable(const QUrl &url) {
    return url.path().startsWith(WebApiURLs::root(), Qt::CaseInsensitive);
}

bool WebApiRequestProcessor::urlEq(QString url1, QString url2) {
    return url1.compare(url2, Qt::CaseInsensitive) == 0;
}

unsigned WebApiRequestProcessor::processHttpRequest(const WebApiRequest &req,
                                                    QByteArray &res,
                                                    QList<const WebApiRequestMethod *> &allowMethods) {
    WebApiDocument reqDoc;
    WebApiDocument resDoc;
    WebApiQueryParams qParams;



    if ((req.method == WebApiRequestMethods::put()) ||
            (req.method == WebApiRequestMethods::post()) ||
            (req.method == WebApiRequestMethods::path())) {
        WebApiError err = reqDoc.load(req.data);
        if (!err.isSuccess()) {
            resDoc.addError(err);
        }
    }

    if (!resDoc.hasErrors()) {
        if (!req.args.isEmpty()) {
            if (!qParams.parse(req.args)) {
                resDoc.addError(WebApiErrors::reqParseInvalidParams());
            }
        }
    }

    if (!resDoc.hasErrors()) {
        QString subpath = req.url.path().remove(0, WebApiURLs::root().length());
        if (subpath.endsWith("/")) {
            subpath = subpath.remove(subpath.length()-1, 1);
        }
        QSharedPointer<Database> db = m_dbAccess->getDb();
        if (!db) {
            resDoc.addError(WebApiError(WebApiErrors::noDbConnection()));
        }
        if (!resDoc.hasErrors()) {
            QStringList pathParts = subpath.split('/');
            if (pathParts.isEmpty()) {
                resDoc.addError(WebApiErrors::unknownURL());
            } else {
                QString curPath = pathParts.takeFirst();
                if (urlEq(curPath, WebApiURLs::auth())) {
                    curPath = pathParts.takeFirst();
                    if (urlEq(curPath, WebApiURLs::auth::login())) {
                        allowMethods.append(WebApiRequestMethods::post());
                        if (req.method == WebApiRequestMethods::post()) {
                            m_auth->login(*db, reqDoc, resDoc);
                        } else if (req.method != WebApiRequestMethods::options()) {
                            resDoc.addError(WebApiErrors::methodNotAllowed());
                        }
                    } else {
                        resDoc.addError(WebApiErrors::unknownURL());
                    }
                } else {
                    const User *user = Q_NULLPTR;
                    WebApiError err = m_auth->check(*db, req.auth, user);
                    if (!err.isSuccess()) {
                        resDoc.addError(err);
                    } else {
                        allowMethods.append(WebApiRequestMethods::get());
                        if (req.method == WebApiRequestMethods::get()) {
                            /* основаня обработка запроса */
                            if (urlEq(curPath, WebApiURLs::projects())) {
                                if (pathParts.isEmpty()) {
                                    m_prjTree->saveProjects(*user, *db, qParams, resDoc);
                                } else {
                                    QString prjId;
                                    if (takeObjId(pathParts, prjId, resDoc)) {
                                        Project *prj = m_prjTree->project(*user, *db, prjId);
                                        if (!prj) {
                                            resDoc.addError(WebApiErrors::resourceNotFound());
                                        } else {
                                            if (pathParts.isEmpty()) {
                                                m_prjTree->saveProject(*user, *prj, qParams, resDoc);
                                            } else {
                                                curPath = pathParts.takeFirst();
                                                if (urlEq(curPath, WebApiURLs::storeLocations())) {
                                                    if (pathParts.isEmpty()) {
                                                        m_prjTree->saveProjectStores(*user, *prj, qParams, resDoc);
                                                    } else {
                                                        QString storeId;
                                                        if (takeObjId(pathParts, storeId, resDoc)) {
                                                            saveStore(pathParts, *user, m_prjTree->store(*user, *prj, storeId), qParams, resDoc);
                                                        }
                                                    }
                                                } else {
                                                    resDoc.addError(WebApiErrors::unknownURL());
                                                }
                                            }
                                        }
                                    }
                                }
                            } else if (urlEq(curPath, WebApiURLs::storeLocations())) {
                                QString storeId;
                                if (takeObjId(pathParts, storeId, resDoc)) {
                                    saveStore(pathParts, *user, m_prjTree->store(*user, *db, storeId), qParams, resDoc);
                                }
                            } else if (urlEq(curPath, WebApiURLs::floors())) {
                                QString floorID;
                                if (takeObjId(pathParts, floorID, resDoc)) {
                                    saveFloor(pathParts, *user, m_prjTree->floor(*user, *db, floorID), qParams, resDoc);
                                }
                            } else if (urlEq(curPath, WebApiURLs::zones())) {
                                QString zoneId;
                                if (takeObjId(pathParts, zoneId, resDoc)) {
                                    saveZone(pathParts, *user, m_prjTree->zone(*user, *db, zoneId), qParams, resDoc);
                                }
                            } else if (urlEq(curPath, WebApiURLs::entrances())) {
                                QString entranceId;
                                if (takeObjId(pathParts, entranceId, resDoc)) {
                                    saveEntrance(pathParts, *user, m_prjTree->entrance(*user, *db, entranceId), qParams, resDoc);
                                }
                            } else if (urlEq(curPath, WebApiURLs::retailTypes())) {
                                if (pathParts.isEmpty()) {
                                    m_prjTree->saveRetailTypes(*user, *db, qParams, resDoc);
                                } else {
                                     QString typeId;
                                     if (takeObjId(pathParts, typeId, resDoc)) {
                                         if (pathParts.isEmpty()) {
                                             RetailType *type = m_prjTree->retailType(*user, *db, typeId);
                                             if (type) {
                                                 m_prjTree->saveRetailType(*user, *type, qParams, resDoc);
                                             } else {
                                                 resDoc.addError(WebApiErrors::resourceNotFound());
                                             }
                                         } else {
                                             resDoc.addError(WebApiErrors::unknownURL());
                                         }
                                     }
                                }
                            } else if (urlEq(curPath, WebApiURLs::zoneTypes())) {
                                if (pathParts.isEmpty()) {
                                    m_prjTree->saveZoneTypes(*user, *db, qParams, resDoc);
                                } else {
                                     QString typeId;
                                     if (takeObjId(pathParts, typeId, resDoc)) {
                                         if (pathParts.isEmpty()) {
                                             ZoneType *type = m_prjTree->zoneType(*user, *db, typeId);
                                             if (type) {
                                                 m_prjTree->saveZoneType(*user, *type, qParams, resDoc);
                                             } else {
                                                 resDoc.addError(WebApiErrors::resourceNotFound());
                                             }
                                         } else {
                                             resDoc.addError(WebApiErrors::unknownURL());
                                         }
                                     }
                                }
                            } else {
                                resDoc.addError(WebApiErrors::unknownURL());
                            }
                        } else if (req.method != WebApiRequestMethods::options()) {
                            resDoc.addError(WebApiErrors::methodNotAllowed());
                        }
                    }
                }

            }
        }

        m_dbAccess->putDb();
    }

    /* OPTIONS всегда допустим для CORS или проверки доступных методов */
    allowMethods.append(WebApiRequestMethods::options());

    if (!resDoc.isEmpty())
        res = resDoc.save(false);
    return static_cast<unsigned>(resDoc.httpStatus());
}

bool WebApiRequestProcessor::takeObjId(QStringList &path, QString &id, WebApiDocument &resDoc) {
    bool ret = false;
    if (path.isEmpty()) {
        resDoc.addError(WebApiErrors::unknownURL());
    } else {
        id = path.takeFirst();
        ret = true;
    }
    return ret;
}

void WebApiRequestProcessor::saveStore(QStringList path, const User &user, const Store *store, const WebApiQueryParams &qParams, WebApiDocument &resDoc) {
    if (!store) {
        resDoc.addError(WebApiErrors::resourceNotFound());
    } else {
        if (path.isEmpty()) {
            m_prjTree->saveStore(user, *store, qParams, resDoc);
        } else {
            QString curPath = path.takeFirst();
            if (urlEq(curPath, WebApiURLs::zones())) {
                QString zoneId;
                if (takeObjId(path, zoneId, resDoc)) {
                    saveZone(path, user, m_prjTree->zone(user, *store, zoneId), qParams, resDoc);
                }
            } else if (urlEq(curPath, WebApiURLs::entrances())) {
                QString entranceID;
                if (takeObjId(path, entranceID, resDoc)) {
                    saveEntrance(path, user, m_prjTree->entrance(user, *store, entranceID), qParams, resDoc);
                }
            } else {
                resDoc.addError(WebApiErrors::unknownURL());
            }
        }
    }
}

void WebApiRequestProcessor::saveFloor(QStringList path, const User &user, const Floor *floor, const WebApiQueryParams &qParams, WebApiDocument &resDoc) {
    if (!floor) {
        resDoc.addError(WebApiErrors::resourceNotFound());
    } else {
        if (path.isEmpty()) {
            m_prjTree->saveFloor(user, *floor, qParams, resDoc);
        } else {
            resDoc.addError(WebApiErrors::unknownURL());
        }
    }
}

void WebApiRequestProcessor::saveZone(QStringList path, const User &user, const Zone *zone, const WebApiQueryParams &qParams, WebApiDocument &resDoc) {
    if (!zone) {
        resDoc.addError(WebApiErrors::resourceNotFound());
    } else {
        if (path.isEmpty()) {
            m_prjTree->saveZone(user, *zone, qParams, resDoc);
        } else {
            QString curPath = path.takeFirst();
            if (urlEq(curPath, WebApiURLs::zones())) {
                QString zoneId;
                if (takeObjId(path, zoneId, resDoc)) {
                    saveZone(path, user, m_prjTree->zone(user, *zone, zoneId), qParams, resDoc);
                }
            } else {
                resDoc.addError(WebApiErrors::unknownURL());
            }
        }
    }

}

void WebApiRequestProcessor::saveEntrance(QStringList path, const User &user, const Entrance *entrance, const WebApiQueryParams &qParams, WebApiDocument &resDoc) {
    if (!entrance) {
        resDoc.addError(WebApiErrors::resourceNotFound());
    } else {
        if (path.isEmpty()) {
            m_prjTree->saveEntrance(user, *entrance, qParams, resDoc);
        } else {
            QString curPath = path.takeFirst();
            if (urlEq(curPath, WebApiURLs::entrances())) {
                QString entranceId;
                if (takeObjId(path, entranceId, resDoc)) {
                    saveEntrance(path, user, m_prjTree->entrance(user, *entrance, entranceId), qParams, resDoc);
                }
            } else {
                resDoc.addError(WebApiErrors::unknownURL());
            }
        }
    }
}


