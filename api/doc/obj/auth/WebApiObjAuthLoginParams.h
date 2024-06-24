#ifndef WEBAPIOBJAUTHLOGINPARAMS_H
#define WEBAPIOBJAUTHLOGINPARAMS_H

#include "../WebApiObj.h"

class WebApiObjAuthLoginParams : public WebApiObj {
public:
    WebApiObjAuthLoginParams();
    WebApiObjAuthLoginParams(QString login, QString passwd);

    static QString objType() {return "authLoginParams";}

    bool hasID() const Q_DECL_OVERRIDE {return false;}
    QString type() const Q_DECL_OVERRIDE {return objType();}

    QString login() const {return m_login;}
    QString password() const {return m_password;}
protected:
    void objSave(QJsonObject &obj) const Q_DECL_OVERRIDE;
    void objLoad(const QJsonObject &obj) Q_DECL_OVERRIDE;
private:
    QString m_login;
    QString m_password;
};

#endif
