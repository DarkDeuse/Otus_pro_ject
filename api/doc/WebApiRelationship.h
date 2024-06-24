#ifndef WEBAPIRELATIONSHIP_H
#define WEBAPIRELATIONSHIP_H

#include "obj/WebApiObjRef.h"
#include "WebApiObjLinks.h"
#include <QList>
class QJsonObject;

class WebApiRelationship {
public:
    WebApiRelationship(QString name, bool many, QString parentURL = QString());
    WebApiRelationship(QString name, const WebApiObjRef &ref, QString parentURL = QString());
    WebApiRelationship(QString name, QList<WebApiObjRef > refs, QString parentURL = QString());

    void addRef(const WebApiObjRef &ref);

    void save(QJsonObject &relationsObj) const;

    void setParentBasedLinks(QString parent);
    void setSelfLink(QString url);
    void setRelatedLink(QString url);
private:
    void init(QString parentURL);

    QString m_name;
    bool m_many;
    QList<WebApiObjRef > m_refs;
    WebApiObjLinks m_links;
};

#endif
