#ifndef WEBAPIURLS_H
#define WEBAPIURLS_H

#include <QString>

class WebApiURLs {
public:
    static QString authScheme() {return "Bearer";}

    static QString root() {return  "/api/v1.0/";}

    static QString auth() {return  "auth";}
    struct auth {
        static QString login()    {return  "login";}
    };


    static QString projects() {return "projects";}
    static QString storeLocations() {return "storeLocations";}
    static QString floors() {return "floors";}
    static QString zones() {return "zones";}
    static QString entrances() {return "entrances";}
    static QString entranceRefs() {return "entranceAssignments";}
    static QString retailTypes() {return "retailTypes";}
    static QString zoneTypes() {return "zoneTypes";}
};



#endif

