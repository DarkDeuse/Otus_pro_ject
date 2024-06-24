#include "WebApiRequestMethod.h"

WebApiRequestMethod::~WebApiRequestMethod() {

}

const WebApiRequestMethod *WebApiRequestMethods::get() {
    static const class WebApiRequestMethodGet : public WebApiRequestMethod {
        QString name() const Q_DECL_OVERRIDE {return "GET";}
    } method;
    return &method;
}

const WebApiRequestMethod *WebApiRequestMethods::head() {
    static const class WebApiRequestMethodHead : public WebApiRequestMethod {
        QString name() const Q_DECL_OVERRIDE {return "HEAD";}
    } method;
    return &method;
}

const WebApiRequestMethod *WebApiRequestMethods::post() {
    static const class WebApiRequestMethodPost : public WebApiRequestMethod {
        QString name() const Q_DECL_OVERRIDE {return "POST";}
    } method;
    return &method;
}

const WebApiRequestMethod *WebApiRequestMethods::put() {
    static const class WebApiRequestMethodPut : public WebApiRequestMethod {
        QString name() const Q_DECL_OVERRIDE {return "PUT";}
    } method;
    return &method;
}

const WebApiRequestMethod *WebApiRequestMethods::del() {
    static const class WebApiRequestMethodDelete : public WebApiRequestMethod {
        QString name() const Q_DECL_OVERRIDE {return "DELETE";}
    } method;
    return &method;
}

const WebApiRequestMethod *WebApiRequestMethods::connect() {
    static const class WebApiRequestMethodConnect : public WebApiRequestMethod {
        QString name() const Q_DECL_OVERRIDE {return "CONNECT";}
    } method;
    return &method;
}

const WebApiRequestMethod *WebApiRequestMethods::options() {
    static const class WebApiRequestMethodOptions : public WebApiRequestMethod {
        QString name() const Q_DECL_OVERRIDE {return "OPTIONS";}
    } method;
    return &method;
}

const WebApiRequestMethod *WebApiRequestMethods::trace() {
    static const class WebApiRequestMethodTrace : public WebApiRequestMethod {
        QString name() const Q_DECL_OVERRIDE {return "TRACE";}
    } method;
    return &method;
}

const WebApiRequestMethod *WebApiRequestMethods::path() {
    static const class WebApiRequestMethodPath : public WebApiRequestMethod {
        QString name() const Q_DECL_OVERRIDE {return "PATH";}
    } method;
    return &method;
}

const WebApiRequestMethod *WebApiRequestMethods::method(QString name) {
    const WebApiRequestMethod *ret = Q_NULLPTR;
    QList<const WebApiRequestMethod *>  all = allMethods();
    Q_FOREACH(const WebApiRequestMethod *met, all) {
        if (met->name() == name) {
            ret = met;
            break;
        }
    }
    return  ret;
}

QList<const WebApiRequestMethod *> WebApiRequestMethods::allMethods() {
    return QList<const WebApiRequestMethod *>() <<  get() << head() << post() << put()
                                                 << del() << connect() << options()
                                                 << trace() << path();
}
