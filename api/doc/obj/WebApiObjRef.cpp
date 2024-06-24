#include "WebApiObjRef.h"
#include <QJsonObject>

WebApiObjRef::WebApiObjRef() {

}

WebApiObjRef::WebApiObjRef(QString type, QString id) :
    m_type(type), m_id(id) {

}

bool WebApiObjRef::isSameType(QString type) const {
    return  m_type.compare(type, Qt::CaseInsensitive) == 0;
}

bool WebApiObjRef::operator==(const WebApiObjRef &ref) const {
    return  isSameType(ref.type()) && (m_id == ref.m_id);
}

void WebApiObjRef::setID(QString id) {
    m_id = id;
}

void WebApiObjRef::save(QJsonObject &obj) const {
    obj["type"] = m_type;
    if (!m_id.isEmpty())
        obj["id"] = m_id;
}

void WebApiObjRef::load(const QJsonObject &obj) {
    m_type = obj["type"].toString();
    m_id = obj["id"].toString();
}
