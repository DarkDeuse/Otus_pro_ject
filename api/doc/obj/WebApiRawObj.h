#ifndef WEBAPIRAWOBJ_H
#define WEBAPIRAWOBJ_H

#include <QJsonObject>
#include "WebApiObjRef.h"
class WebApiObj;

class WebApiRawObj {
public:
    WebApiRawObj(const WebApiObj &obj);
    WebApiRawObj(const QJsonObject &jObj);
    WebApiRawObj(const WebApiObjRef &ref, const QJsonObject &jObj);

    const WebApiObjRef &objRef() const {return m_ref;}
    const QJsonObject &jsonObject() const {return  m_jObj;}
private:
    WebApiObjRef m_ref;
    QJsonObject m_jObj;
};

#endif 
