#ifndef WEBAPIDOCUMENT_H
#define WEBAPIDOCUMENT_H

#include <QList>
#include "WebApiError.h"
class WebApiRawObj;
class QJsonDocument;
class QJsonObject;
class WebApiObj;

class WebApiDocument {
public:
    WebApiDocument(bool collection = false);
    ~WebApiDocument();



    void addDataObj(const WebApiObj &obj);
    void addDataObj(WebApiRawObj *newRaw);
    void addError(const WebApiError &err);
    void addNewIncludedObjects(QList<WebApiRawObj *> newRaws);

    const WebApiRawObj &dataObj(int idx = 0) const {return  *m_dataObjs.at(idx);}
    int dataObjCount() const {return m_dataObjs.size();}



    const WebApiError &error(int idx = 0) const {return *m_errs.at(idx);}
    int errorsCount() const {return  m_errs.size();}
    bool hasErrors() const{return !m_errs.isEmpty();}
    int httpStatus() const;

    void setIncludeReqired(bool req);
    void setIsCollection(bool isCollection);

    void save(QJsonDocument &doc) const;
    WebApiError load(const QJsonDocument &doc);

    WebApiError load(const QByteArray &data);
    QByteArray save(bool verbose) const;

    bool isEmpty() const;
private:
    void clear();
    bool m_collection;
    bool m_includeReqired;
    QList<WebApiRawObj *> m_dataObjs;
    QList<WebApiRawObj *> m_includedObjs;
    QList<WebApiError *> m_errs;
};

#endif
