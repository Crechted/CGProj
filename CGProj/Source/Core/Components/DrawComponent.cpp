#include "DrawComponent.h"

#include <iostream>
#include <d3d.h>
#include <d3dcompiler.h>

#include "TriangleComponent.h"
#include "Core/CoreTypes.h"
#include "Core/Engine.h"
#include "Core/Windisplay.h"

DrawComponent::DrawComponent()
{
}

DrawComponent::~DrawComponent()
{
    DestroyResource();
}

void DrawComponent::Initialize()
{
    if (!CompileVertexBC())
        return;

    if (!CompilePixelBC())
        return;

    CreateShaders();
    CreateLayout();
    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateAndSetRasterizerState();

    curDrawData = new TriangleDrawData();
    curDrawData->rastState = rastState;
    curDrawData->layout = layout;
    curDrawData->vertexShader = vertexShader;
    curDrawData->pixelShader = pixelShader;
    curDrawData->vertexBuffer = vertexBuffer;
    curDrawData->indexBuffer = indexBuffer;
    drawsData.insert(engInst->GetIdxCurrentPipeline(), curDrawData);
}

void DrawComponent::DestroyResource()
{
    for (const auto element : drawsData)
    {
        if (element->rastState) element->rastState->Release();
        if (element->layout) element->layout->Release();
        if (element->vertexShader) element->vertexShader->Release();
        if (element->pixelShader) element->pixelShader->Release();
        if (element->vertexBuffer) element->vertexBuffer->Release();
        if (element->indexBuffer) element->indexBuffer->Release();
        if (vertexBC) vertexBC->Release();
        if (pixelBC) pixelBC->Release();
    }
}

void DrawComponent::Draw()
{
    if (!engInst)
        return;

    UpdateData();
    engInst->GetContext()->RSSetState(rastState);
    engInst->GetContext()->IASetInputLayout(layout);
    engInst->GetContext()->IASetPrimitiveTopology(topology);
    engInst->GetContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    engInst->GetContext()->IASetVertexBuffers(0, 1, &vertexBuffer, strides, offsets);
    engInst->GetContext()->VSSetShader(vertexShader, nullptr, 0);
    engInst->GetContext()->PSSetShader(pixelShader, nullptr, 0);

    engInst->GetContext()->DrawIndexed(indexes.size(), 0, 0);
}

void DrawComponent::Reload()
{
}

void DrawComponent::UpdateData()
{
    curDrawData = drawsData[engInst->GetIdxCurrentPipeline()];
    rastState = curDrawData->rastState;
    layout = curDrawData->layout;
    vertexShader = curDrawData->vertexShader;
    pixelShader = curDrawData->pixelShader;
    vertexBuffer = curDrawData->vertexBuffer;
    indexBuffer = curDrawData->indexBuffer;
}

void DrawComponent::Update(float timeTick)
{
    UpdateData();
    totalTime = engInst->GetTotalTime();
}

void DrawComponent::AddVertex(const VertexNoTex& vertex)
{
    const D3DMinVertex vert{{vertex.position.x, vertex.position.y, vertex.position.z, vertex.position.w},
                            {vertex.color.x, vertex.color.y, vertex.color.z, vertex.color.w}};
    vertices.insert(vert);
    /*vertices.insert(vertex.position);
    vertices.insert(vertex.color);*/
}

void DrawComponent::AddVertex(const DirectX::XMFLOAT4& pos, const DirectX::XMFLOAT4& col)
{
    const D3DMinVertex vert{pos, col};
    vertices.insert(vert);
    /*vertices.insert(pos);
    vertices.insert(col);*/
}

void DrawComponent::SetVertices(const Array<VertexNoTex>& verts)
{
    for (const auto& vert : verts)
    {
        const D3DMinVertex tempVert{{vert.position.x, vert.position.y, vert.position.z, vert.position.w},
                                    {vert.color.x, vert.color.y, vert.color.z, vert.color.w}};
        vertices.insert(tempVert);
    }
}

void DrawComponent::SetVertices(const Array<DirectX::XMFLOAT4>& pts)
{
    for (int32_t i = 0; i < pts.size() - 1; i += 2)
    {
        const D3DMinVertex tempVert{{pts[i].x, pts[i].y, pts[i].z, pts[i].w},
                                    {pts[i + 1].x, pts[i + 1].y, pts[i + 1].z, pts[i + 1].w},};
        vertices.insert(tempVert);
    }
}

void DrawComponent::AddIndex(int32_t idx)
{
    indexes.insert(idx);
}

void DrawComponent::SetVertices(DirectX::XMFLOAT4* pts, int32_t count)
{
    for (int32_t i = 0; i < count - 1; i += 2)
    {
        const D3DMinVertex tempVert{{pts[i].x, pts[i].y, pts[i].z, pts[i].w},
                                    {pts[i + 1].x, pts[i + 1].y, pts[i + 1].z, pts[i + 1].w},};
        vertices.insert(tempVert);
    }
}

void DrawComponent::SetIndexes(int32_t* idxs, int32_t count)
{
    for (int32_t i = 0; i < count; i++)
    {
        indexes.insert(idxs[i]);
    }
}

bool DrawComponent::CompileVertexBC()
{
    vertexBC = nullptr;
    ID3DBlob* errorVertexCode = nullptr;
    const auto res = D3DCompileFromFile(pFileName, nullptr, nullptr,
        VSname, "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexBC,
        &errorVertexCode);

    return CheckResCompile(errorVertexCode, res, pFileName);
}

bool DrawComponent::CompilePixelBC()
{
    D3D_SHADER_MACRO Shader_Macros[] = {"TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr};
    ID3DBlob* errorPixelCode;

    auto res = D3DCompileFromFile(pFileName, Shader_Macros,
        nullptr, PSname,
        "ps_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pixelBC, &errorPixelCode);
    return CheckResCompile(errorPixelCode, res, pFileName);
}

bool DrawComponent::CheckResCompile(ID3DBlob* errorVertexCode, const HRESULT& res, LPCWSTR pFileName)
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

void DrawComponent::CreateShaders()
{
    engInst->GetDevice()->CreateVertexShader(vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(), nullptr, &vertexShader);
    engInst->GetDevice()->CreatePixelShader(pixelBC->GetBufferPointer(), pixelBC->GetBufferSize(), nullptr, &pixelShader);
}

void DrawComponent::CreateLayout()
{
    D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        D3D11_INPUT_ELEMENT_DESC{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        D3D11_INPUT_ELEMENT_DESC{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,
                                 0}};

    engInst->GetDevice()->CreateInputLayout(inputElements, numElements, vertexBC->GetBufferPointer(), vertexBC->GetBufferSize(),
        &layout);
}

void DrawComponent::CreateVertexBuffer()
{
    vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;          // D3D11_USAGE_DEFAULT
    vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // D3D11_BIND_VERTEX_BUFFER
    vertexBufDesc.CPUAccessFlags = 0;
    vertexBufDesc.MiscFlags = 0; // 0
    vertexBufDesc.StructureByteStride = 0;
    vertexBufDesc.ByteWidth = sizeof(D3DMinVertex) * vertices.size();

    vertexData.pSysMem = &vertices[0];
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    engInst->GetDevice()->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);
}

void DrawComponent::CreateIndexBuffer()
{
    indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufDesc.CPUAccessFlags = 0;
    indexBufDesc.MiscFlags = 0;
    indexBufDesc.StructureByteStride = 0;
    indexBufDesc.ByteWidth = sizeof(int32_t) * indexes.size();

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = &indexes[0];
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    engInst->GetDevice()->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);
}

void DrawComponent::CreateAndSetRasterizerState()
{
    strides = new UINT[1]{32};
    offsets = new UINT[1]{0};

    CD3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.CullMode = cullMode; // D3D11_CULL_NONE
    rastDesc.FillMode = fillMode; // D3D11_FILL_SOLID
    rastDesc.AntialiasedLineEnable = isAntialiasedLine;

    auto res = engInst->GetDevice()->CreateRasterizerState(&rastDesc, &rastState);
    //engInst->GetContext()->RSSetState(rastState);
}
