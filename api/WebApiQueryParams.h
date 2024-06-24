#ifndef WEBAPIQUERYPARAMS_H
#define WEBAPIQUERYPARAMS_H

#include <QStringList>
#include <QHash>

class WebApiQueryParams {
public:
    WebApiQueryParams();


    QStringList included() const {return m_included;}
    bool isIncludeRequired(QString rel, bool all_en) const;

    void setDefaultIncluded(QStringList included);

    bool parse(QHash<QString, QString> args);

    WebApiQueryParams stepDownParams(QString stepName, bool all_en = true) const;
private:
    QStringList m_included;
};

#endif
