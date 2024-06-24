#ifndef WEBAPIOBJAUTHTOKEN_H
#define WEBAPIOBJAUTHTOKEN_H

#include "../WebApiObj.h"
#include <QDateTime>


class WebApiObjAuthToken : public WebApiObj {
public:
    WebApiObjAuthToken(QString data = QString(), QString id = QString(), QDateTime expTime = QDateTime());

    static QString objType() {return "authTokens";}

    bool hasID() const Q_DECL_OVERRIDE {return true;}
    QString type() const Q_DECL_OVERRIDE {return objType();}

    QString body() const {return m_body;}
    const QDateTime &expirationTime() const {return m_expTime;}
protected:
    void objSave(QJsonObject &obj) const Q_DECL_OVERRIDE;
    void objLoad(const QJsonObject &obj) Q_DECL_OVERRIDE;
private:
    QString m_body;
    QDateTime m_expTime;
};

#endif
