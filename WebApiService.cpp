#include "WebApiService.h"
#include "WebApiHTTPServer.h"
#include "WebApiDbAccess.h"
#include "WebApiRequestProcessor.h"
#include <QtDebug>

WebApiService::WebApiService() : m_srv(Q_NULLPTR), m_dbAccess(Q_NULLPTR), m_reqProc(Q_NULLPTR) {

}

void WebApiService::start() {
    m_bgThread.start();
    moveToThread(&m_bgThread);
    connect(this, &WebApiService::startRequested,  this, &WebApiService::onStart, Qt::QueuedConnection);
    connect(this, &WebApiService::stopRequested,  this, &WebApiService::onStop, Qt::QueuedConnection);
    Q_EMIT startRequested();
}

void WebApiService::stop() {
    Q_EMIT stopRequested();
    m_bgThread.wait();
}

void WebApiService::setConnectionParams(QSharedPointer<DbConnectionParams> params) {
    if (m_dbAccess) {
        m_dbAccess->setConnectionParams(params);
    }
}

void WebApiService::onStart() {
    m_dbAccess = new WebApiDbAccess();
    m_reqProc = new WebApiRequestProcessor(m_dbAccess);


    m_srv = new WebApiHTTPServer(m_reqProc, 1537);
    bool ok = m_srv->start();
    qDebug() << "wbapi start" << ok;
}

void WebApiService::onStop() {
    if (m_srv) {
        m_srv->stop();
        delete  m_srv;
        m_srv = Q_NULLPTR;

        delete m_reqProc;
        delete m_dbAccess;
        m_bgThread.quit();
    }
}
