#ifndef IPROPERTYINSPECTOR_H
#define IPROPERTYINSPECTOR_H

#include <string>

class IPropertyInspector
{
public:
    virtual ~IPropertyInspector() = default;

    virtual void property(const std::string& name, int& value) = 0;
    virtual void property(const std::string& name, float& value) = 0;
};

#endif // IPROPERTYINSPECTOR_H
