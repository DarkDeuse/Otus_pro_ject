#ifndef WEBAPIREQUESTORIGININFO_H
#define WEBAPIREQUESTORIGININFO_H


class WebApiRequestOriginInfo {
public:
    WebApiRequestOriginInfo();

    void setOrigin(const char *origin) {m_origin = origin;}
    void setReqMethods(const char *reqMehtods) {m_req_methods = reqMehtods;}
    void setReqHeaders(const char *reqHeaders) {m_req_headers = reqHeaders;}


    const char *origin() const {return m_origin;}
    const char *reqMehtod() const {return m_req_methods;}
    const char *reqHeaders() const {return m_req_headers;}
private:
    const char *m_origin;
    const char *m_req_methods;
    const char *m_req_headers;
};

#endif
