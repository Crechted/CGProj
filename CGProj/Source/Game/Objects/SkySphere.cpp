#include "SkySphere.h"

#include "Core/Components/TriangleComponent.h"


SkySphere::SkySphere(const wchar_t* pathTex, float rad, int32_t slice, int32_t stack, bool FirstHalf, bool SecondHalf,
    bool dUp, bool dDown)
{
    radius = rad;
    sliceCount = slice;
    stackCount = stack;
    drawFirstHalf = FirstHalf;
    drawSecondHalf = SecondHalf;
    drawUp = dUp;
    drawDown = dDown;
    initPathTex = pathTex;
    triangleComp->cullMode = D3D11_CULL_BACK;
}
