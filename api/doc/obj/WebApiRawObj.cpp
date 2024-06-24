#include "WebApiRawObj.h"
#include "WebApiObj.h"


WebApiRawObj::WebApiRawObj(const WebApiObj &obj) {
    m_ref = obj.ref();
    QJsonObject jObj;
    obj.save(jObj);
    m_jObj = jObj;
}

WebApiRawObj::WebApiRawObj(const QJsonObject &jObj) {
    m_jObj = jObj;
    m_ref.load(jObj);
}

WebApiRawObj::WebApiRawObj(const WebApiObjRef &ref, const QJsonObject &jObj) :
    m_ref(ref), m_jObj(jObj) {
    m_ref.save(m_jObj);
}
