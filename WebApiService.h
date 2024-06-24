#ifndef WEBAPISERVICE_H
#define WEBAPISERVICE_H

#include <QObject>
#include <QThread>
#include <QSharedPointer>
class WebApiHTTPServer;
class WebApiRequestProcessor;
class WebApiDbAccess;
class DbConnectionParams;

class WebApiService : public QObject {
    Q_OBJECT
public:
    WebApiService();

    void start();
    void stop();
public Q_SLOTS:
    void setConnectionParams(QSharedPointer<DbConnectionParams> params);
Q_SIGNALS:
    void startRequested();
    void stopRequested();
private Q_SLOTS:
    void onStart();
    void onStop();

private:
    QThread m_bgThread;
    WebApiHTTPServer *m_srv;

    WebApiDbAccess *m_dbAccess;
    WebApiRequestProcessor *m_reqProc;
};

#endif
