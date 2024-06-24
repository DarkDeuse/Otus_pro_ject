#include "WebApiObj.h"
#include "WebApiRawObj.h"
#include <QJsonObject>
#include <QJsonArray>


WebApiObj::~WebApiObj() {

}

QString WebApiObj::getType(const QJsonObject &obj) {
    return  obj["type"].toString();
}

void WebApiObj::save(QJsonObject &obj) const {
    m_ref.save(obj);
    objSave(obj);
}

bool WebApiObj::load(const QJsonObject &obj) {
    bool ok = false;
    WebApiObjRef ref;
    ref.load(obj);
    if (ref.type() == type()) {
        objLoad(obj);
        ok = true;
    }
    return  ok;
}

bool WebApiObj::load(const WebApiRawObj &rawObj) {
    return  load(rawObj.jsonObject());
}

WebApiObj::WebApiObj(const WebApiObjRef &ref) : m_ref(ref) {

}

const QJsonObject WebApiObj::getAttributes(const QJsonObject &obj) {
    return obj["attributes"].toObject();
}

void WebApiObj::setAttributes(QJsonObject &obj, const QJsonObject &attrObj) {
    obj["attributes"] = attrObj;
}
