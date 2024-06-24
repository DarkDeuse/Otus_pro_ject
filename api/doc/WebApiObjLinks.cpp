#include "WebApiObjLinks.h"
#include <QJsonObject>

WebApiObjLinks::WebApiObjLinks() {
}

WebApiObjLinks::WebApiObjLinks(QString selfUrl) {
    addLink(WebApiLink("self", selfUrl));
}

void WebApiObjLinks::save(QJsonObject &parentObj) const {
    if (!m_links.isEmpty()) {
        QJsonObject linksObj;
        Q_FOREACH(const WebApiLink &link, m_links) {
            link.save(linksObj);
        }
        parentObj["links"] = linksObj;
    }
}

void WebApiObjLinks::addLink(const WebApiLink &link) {
    m_links[link.name()] = link;
}
