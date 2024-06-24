#ifndef WEBAPIERROR_H
#define WEBAPIERROR_H

#include <QString>
class QJsonObject;

class WebApiError {
public:
    WebApiError();
    WebApiError(int errCode, int http_status, QString title, QString details);

    bool operator==(const WebApiError &other) const;
    bool operator!=(const WebApiError &other) const;

    bool isSuccess() const {return  m_code == 0;}
    int code() const {return m_code;}
    int httpStatus() const {return m_http_status;}
    bool httpStatusValid() const {return m_http_status > 0;}
    QString title() const {return m_title;}
    QString detail() const {return m_detail;}

    void load(const QJsonObject &obj);
    void save(QJsonObject &obj) const;
private:
    int m_code;
    int m_http_status;
    QString m_title;
    QString m_detail;

};

#endif
