#ifndef WEBAPIOBJ_H
#define WEBAPIOBJ_H

#include <QString>
#include "WebApiObjRef.h"
class QJsonObject;
class QJsonArray;
class WebApiRawObj;


class WebApiObj {
public:
    virtual ~WebApiObj();

    virtual bool hasID() const = 0;
    virtual QString type() const = 0;


    static QString getType(const QJsonObject &obj);
    void save(QJsonObject &obj) const;
    bool load(const QJsonObject &obj);
    bool load(const WebApiRawObj &rawObj);

    const WebApiObjRef &ref() const {return m_ref;}
protected:
    WebApiObj(const WebApiObjRef &ref);

    virtual void objSave(QJsonObject &obj) const = 0;
    virtual void objLoad(const QJsonObject &obj) = 0;

    static const QJsonObject getAttributes(const QJsonObject &obj);
    static void setAttributes(QJsonObject &obj, const QJsonObject &attrObj);

private:
    WebApiObjRef m_ref;
};

#endif
