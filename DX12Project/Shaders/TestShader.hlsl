
struct GlobalConstants
{
    float4x4 WorldViewProj;
};

ConstantBuffer<GlobalConstants> Globals : register(b0);

cbuffer Transform : register(b1)
{
    float4x4 VertexTransform;
}

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

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

    // Transform
    {
        vout.Position = mul(float4(vin.Position, 1.0f), Globals.WorldViewProj);
        vout.Normal = float4(vin.Normal, 1.0f);
        vout.TexCoord = vin.TexCoord;
    }
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
//        return g_texture.Sample(g_sampler, pin.TexCoord);

    // lambert's cosine test
    float3 normal = normalize(pin.Normal);

    float3 lightDirection = float3(1, 1, 0); // test directional light
    float diffuseAmount = max(dot(normal, lightDirection), 0.0f);

    float4 color = float4(1.0f, 1.0f, 0, 0);
    float4 finalColor = color * diffuseAmount;

    return finalColor;
}
