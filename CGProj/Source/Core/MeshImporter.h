#pragma once
#include <string>

class MeshComponent;
class TriangleComponent;

class MeshImporter
{
public:
    MeshComponent* ImportMesh(const std::string& path);

};
