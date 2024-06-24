#include "WebApiRelationship.h"
#include <QJsonObject>
#include <QJsonArray>

WebApiRelationship::WebApiRelationship(QString name, bool many, QString parentURL) :
    m_name(name), m_many(many) {

    init(parentURL);
}

WebApiRelationship::WebApiRelationship(QString name, const WebApiObjRef &ref, QString parentURL) :
    m_name(name), m_many(false) {

    init(parentURL);

    m_refs.append(ref);
}

WebApiRelationship::WebApiRelationship(QString name, QList<WebApiObjRef> refs, QString parentURL) :
    m_name(name), m_many(true), m_refs(refs) {

    init(parentURL);
}

void WebApiRelationship::addRef(const WebApiObjRef &ref) {
    m_refs.append(ref);
}

void WebApiRelationship::save(QJsonObject &relationsObj) const {
    QJsonObject relObj;
    if (m_many) {
        QJsonArray dataArray;
        Q_FOREACH(const WebApiObjRef &ref, m_refs) {
            QJsonObject refObj;
            ref.save(refObj);
            dataArray.append(refObj);
        }
        relObj["data"] = dataArray;
    } else if (!m_refs.isEmpty()) {
        QJsonObject dataObj;
        m_refs.first().save(dataObj);
        relObj["data"] = dataObj;
    } else {
        relObj["data"] = QJsonValue(QJsonValue::Null);
    }
    relationsObj[m_name] = relObj;
    m_links.save(relationsObj);
}

void WebApiRelationship::setParentBasedLinks(QString parent) {
    setSelfLink(QString("%1/relationships/%2").arg(parent).arg(m_name));
    setRelatedLink(QString("%1/%2").arg(parent).arg(m_name));
}

void WebApiRelationship::setSelfLink(QString url) {
    m_links.addLink(WebApiLink("self", url));
}

void WebApiRelationship::setRelatedLink(QString url) {
    m_links.addLink(WebApiLink("related", url));
}

void WebApiRelationship::init(QString parentURL) {
    if (!parentURL.isEmpty()) {
        setParentBasedLinks(parentURL);
    }
}
