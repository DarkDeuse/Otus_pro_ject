#ifndef WEBAPIAUTHPARAMS_H
#define WEBAPIAUTHPARAMS_H

#include <QString>
#include <QByteArray>

class WebApiAuthParams {
public:
    WebApiAuthParams();
    WebApiAuthParams(QString scheme, QString data);

    QString scheme() const {return m_scheme;}
    QString data() const {return m_data;}
private:
    QString m_scheme;
    QString m_data;
};

#endif
