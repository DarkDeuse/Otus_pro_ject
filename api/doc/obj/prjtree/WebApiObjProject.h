#ifndef WEBAPIOBJPROJECT_H
#define WEBAPIOBJPROJECT_H

#include "../WebApiObj.h"
class Project;

class WebApiObjProject : public WebApiObj {
public:
    WebApiObjProject();
    WebApiObjProject(const Project *prj);

    static QString objType() {return "project";}

    bool hasID() const Q_DECL_OVERRIDE {return true;}
    QString type() const Q_DECL_OVERRIDE {return objType();}
protected:
    void objSave(QJsonObject &obj) const Q_DECL_OVERRIDE;
    void objLoad(const QJsonObject &obj) Q_DECL_OVERRIDE;
};

#endif
