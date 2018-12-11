
struct VS_OUTPUT
{
    float4 pos      : SV_POSITION;
    float4 worldPos : WORLD;
    float4 normal   : NORMAL;
    float3x3 TBN    : TBN;
    float2 uv       : UV;

    float4 color    : COLOR;
    int4 info       : INFO;

    float TessFactor : TESS;
};

struct HS_OUTPUT
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
    float EdgeTess[3]   : SV_TessFactor;
    float InsideTess    : SV_InsideTessFactor;
};

PatchTess PatchHS(InputPatch<VS_OUTPUT, 3> patch,
                  uint patchID : SV_PrimitiveID)
{
    PatchTess pt = (PatchTess)0;
    
    // Average tess factors along edges, and pick an edge tess factor for 
    // the interior tessellation.  It is important to do the tess factor
    // calculation based on the edge properties so that edges shared by 
    // more than one triangle will have the same tessellation factor.  
    // Otherwise, gaps can appear.
    pt.EdgeTess[0] = 0.5f * (patch[1].TessFactor + patch[2].TessFactor);
    pt.EdgeTess[1] = 0.5f * (patch[2].TessFactor + patch[0].TessFactor);
    pt.EdgeTess[2] = 0.5f * (patch[0].TessFactor + patch[1].TessFactor);

    //float tessFactor = 4;
    //
    //pt.EdgeTess[0] = tessFactor;
    //pt.EdgeTess[1] = tessFactor;
    //pt.EdgeTess[2] = tessFactor;
    pt.InsideTess = pt.EdgeTess[0];
    
    return pt;
}

//[partitioning("fractional_odd")]
[domain("tri")]
[partitioning("pow2")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchHS")]
HS_OUTPUT HS(InputPatch<VS_OUTPUT, 3> p,
           uint i : SV_OutputControlPointID,
           uint patchId : SV_PrimitiveID)
{
    HS_OUTPUT hout;
    
    // Pass through shader.
    hout.pos        = p[i].pos;
    hout.worldPos   = p[i].worldPos;
    hout.normal     = p[i].normal;
    hout.TBN        = p[i].TBN;
    hout.uv         = p[i].uv;
    hout.color      = p[i].color;
    hout.info       = p[i].info;
    
    return hout;
}