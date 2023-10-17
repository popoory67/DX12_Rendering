//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

struct Constants
{
    float4x4 WorldViewProj;
};

ConstantBuffer<Constants> Globals : register(b0);

struct VertexIn
{
    float3 Position     : POSITION;
    float3 Normal       : NORMAL;
    float2 TexCoord     : TEXCOORD;
};

struct VertexOut
{
    float4 Position     : SV_POSITION;
    //float3 PosW         : POSITION;
    float3 Normal       : NORMAL;
    float2 TexCoord     : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;

    vout.Position = float4(vin.Position, 1.0f);
    vout.Normal = vin.Normal;

    //----
    //// Transform to world space.
    //float4 posW = mul(float4(vin.Position, 1.0f), gWorld);
    //vout.PosW = posW.xyz;

    //// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    //vout.NormalW = mul(vin.Normal, (float3x3)gWorld);

    //// Transform to homogeneous clip space.
    //vout.PosH = mul(posW, gViewProj);

    //// Output vertex attributes for interpolation across triangle.
    //float4 texC = mul(float4(vin.TexCoord, 0.0f, 1.0f), gTexTransform);
    //vout.TexCoord = mul(texC, gMatTransform).xy;

    //VertexOut vout;
    //vout.Position = mul(float4(vin.Position, 1), Globals.WorldViewProj);
    //vout.Normal = vin.Normal;

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    // lambert's cosine test
    float3 normal = normalize(pin.Normal);

    float3 lightDirection = float3(0, -1, 0); // test directional light
    float diffuseAmount = max(dot(normal, lightDirection), 0.0f);

    float4 color = float4(1.0f, 1.0f, 0, 0);
    float4 finalColor = color * diffuseAmount;

    return finalColor;
}
