#include "../StaticConstantBuffers.hlsli"

cbuffer LIGHT_INDEX : register(b13)
{
    uint4 LIGHT_POS;    
    uint4 sides[8];
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

[maxvertexcount(6 * 3)]
void main(
	triangle float4 input[3] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)
{
    [untroll]
    for (int targetMatrix = 0; targetMatrix < numberOfViewProjection[LIGHT_POS.x].x; targetMatrix++)
    {
        if(sides[targetMatrix].x == 0)
            continue;
        GSOutput element;
        element.RTIndex = targetMatrix;
        [untroll]
        for (int vertex = 0; vertex < 3; vertex++)
        {
            element.pos = mul(input[vertex], lightViewProjection[LIGHT_POS.x][targetMatrix]);
            output.Append(element);
        }			
		output.RestartStrip();
    }
}