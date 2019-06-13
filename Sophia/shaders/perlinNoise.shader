Shader "Cloud/PerlinNoise"
{
    Properties {}

    Pass
    {
        HLSLPROGRAM

    	Name "Perlin Noise"
		#pragma vertex   vert
        #pragma fragment frag

    	struct vertInput
    	{
			uint id : SV_VertexID;
    	};

        struct fragInput
        {
		    float2 texCoord : TEXCOORD0;
		    float4 position : SV_POSITION;
        };

        fragInput vert(vertInput input)
        {
        	fragInput output;

			output.texCoord = float2((input.id << 1) & 2, input.id & 2);
			output.position = float4(output.texCoord * float2(2, -2) + float2(-1, 1), 0, 1);
            
            return output;
        }

        float4 frag(fragInput input) : SV_Target
        {
        	// return float4(1.0f, 0.0f, 0.0f, 1.0f);
        	return float4(input.texCoord, 0.0f, 1.0f);
        	// return float4(1.0f, 0.0f, 0.0f, 1.0f);
        }
        
        ENDHLSL
    }
}