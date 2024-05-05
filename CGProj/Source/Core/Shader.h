#pragma once
#include <cstdint>
#include <d3d11.h>

#include "Utils/Array/Array.h"

class Engine;

enum class ShaderType : uint8_t
{
    Vertex,
    Pixel,
    Geometry,
    Compute,
    Hull,
    Domain
};

struct ShaderData
{
    bool operator==(const ShaderData& other) const
    {
        return pFileName == other.pFileName
               && shader == other.shader
               && type == other.type;
    }

    ShaderType type;
    LPCWSTR pFileName = nullptr;
    ID3D11DeviceChild* shader = nullptr;
    ID3DBlob* byteCode;    
    ID3D11InputLayout* layout;
};

class Shader
{
public:
    Shader();
    virtual ~Shader();
    void Destroy();
    void Draw();

    static bool FindShader(LPCWSTR pFileName, ShaderType type, ShaderData& retShaderData);
    static bool ContainsShader(LPCWSTR pFileName, ShaderType type);
    static bool ContainsShader(const ShaderData& data);
    static void RemoveShader(const ShaderData& data);
    static void AddShader(const ShaderData& data);
    bool CreateShader(LPCWSTR shaderPath, ShaderType type, const D3D_SHADER_MACRO* shaderMacros = nullptr, LPSTR entry = nullptr);
    void AddInputElementDesc(LPCSTR SemanticName, UINT SemanticIndex = 0, DXGI_FORMAT Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
        D3D11_INPUT_CLASSIFICATION InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA);

protected:
    struct DefData
    {
        LPSTR entryPoint;
        LPSTR target;
    };

    void UpdateShadersData();
    DefData GetEntryPointAndTargetByType(ShaderType type) const;
    ID3D11DeviceChild* CreateShaderByType(ShaderType type, ID3DBlob* byteCode);
    ShaderData FindShaderByType(ShaderType type) const;
    bool ContainsLocalShader(LPCWSTR pFileName, ShaderType type);
    bool CheckResCompile(ID3DBlob* errorVertexCode, const HRESULT& res, LPCWSTR pFileName) const;


    Engine* engInst;
    Array<ShaderData> shadersData;
    Array<D3D11_INPUT_ELEMENT_DESC> inputElements;

    LPSTR defVSEntry = (LPSTR)"VS";
    LPSTR defPSEntry = (LPSTR)"PS";
    LPSTR defGSEntry = (LPSTR)"GS";

    LPSTR defVSTarget = (LPSTR)"vs_5_0";
    LPSTR defPSTarget = (LPSTR)"ps_5_0";
    LPSTR defGSTarget = (LPSTR)"gs_5_0";

private:
    ID3D11InputLayout* curLayout = nullptr;
};
