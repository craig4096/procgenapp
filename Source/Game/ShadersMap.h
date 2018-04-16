#ifndef SHADERSMAP_H
#define SHADERSMAP_H

#include <map>
#include "../Shader.h"

// could really be a typedef
class ShadersMap : public std::map<std::string, Shader*>
{
public:
};

#endif // SHADERSMAP_H
