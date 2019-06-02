Shader "Cloud/PerlinNoise"
{
    Properties {}

    Pass
    {
    	Name "Perlin Noise"
		#pragma vertex   vert
        #pragma fragment frag

    	struct vertInput
    	{
			uint id : SV_VERTEXID;
    	};

        struct fragInput
        {
            float2 texCoord : TEXCOORD0;
        };

        fragInput vs_main(vertInput input)
        {
        	fragInput output;

            output.texCoord.x = (float)(input.id / 2) * 2.0f;
            output.texCoord.y = 1.0f - (float)(input.id % 2) * 2.0f;
            
            return output;
        }

        float4 ps_main(fragInput input) : SV_Target
        {
        	return float4(input.texCoord, 0.0f, 1.0f);
        	// return float4(1.0f, 0.0f, 0.0f, 1.0f);
        }
    }
}