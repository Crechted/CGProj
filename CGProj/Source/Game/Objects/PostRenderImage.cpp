#include "PostRenderImage.h"

#include "Core/Engine.h"
#include "Core/Render/Shader.h"
#include "Utils/DebugDrawing.h"
#include "Core/Render/Buffer.h"

PostRenderImage::PostRenderImage(ID3D11ShaderResourceView* imageSRV, Vector2 viewportDim, Vector2 viewportPos, Vector2 viewportDepth)
    : PostRenderObject(viewportDim, viewportPos, viewportDepth), imageSRV(imageSRV)
{
}

PostRenderImage::PostRenderImage(ID3D11ShaderResourceView* imageSRV, const D3D11_VIEWPORT& viewport)
    : PostRenderObject(viewport), imageSRV(imageSRV)
{
}

void PostRenderImage::Destroy()
{
    if (imageSRV) imageSRV->Release();
    if (imgShader) imgShader->Destroy();
    if (vertBuf) vertBuf->Destroy();
    if (indBuf) indBuf->Destroy();

    PostRenderObject::Destroy();
}

void PostRenderImage::Initialize()
{
    imgShader = new Shader();
    imgShader->AddInputElementDesc("POSITION");
    imgShader->AddInputElementDesc("COLOR");
    imgShader->CreateShader(shaderPath, SVertex);
    imgShader->CreateShader(shaderPath, SPixel);
    DebugDrawing::CreateFullQuad(Vector2(viewport.Width, viewport.Height), vertices, indexes);
    CreateBuffers();
    PostRenderObject::Initialize();
}

void PostRenderImage::Draw()
{
    if (!IsVisible() || !imageSRV) return;
    PostRenderObject::Draw();
    const uint32_t stride = sizeof(VertexNoTex);
    const uint32_t offset = 0;
    imgShader->BindShaders();
    engInst->GetContext()->PSSetShaderResources(0, 1, &imageSRV);
    engInst->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    engInst->GetContext()->IASetVertexBuffers(0, 1, vertBuf->GetBuffers(), &stride, &offset);
    engInst->GetContext()->IASetIndexBuffer(indBuf->GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
    engInst->GetContext()->DrawIndexed(indexes.size(), 0, 0);
    ID3D11ShaderResourceView* srvs[] = {nullptr};
    engInst->GetContext()->PSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
}

void PostRenderImage::CreateBuffers()
{
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = &vertices[0];
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    vertBuf = (new Buffer())->CreateBuffer<VertexNoTex>(D3D11_USAGE_DYNAMIC, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, 0,
        sizeof(VertexNoTex) * vertices.size(), &vertexData);

    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = &indexes[0];
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;
    indBuf = (new Buffer())->CreateBuffer<int32_t>(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER, 0, 0,
        sizeof(VertexNoTex) * vertices.size(), &indexData);
}
