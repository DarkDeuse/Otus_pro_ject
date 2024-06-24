#include "WebApiObjAuthLoginParams.h"
#include <QJsonObject>

WebApiObjAuthLoginParams::WebApiObjAuthLoginParams() : WebApiObj(WebApiObjRef(objType())) {

}

WebApiObjAuthLoginParams::WebApiObjAuthLoginParams(QString login, QString passwd) :
    WebApiObj(WebApiObjRef(objType())), m_login(login), m_password(passwd) {

}

void WebApiObjAuthLoginParams::objSave(QJsonObject &obj) const {
    QJsonObject attrObj;
    attrObj["login"] = m_login;
    attrObj["password"] = m_password;
    setAttributes(obj, attrObj);
}

void WebApiObjAuthLoginParams::objLoad(const QJsonObject &obj) {
    const QJsonObject &attrObj = getAttributes(obj);
    m_login = attrObj["login"].toString();
    m_password = attrObj["password"].toString();
}
