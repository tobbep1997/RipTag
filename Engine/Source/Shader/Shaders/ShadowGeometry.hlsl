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
static const uint maxLight = 8;

[maxvertexcount(maxLight * 6 * 3)]
void main(
	triangle float4 input[3] : SV_POSITION,
	inout TriangleStream< GSOutput > output
)
{
	bool onScreen = false;

	float4 t[3];

    for (int lights = 0; lights < numberOfLights; lights++)
    {
        for (int targetMatrix = 0; targetMatrix < 6; targetMatrix++)
        {
            GSOutput element;
            element.RTIndex = (lights * 6) + targetMatrix;
			/*
			t[0] = mul(input[0], lightViewProjection[lights][targetMatrix]);
			t[1] = mul(input[1], lightViewProjection[lights][targetMatrix]);
			t[2] = mul(input[2], lightViewProjection[lights][targetMatrix]);
			
			onScreen = false;
			for (int i = 0; i < 3 && !onScreen; i++)
			{
				if (abs(t[i]).x <= 1 || abs(t[i]).y <= 1)
					onScreen = true;
			}*/

            for (int vertex = 0; vertex < 3; vertex++)
            {
                element.pos = mul(input[vertex], lightViewProjection[lights][targetMatrix]);
                output.Append(element);
            }


            output.RestartStrip();
        }
    }
}