#include "Shader.h"

#include <d3dcompiler.h>
#include <iostream>

#include "Core/Engine.h"
#include "Core/Windisplay.h"

static Array<ShaderData> GlobalShadersData;


Shader::Shader()
{
    engInst = &Engine::GetInstance();
}

Shader::~Shader()
{
    Destroy();
}

bool Shader::FindShader(LPCWSTR pFileName, ShaderType type, ShaderData& retShaderData)
{
    for (const auto& globalData : GlobalShadersData)
    {
        if (globalData.pFileName == pFileName && globalData.type == type)
        {
            retShaderData = globalData;
            return true;
        }
    }
    return false;
}

bool Shader::ContainsShader(LPCWSTR pFileName, ShaderType type)
{
    for (const auto& globalData : GlobalShadersData)
    {
        if (globalData.pFileName == pFileName && globalData.type == type) return true;
    }
    return false;
}

bool Shader::ContainsShader(const ShaderData& data)
{
    return GlobalShadersData.contains(data);
}

bool Shader::ContainsLocalShader(LPCWSTR pFileName, ShaderType type)
{
    for (const auto& data : shadersData)
    {
        if (data.pFileName == pFileName && data.type == type) return true;
    }
    return false;
}

void Shader::RemoveShader(const ShaderData& data)
{
    GlobalShadersData.remove(data);
}

void Shader::AddShader(const ShaderData& data)
{
    if (!ContainsShader(data)) GlobalShadersData.insert(data);
}

void Shader::Clear()
{
    for (const auto& shaderData : GlobalShadersData)
    {
        RemoveShader(shaderData);
        //if (shaderData.layout) shaderData.layout->Release();
        if (shaderData.byteCode) shaderData.byteCode->Release();
        if (shaderData.shader) shaderData.shader->Release();
    }
    GlobalShadersData.clear();
}

void Shader::Destroy()
{
    /*for (const auto& shaderData : shadersData)
    {
        RemoveShader(shaderData);
        if (shaderData.layout) shaderData.layout->Release();
        if (shaderData.byteCode) shaderData.byteCode->Release();
        if (shaderData.shader) shaderData.shader->Release();
    }*/
    if (curLayout) curLayout->Release();
    shadersData.clear();
}

void Shader::Draw()
{
    if (curLayout) engInst->GetContext()->IASetInputLayout(curLayout);
    engInst->GetContext()->VSSetShader(nullptr, nullptr, 0);
    engInst->GetContext()->PSSetShader(nullptr, nullptr, 0);
    engInst->GetContext()->GSSetShader(nullptr, nullptr, 0);
    for (const auto& data : shadersData)
    {
        switch (data.type)
        {
            case SVertex:
            {
                ID3D11VertexShader* shader;
                if (S_OK == data.shader->QueryInterface(IID_ID3D11VertexShader,
                        reinterpret_cast<void**>(&shader)))
                {
                    engInst->GetContext()->VSSetShader(shader, nullptr, 0);
                }
                break;
            }
            case SPixel:
            {
                ID3D11PixelShader* shader;
                if (S_OK == data.shader->QueryInterface(IID_ID3D11PixelShader,
                        reinterpret_cast<void**>(&shader)))
                {
                    engInst->GetContext()->PSSetShader(shader, nullptr, 0);
                }

                break;
            }
            case SGeometry:
            {
                ID3D11GeometryShader* shader;
                if (S_OK == data.shader->QueryInterface(IID_ID3D11GeometryShader,
                        reinterpret_cast<void**>(&shader)))
                {
                    engInst->GetContext()->GSSetShader(shader, nullptr, 0);
                }

                break;
            }
            default: return;
        }
    }
}

bool Shader::CreateShader(LPCWSTR shaderPath, ShaderType type, const D3D_SHADER_MACRO* shaderMacros, LPSTR entry)
{
    //D3D_SHADER_MACRO Shader_Macros[] = {"TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr};
    if (ContainsLocalShader(shaderPath, type)) return true;
    ShaderData data;
    if (FindShader(shaderPath, type, data))
    {
        shadersData.insert(data);
        UpdateShadersData();
        if (!curLayout)
            engInst->GetDevice()->CreateInputLayout(&inputElements[0], inputElements.size(), data.byteCode->GetBufferPointer(),
                data.byteCode->GetBufferSize(), &curLayout);
        return true;
    }

    ID3DBlob* errorPixelCode;
    ID3DBlob* byteCode;

    const DefData defData = GetEntryPointAndTargetByType(type);
    const auto res = D3DCompileFromFile(shaderPath, shaderMacros, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entry ? entry : defData.entryPoint, defData.target,
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &byteCode, &errorPixelCode);

    if (!CheckResCompile(errorPixelCode, res, shaderPath)) return false;

    if (const auto shader = CreateShaderByType(type, byteCode))
    {
        shadersData.insert(ShaderData{type, shaderPath, shader, byteCode});
        UpdateShadersData();
        if (!curLayout)
            engInst->GetDevice()->CreateInputLayout(&inputElements[0], inputElements.size(), byteCode->GetBufferPointer(),
                byteCode->GetBufferSize(), &curLayout);
        return true;
    }
    return false;
}

ID3D11DeviceChild* Shader::CreateShaderByType(ShaderType type, ID3DBlob* byteCode)
{
    const auto bp = byteCode->GetBufferPointer();
    const auto bs = byteCode->GetBufferSize();
    //curLayout = nullptr;
    switch (type)
    {
        case SVertex:
        {
            ID3D11VertexShader* shad;
            if (FAILED(engInst->GetDevice()->CreateVertexShader(bp, bs, nullptr, &shad))) return nullptr;
            return shad;
        }

        case SPixel:
        {
            ID3D11PixelShader* shad;
            if (FAILED(engInst->GetDevice()->CreatePixelShader(bp, bs, nullptr, &shad))) return nullptr;
            return shad;
        }

        case SGeometry:
        {
            ID3D11GeometryShader* shad;
            if (FAILED(engInst->GetDevice()->CreateGeometryShader(bp, bs, nullptr, &shad))) return nullptr;
            return shad;
        }

        default: return nullptr;
    }
}

ShaderData Shader::FindShaderByType(ShaderType type) const
{
    for (const auto& shaderData : shadersData)
    {
        if (shaderData.type == type) return shaderData;
    }
    return {};
}

void Shader::AddInputElementDesc(LPCSTR SemanticName, UINT SemanticIndex, DXGI_FORMAT Format, D3D11_INPUT_CLASSIFICATION InputSlotClass)
{
    inputElements.insert(D3D11_INPUT_ELEMENT_DESC{SemanticName, SemanticIndex, Format, 0, D3D11_APPEND_ALIGNED_ELEMENT, InputSlotClass, 0});
}

bool Shader::CheckResCompile(ID3DBlob* errorVertexCode, const HRESULT& res, LPCWSTR pFileName) const
{
    if (FAILED(res))
    {
        // If the shader failed to compile it should have written something to the error message.
        if (errorVertexCode)
        {
            char* compileErrors = (char*)(errorVertexCode->GetBufferPointer());
            std::cout << compileErrors << std::endl;
        }
        // If there was  nothing in the error message then it simply could not find the shader file itself.
        else
        {
            MessageBox(engInst->GetDisplay()->hWnd, pFileName, L"Missing Shader File", MB_OK);
        }

        return false;
    }
    return true;
}

Shader::DefData Shader::GetEntryPointAndTargetByType(ShaderType type) const
{
    switch (type)
    {
        case SVertex: return {defVSEntry, defVSTarget};
        case SPixel: return {defPSEntry, defPSTarget};
        case SGeometry: return {defGSEntry, defGSTarget};
        default: return {};
    }
}

void Shader::UpdateShadersData()
{
    for (const auto& shaderData : shadersData)
    {
        if (GlobalShadersData.contains(shaderData)) continue;
        GlobalShadersData.insert(shaderData);
    }
}
