#ifndef WEBAPILINK_H
#define WEBAPILINK_H

#include <QString>
class QJsonObject;

class WebApiLink {
public:
    WebApiLink();
    WebApiLink(QString name, QString url);

    QString name() const {return m_name;}
    QString url() const {return m_url;}

    void save(QJsonObject &linksObj) const;
private:
    QString m_name;
    QString m_url;
};

#endif
