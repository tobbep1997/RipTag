
cbuffer CAMERA_BUFFER : register(b2)
{
    float4 cameraPosition;
    float4x4 viewProjection;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : WORLD;
    float4 normal : NORMAL;
    float3x3 TBN : TBN;
    float2 uv : UV;

    float4 color : COLOR;
    int4 info : INFO;
};

struct PatchTess
{
    float EdgeTess[3] : SV_TessFactor;
    float InsideTess : SV_InsideTessFactor;
};

Texture2D diffuseTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D MRATexture : register(t3);
Texture2D displacementMap : register(t4);
SamplerState defaultSampler : register(s1);

[domain("tri")]
VS_OUTPUT DS(PatchTess patchTess,
             float3 bary : SV_DomainLocation,
             const OutputPatch<VS_OUTPUT, 3> tri)
{
    VS_OUTPUT dout = (VS_OUTPUT)0;
    
    // Interpolate patch attributes to generated vertices.
    dout.worldPos = bary.x * tri[0].worldPos + bary.y * tri[1].worldPos + bary.z * tri[2].worldPos;
    dout.worldPos.w = 1;
    dout.normal = bary.x * tri[0].normal + bary.y * tri[1].normal + bary.z * tri[2].normal;
    dout.uv = bary.x * tri[0].uv + bary.y * tri[1].uv + bary.z * tri[2].uv;
    
    // Interpolating normal can unnormalize it, so normalize it.
    dout.normal = normalize(dout.normal);
    dout.normal.w = 0;
    
    dout.TBN = tri[0].TBN; 
    dout.color = tri[0].color;
    dout.info = tri[0].info;

    //
    // Displacement mapping.
    //
    
    // Choose the mipmap level based on distance to the eye; specifically, choose
    // the next miplevel every MipInterval units, and clamp the miplevel in [0,6].
    //const float MipInterval = 20.0f;
    //float mipLevel = clamp((distance(dout.PosW, gEyePosW) - MipInterval) / MipInterval, 0.0f, 6.0f);
    
    // Sample height map (stored in alpha channel).
    if (tri[0].info.z == 1)
    {
        float h = length(displacementMap.SampleLevel(defaultSampler, dout.uv, 0).rbg);

        // Offset vertex along normal.
        dout.worldPos += (0.06f * h) * dout.normal;
    }
    else
    {
        //dout.worldPos += 0.04f * dout.normal;
    }
    
    
    // Project to homogeneous clip space.
    dout.pos = mul(dout.worldPos, viewProjection);
    
    return dout;
}