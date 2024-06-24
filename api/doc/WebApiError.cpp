#include "WebApiError.h"
#include "Error.h"
#include <QJsonObject>


WebApiError::WebApiError() : m_code(0), m_http_status(0) {

}

WebApiError::WebApiError(int errCode, int http_status, QString title, QString details) :
    m_code(errCode), m_http_status(http_status), m_title(title), m_detail(details) {

}

bool WebApiError::operator==(const WebApiError &other) const {
    return  m_code == other.m_code;
}

bool WebApiError::operator!=(const WebApiError &other) const {
    return  !(*this == other);
}

void WebApiError::load(const QJsonObject &obj) {
    m_code = obj["code"].toString().toInt();
    m_http_status = obj["status"].toString().toInt();
    m_title = obj["title"].toString();
    m_detail = obj["detail"].toString();
}

void WebApiError::save(QJsonObject &obj) const {
    obj["code"] = QString::number(m_code);
    obj["title"] = m_title;
    obj["detail"] = m_detail;
    if (m_http_status > 0)
        obj["status"] = QString::number(m_http_status);
}
