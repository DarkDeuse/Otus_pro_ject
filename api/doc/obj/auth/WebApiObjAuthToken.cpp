#include "WebApiObjAuthToken.h"
#include <QJsonObject>

WebApiObjAuthToken::WebApiObjAuthToken(QString data, QString id, QDateTime expTime) :
    WebApiObj(WebApiObjRef(objType(), id)), m_body(data), m_expTime(expTime) {

}

void WebApiObjAuthToken::objSave(QJsonObject &obj) const {
    QJsonObject attrObj;
    attrObj["body"] = m_body;
    if (m_expTime.isValid())
        attrObj["expirationTime"] = m_expTime.toString(Qt::ISODate);
    setAttributes(obj, attrObj);
}

void WebApiObjAuthToken::objLoad(const QJsonObject &obj) {
    const QJsonObject &attrObj = getAttributes(obj);
    m_body = attrObj["body"].toString();
    m_expTime = QDateTime::fromString(attrObj["expirationTime"].toString(), Qt::ISODate);
}
