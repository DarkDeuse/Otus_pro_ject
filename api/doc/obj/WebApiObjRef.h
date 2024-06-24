#ifndef WEBAPIOBJREF_H
#define WEBAPIOBJREF_H

#include <QString>
class QJsonObject;

class WebApiObjRef {
public:
    WebApiObjRef();
    WebApiObjRef(QString type, QString id = QString());

    QString type() const {return m_type;}
    QString id() const {return m_id;}

    bool isSameType(QString type) const;
    bool operator==(const WebApiObjRef &ref) const;

    void setID(QString id);

    void save(QJsonObject &obj) const;
    void load(const QJsonObject &obj);
private:
    QString m_type;
    QString m_id;
};

#endif
