#include "WebApiQueryParams.h"

WebApiQueryParams::WebApiQueryParams() {

}

bool WebApiQueryParams::isIncludeRequired(QString rel, bool all_en) const {
    bool ret = false;
    QString startPref = QString("%1.").arg(rel);
    QString startRecursivePref = QString("%1*").arg(rel);

    Q_FOREACH(QString inc, m_included) {
        if (((inc == "*") && all_en) || (inc == rel) || inc.startsWith(startPref) || inc.startsWith(startRecursivePref)) {
            ret = true;
            break;
        }
    }
    return ret;
}

void WebApiQueryParams::setDefaultIncluded(QStringList included) {
    if (m_included.isEmpty()) {
        m_included = included;
    }
}

bool WebApiQueryParams::parse(QHash<QString, QString> args) {
    bool ok = true;
    Q_FOREACH(QString param, args.keys()) {
        if (param == "include") {
            m_included = args[param].split(",");
        }
    }
    return ok;
}

WebApiQueryParams WebApiQueryParams::stepDownParams(QString stepName, bool all_en) const {
    WebApiQueryParams ret;
    QString startPref = QString("%1.").arg(stepName);
    QString startRecursivePref = QString("%1*").arg(stepName);
    Q_FOREACH(QString include, m_included) {
        if ((include == "*") && all_en) {
            ret.m_included.append(include);
        } else if (include.startsWith(startPref)) {
            ret.m_included.append(include.right(include.size() - startPref.size()));
        } else if (include.startsWith(startRecursivePref)) {
            ret.m_included.append(include);
            if (include.size() > (startRecursivePref.size() + 1)) {
                ret.m_included.append(include.right(include.size() - (startRecursivePref.size() + 1)));
            }
        }
    }
    return ret;
}
