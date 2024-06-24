#include "WebApiLink.h"
#include <QJsonObject>

WebApiLink::WebApiLink() {

}

WebApiLink::WebApiLink(QString name, QString url) : m_name(name), m_url(url){

}

void WebApiLink::save(QJsonObject &linksObj) const {
    linksObj[m_name]  = m_url;
}
