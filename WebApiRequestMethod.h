#ifndef WEBAPIREQUESTMETHOD_H
#define WEBAPIREQUESTMETHOD_H

#include <QString>
#include <QList>

class WebApiRequestMethod {
public:
    virtual ~WebApiRequestMethod();
    virtual QString name() const = 0;
};

class WebApiRequestMethods {
public:
    static const WebApiRequestMethod *get();
    static const WebApiRequestMethod *head();
    static const WebApiRequestMethod *post();
    static const WebApiRequestMethod *put();
    static const WebApiRequestMethod *del();
    static const WebApiRequestMethod *connect();
    static const WebApiRequestMethod *options();
    static const WebApiRequestMethod *trace();
    static const WebApiRequestMethod *path();


    static const WebApiRequestMethod *method(QString name);
    static QList<const WebApiRequestMethod *> allMethods();
};

#endif
