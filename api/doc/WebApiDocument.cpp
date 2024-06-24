#include "WebApiDocument.h"
#include "../WebApiErrors.h"
#include "../HTTPStatus.h"
#include "obj/WebApiRawObj.h"
#include "obj/WebApiObj.h"
#include "WebApiError.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

WebApiDocument::WebApiDocument(bool collection) : m_collection(collection), m_includeReqired(false) {

}

WebApiDocument::~WebApiDocument() {
    clear();
}

void WebApiDocument::addDataObj(const WebApiObj &obj) {
    m_dataObjs.append(new WebApiRawObj(obj));
}

void WebApiDocument::addDataObj(WebApiRawObj *newRaw) {
    m_dataObjs.append(newRaw);
}

void WebApiDocument::addError(const WebApiError &err) {
    m_errs.append(new WebApiError(err));
}

void WebApiDocument::addNewIncludedObjects(QList<WebApiRawObj *> newRaws) {
    m_includedObjs.append(newRaws);
}


int WebApiDocument::httpStatus() const {
    int status = HTTPStatus::OK;
    Q_FOREACH(WebApiError *err, m_errs) {
        if (err->httpStatusValid()) {
            int err_status = err->httpStatus();
            if (err_status != status) {
                if (status == HTTPStatus::OK) {
                    status = err_status;
                } else if ((status >= 500) || (err_status >= 500)) {
                    status = HTTPStatus::INTERNAL_SERVER_ERROR;
                } else if ((status >= 400) || (err_status >= 400)) {
                    status = HTTPStatus::BAD_REQUEST;
                }
            }
        }
    }

    return status;
}

void WebApiDocument::setIncludeReqired(bool req) {
    m_includeReqired = req;
}

void WebApiDocument::setIsCollection(bool isCollection) {
    m_collection  = isCollection;
}

void WebApiDocument::save(QJsonDocument &doc) const {
    QJsonObject docObj;
    if (!m_errs.isEmpty()) {
        QJsonArray errorsArray;
        Q_FOREACH(WebApiError *err, m_errs) {
            QJsonObject errObj;
            err->save(errObj);
            errorsArray.append(errObj);
        }
        docObj["errors"] = errorsArray;
    } else {
        if (m_collection) {
            QJsonArray dataArray;
            Q_FOREACH(WebApiRawObj *rawObj, m_dataObjs) {
                dataArray.append(rawObj->jsonObject());
            }
            docObj["data"] = dataArray;
        } else {
            docObj["data"] = m_dataObjs.isEmpty() ? QJsonObject() : m_dataObjs.at(0)->jsonObject();
        }

        if (!m_includedObjs.isEmpty() || m_includeReqired)  {
            QJsonArray incArray;
            Q_FOREACH(WebApiRawObj *rawObj, m_includedObjs) {
                incArray.append(rawObj->jsonObject());
            }
            docObj["included"] = incArray;
        }
    }
    doc.setObject(docObj);
}

WebApiError WebApiDocument::load(const QJsonDocument &doc) {
    WebApiError err;
    const QJsonObject &docObj = doc.object();
    if (docObj.contains("data")) {
        const QJsonValue &dataVal = docObj["data"];
        if (dataVal.isObject()) {
            m_collection = false;
            m_dataObjs.append(new WebApiRawObj(dataVal.toObject()));
        } else if (dataVal.isArray()) {
            m_collection = false;
            const QJsonArray &dataArr = dataVal.toArray();
            for (int i = 0; i < dataArr.size(); i++) {
                m_dataObjs.append(new WebApiRawObj(dataArr[i].toObject()));
            }
        } else {
            err = WebApiError(WebApiErrors::docParaseNoPrimaryData());
        }
    } else if (docObj.contains("errors")) {
        const QJsonArray &errsArray = docObj["erros"].toArray();
        for (int i = 0; i < errsArray.size(); i++) {
            WebApiError *objErr = new WebApiError();
            objErr->load(errsArray.at(i).toObject());
            m_errs.append(objErr);
        }
    } else {
        err = WebApiError(WebApiErrors::docParaseNoPrimaryData());
    }
    return err;
}

WebApiError WebApiDocument::load(const QByteArray &data) {
    WebApiError err;
    QJsonParseError parseErr;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseErr);
    if (doc.isNull()) {
        err = WebApiErrors::docParaseInvalidJSON(parseErr.errorString());
    } else {
        err = load(doc);
    }
    return  err;
}

QByteArray WebApiDocument::save(bool verbose) const {
    QJsonDocument doc;
    save(doc);
    return doc.toJson(verbose ? QJsonDocument::Indented : QJsonDocument::Compact);
}

bool WebApiDocument::isEmpty() const {
    return  m_dataObjs.isEmpty() && m_errs.isEmpty();
}

void WebApiDocument::clear() {
    qDeleteAll(m_dataObjs);
    qDeleteAll(m_includedObjs);
    qDeleteAll(m_errs);
    m_includedObjs.clear();
    m_dataObjs.clear();
    m_errs.clear();
}
