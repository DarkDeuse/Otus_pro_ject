#ifndef WEBAPIHTTPSERVER_H
#define WEBAPIHTTPSERVER_H

#include <QByteArray>
class WebApiRequestProcessor;

class WebApiHTTPServer {
public:
    WebApiHTTPServer(WebApiRequestProcessor *proc, int port);


    bool start();
    void stop();
    bool isRunning() const;

    int port() const {return m_port;}
private:
    struct MHD_Daemon * d;
    int m_port;

    QByteArray m_server_key;
    QByteArray m_server_cert;

    WebApiRequestProcessor *m_proc;
};

#endif
