#include "WebApiHTTPServer.h"
#include <microhttpd.h>
#include <QByteArray>
#include <QString>
#include <QUrl>
#include <QFile>
#include <QHash>
#include <QCoreApplication>
#include "api/WebApiURLs.h"

#include "WebApiRequest.h"
#include "WebApiRequestProcessor.h"
#include "api/WebApiURLs.h"
#include "WebApiRequestMethod.h"


#ifdef Q_OS_WIN32
    #define strcasecmp _stricmp
#endif

struct ConnectionInfo {
    QByteArray content;
};

struct RequestCtx {
    QHash<QString, QString> args;
};

static QByteArray load_file(QString filename) {
    QByteArray ret;
    QFile file(filename);
    if (file.open(QFile::ReadOnly)) {
        ret = file.readAll();
        file.close();
    }
    return  ret;
}

static void request_completed (void *cls, struct MHD_Connection *connection,
                        void **con_cls,
                        enum MHD_RequestTerminationCode toe) {
    ConnectionInfo *conInfo =  (ConnectionInfo *)*con_cls;
    delete conInfo;
    *con_cls = Q_NULLPTR;
}

static int parse_get_arg(void *cls, enum MHD_ValueKind kind,
                         const char *key,
                         const char *value) {
    WebApiRequest *req = reinterpret_cast<WebApiRequest *>(cls);
    req->args[QString::fromUtf8(key)] = value ? QString::fromUtf8(value) : QString();
    return MHD_YES;
}

static int parse_hdrs(void *cls, enum MHD_ValueKind kind,
                         const char *key,
                         const char *value) {
    WebApiRequest *req = reinterpret_cast<WebApiRequest *>(cls);
    if (!strcasecmp(key, MHD_HTTP_HEADER_AUTHORIZATION)) {
        QStringList authHdrFields = QString(value).split(" ");
        if (authHdrFields.size() >= 2) {
            req->auth = WebApiAuthParams(authHdrFields.at(0), authHdrFields.at(1));
        }
    } else if (!strcasecmp(key, MHD_HTTP_HEADER_ORIGIN)) {
        req->origin.setOrigin(value);
    }  else if(!strcasecmp(key, "Access-Control-Request-Method")) {
        req->origin.setReqMethods(value);
    } else if(!strcasecmp(key, "Access-Control-Request-Headers")) {
        req->origin.setReqHeaders(value);
    }
    return MHD_YES;
}
static int answer_to_connection (void *cls, struct MHD_Connection *connection,
                      const char *urlp,
                      const char *pmethod, const char *version,
                      const char *upload_data,
                      size_t *upload_data_size, void **con_cls) {
    int ret = MHD_YES;
    /* пустой *con_cls - признак начала нового соединения.
     * просто создаем контекст по данному вызову (обработка будет в последующих) */
    if(!*con_cls) {
         ConnectionInfo *conInfo = new ConnectionInfo();
        *con_cls = (void*) conInfo;
         ret = MHD_YES;
    } else {
        /* иначе это обработка созданного подключения.
         * в *con_cls идет контект подключения */
        ConnectionInfo *conInfo = static_cast<ConnectionInfo *>(*con_cls);
        /* конец передачи обозначается нулевой длиной данных.
         * если длина не нулевая - просто копируем данные и ждем
         * повторого вызова */
        if (*upload_data_size != 0) {
            QByteArray newData(upload_data, *upload_data_size);
            conInfo->content += newData;
            *upload_data_size = 0;
            ret = MHD_YES;
        } else {
            /* если нулевая длина данных, то запрос принят, выполняем обработку */
            WebApiRequestProcessor *reqProc = static_cast<WebApiRequestProcessor *>(cls);
            WebApiRequest req;
            req.url = QUrl(urlp);
            unsigned http_status = MHD_HTTP_OK;
            QByteArray respData;
            QList<const WebApiRequestMethod *> allowedMethods;
            /* сперва проверяем, что URL вообще принадлежит API.
             * Если нет, то дальнейшая обработка не треубется */
            if (!reqProc->isUrlAplicable(req.url)) {
                http_status = MHD_HTTP_NOT_FOUND;
            } else {
                /* анализ метода */
                req.method = WebApiRequestMethods::method(QString(pmethod));
                if (!req.method) {
                    http_status = MHD_HTTP_NOT_IMPLEMENTED;
                }
            }

            if (http_status == MHD_HTTP_OK) {
                MHD_get_connection_values(connection, MHD_HEADER_KIND, parse_hdrs, &req);
                MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, parse_get_arg, &req);
                req.data = conInfo->content;
                /* обработка запроса */
                http_status = reqProc->processHttpRequest(req, respData, allowedMethods);
            }
            struct MHD_Response *response;
            response = MHD_create_response_from_buffer (respData.length(),
                                                        respData.data(),
                                                        MHD_RESPMEM_MUST_COPY);
            if (response) {
                if (req.origin.origin()) {
                    bool is_valid = true;
                    MHD_add_response_header(response, MHD_HTTP_HEADER_VARY, "Origin");
                    if (req.method == WebApiRequestMethods::options()) {
                        const WebApiRequestMethod *reqMethod = Q_NULLPTR;
                        if (req.origin.reqMehtod()) {
                            reqMethod = WebApiRequestMethods::method(QString(req.origin.reqMehtod()));
                            if (allowedMethods.contains(reqMethod)) {
                                is_valid = true;
                            }
                        } else {
                            is_valid = false;
                        }

                        if (is_valid) {
                            /* можем возвращать как весь список поддерживаемых методов, так
                             * и только тот, который был запрашиваем */
                            MHD_add_response_header(response, "Access-Control-Allow-Methods",
                                                    reqMethod->name().toLatin1().data());
                            /* если запрашивались дополнительные заголовки, то возвращаем их.
                             * сейчас считаем, что все заголовки разрешены */
                            if (req.origin.reqHeaders()) {
                                MHD_add_response_header(response, "Access-Control-Allow-Headers",
                                                        req.origin.reqHeaders());
                            }
                        }
                    }

                    if (is_valid) {
                        MHD_add_response_header(response, "Access-Control-Allow-Credentials", "true");
                        MHD_add_response_header (response, MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN,
                                                 req.origin.origin());
                    }
                } else if (req.method == WebApiRequestMethods::options()) {
                    QString methodsStr;
                    Q_FOREACH(const WebApiRequestMethod *method, allowedMethods) {
                        if (!methodsStr.isEmpty())
                            methodsStr.append(", ");
                        methodsStr.append(method->name());
                    }
                    MHD_add_response_header(response, MHD_HTTP_HEADER_ALLOW,
                                            methodsStr.toLocal8Bit().data());

                }
                MHD_add_response_header (response, MHD_HTTP_HEADER_CACHE_CONTROL,
                                         "no-store, no-cache, must-revalidate");
                MHD_add_response_header (response, MHD_HTTP_HEADER_PRAGMA,
                                         "no-cache");



                if (!respData.isEmpty()) {                    
                    MHD_add_response_header (response, MHD_HTTP_HEADER_CONTENT_TYPE,
                                             "application/vnd.api+json");
                }
                if (http_status == MHD_HTTP_UNAUTHORIZED) {
                    MHD_add_response_header (response, MHD_HTTP_HEADER_WWW_AUTHENTICATE,
                                             WebApiURLs::authScheme().toLatin1().data());
                }


                ret = MHD_queue_response (connection, http_status, response);
                MHD_destroy_response (response);
            } else {
                ret = MHD_NO;
            }
        }
    }
    return ret;

}
WebApiHTTPServer::WebApiHTTPServer(WebApiRequestProcessor *proc, int port) :
    d(Q_NULLPTR), m_port(port), m_proc(proc) {

}
bool WebApiHTTPServer::start() {
    stop();
    m_server_key = load_file(QString("%1/%2").arg(qApp->applicationDirPath()).arg("keys/webapisrv.key"));
    m_server_cert = load_file(QString("%1/%2").arg(qApp->applicationDirPath()).arg("keys/webapisrv.pem"));

    d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION //MHD_USE_AUTO_INTERNAL_THREAD
                         | MHD_USE_SSL,
                         m_port,
                         Q_NULLPTR,
                         Q_NULLPTR,
                         &answer_to_connection,
                         m_proc,
                         MHD_OPTION_NOTIFY_COMPLETED, &request_completed, this,
                         MHD_OPTION_HTTPS_MEM_KEY, m_server_key.data(),
                         MHD_OPTION_HTTPS_MEM_CERT, m_server_cert.data(),
                         MHD_OPTION_END);
    return isRunning();
}

void WebApiHTTPServer::stop() {
    if (isRunning()) {
        MHD_stop_daemon(d);
        d = Q_NULLPTR;
    }
}

bool WebApiHTTPServer::isRunning() const {
    return d != Q_NULLPTR;
}
