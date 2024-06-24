#include "WebApiObjProject.h"

WebApiObjProject::WebApiObjProject() : WebApiObj(WebApiObjRef(objType())) {

}

WebApiObjProject::WebApiObjProject(const Project *prj)  : WebApiObj(WebApiObjRef(objType(), prj->idStr())) {

}

void WebApiObjProject::objSave(QJsonObject &obj) const {

}

void WebApiObjProject::objLoad(const QJsonObject &obj) {

}
