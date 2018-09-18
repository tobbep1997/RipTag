cbuffer LIGHT_MATRIX : register(b0)
{
	float4x4 lightViewProjection[8][6];
    int numberOfLights;
};


struct GSOutput
{
	float4 pos : SV_POSITION;
	uint RTIndex : SV_RenderTargetArrayIndex;
};

// after optimization, change this to 8
static const uint maxLight = 2;

[maxvertexcount(maxLight * 6 * 3)]
void main(
	triangle float4 input[3] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)
{
    for (int lights = 0; lights < numberOfLights; lights++)
    {
        for (int targetMatrix = 0; targetMatrix < 6; targetMatrix++)
        {
            GSOutput element;
            element.RTIndex = (lights * 6) + targetMatrix;
            for (int vertex = 0; vertex < 3; vertex++)
            {
                element.pos = mul(input[vertex], lightViewProjection[lights][targetMatrix]);
                output.Append(element);
            }
            output.RestartStrip();
        }
    }
}