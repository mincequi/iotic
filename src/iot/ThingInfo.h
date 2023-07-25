#ifndef THINGINFO_H
#define THINGINFO_H

#include <string>

class ThingInfo {
public:
    enum Type {
        Invalid,
        SunSpec,
        Http
    };

    ThingInfo(Type type, const std::string& id);

    bool isValid() const;
    Type type() const;
    std::string id() const;
    std::string name() const;

private:
    Type _type = Type::Invalid;
    std::string _id;
    std::string _name;
};

#endif // THINGINFO_H
