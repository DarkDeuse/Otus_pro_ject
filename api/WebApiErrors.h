#ifndef WEBAPIERRORS_H
#define WEBAPIERRORS_H

#include "Error.h"
#include "doc/WebApiError.h"
#include "HTTPStatus.h"


class WebApiErrors : public QObject {
    Q_OBJECT
public:
    static WebApiError noDbConnection(QString details = QString()) {
        return WebApiError(1000, HTTPStatus::INTERNAL_SERVER_ERROR, tr("Нед действительного подключения к базе данных"), details);
    }

    static WebApiError methodNotAllowed(QString details = QString()) {
        return  WebApiError(1001, HTTPStatus::METHOD_NOT_ALLOWED, tr("Метод не применим для указанного ресурса"), details);
    }

    static WebApiError resourceNotFound(QString details = QString()) {
        return  WebApiError(1002, HTTPStatus::NOT_FOUND, tr("Указанный ресурс не найден"), details);
    }

    static WebApiError unknownURL(QString details = QString()) {
        return  WebApiError(1003, HTTPStatus::NOT_FOUND, tr("Неизвестный URL"), details);
    }

    static WebApiError reqParseInvalidParams(QString details = QString()) {
        return  WebApiError(1004, HTTPStatus::BAD_REQUEST, tr("Неверные параметры запроса"), details);
    }

    static WebApiError docParaseInvalidJSON(QString details = QString()) {
        return  WebApiError(1100, HTTPStatus::BAD_REQUEST, tr("Документ не является действительным документом JSON"), details);
    }

    static WebApiError docParaseNoPrimaryData(QString details = QString()) {
        return  WebApiError(1101, HTTPStatus::BAD_REQUEST, tr("В документе отсутствуют первичные данные"), details);
    }






    static WebApiError authFailed(QString details = QString()) {
        return  WebApiError(1200, HTTPStatus::UNAUTHORIZED, tr("Ошибка аутентификации"), details);
    }

    static WebApiError authReqired(QString details = QString()) {
        return  WebApiError(1201, HTTPStatus::UNAUTHORIZED, tr("Требуется аутентификация"), details);
    }

    static WebApiError authTokenCreate(QString details = QString()) {
        return  WebApiError(1202, HTTPStatus::INTERNAL_SERVER_ERROR, tr("Ошибка создания токена аутентификации"), details);
    }

    static WebApiError authMethodNotSupported(QString details = QString()) {
        return  WebApiError(1203, HTTPStatus::UNAUTHORIZED, tr("Указанный метод аутентификации не поддерживается"), details);
    }

    static WebApiError authInvalidToken(QString details = QString()) {
        return  WebApiError(1204, HTTPStatus::UNAUTHORIZED, tr("Неверный токен аутентификации"), details);
    }
    static WebApiError authTokenExpired(QString details = QString()) {
        return  WebApiError(1205, HTTPStatus::UNAUTHORIZED, tr("Время действия токена аутентификации истекло"), details);
    }
    static WebApiError authInvalidDb(QString details = QString()) {
        return  WebApiError(1206, HTTPStatus::UNAUTHORIZED, tr("Токен аутентификации не относится к используемой базе данных"), details);
    }
};

#endif
