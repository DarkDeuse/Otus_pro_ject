#ifndef WEBAPIOBJLINKS_H
#define WEBAPIOBJLINKS_H

#include "WebApiLink.h"
#include <QHash>


class WebApiObjLinks {
public:
    WebApiObjLinks();
    WebApiObjLinks(QString selfUrl);

    void save(QJsonObject &parentObj) const;

    void addLink(const WebApiLink &link);
private:
    QHash<QString, WebApiLink> m_links;
};

#endif
