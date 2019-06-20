Shader "Cloud/PerlinNoise"
{
    Properties {}

    Pass
    {
        HLSLPROGRAM

    	  Name "Perlin Noise"
		    #pragma vertex   vert
        #pragma fragment frag
        #define MOD3 float3(.1031,.11369,.13787)

        float time;

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

        float3 hash33(float3 p3)
        {
            p3 = frac(p3 * MOD3);
            p3 += dot(p3, p3.yxz + 19.19);
            return -1.0 + 2.0 * frac(float3((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y, (p3.y + p3.z) * p3.x));
        }

        //--! Ref: https://www.shadertoy.com/view/MdGSzt
        float perlinNoise(float3 p)
        {
            float3 pi = floor(p);
            float3 pf = p - pi;
            float3 w  = pf * pf * (3.0 - 2.0 * pf);
            
            return lerp(
                       lerp(
                           lerp(dot(pf - float3(0, 0, 0), hash33(pi + float3(0, 0, 0))), 
                               dot(pf - float3(1, 0, 0), hash33(pi + float3(1, 0, 0))),
                               w.x),
                           lerp(dot(pf - float3(0, 0, 1), hash33(pi + float3(0, 0, 1))), 
                               dot(pf - float3(1, 0, 1), hash33(pi + float3(1, 0, 1))),
                               w.x),
                           w.z),
                       lerp(
                           lerp(dot(pf - float3(0, 1, 0), hash33(pi + float3(0, 1, 0))), 
                               dot(pf - float3(1, 1, 0), hash33(pi + float3(1, 1, 0))),
                               w.x),
                           lerp(dot(pf - float3(0, 1, 1), hash33(pi + float3(0, 1, 1))), 
                               dot(pf - float3(1, 1, 1), hash33(pi + float3(1, 1, 1))),
                               w.x),
                           w.z),
                       w.y);
        }

        float4 frag(fragInput input) : SV_Target
        {
            float3 position = float3(input.texCoord * 5, time);
            
            float noise = perlinNoise(position);
            return float4(noise.xxx, 1.0f);
            
            // return float4(1.0f, 0.0f, 0.0f, 1.0f);
            // return float4(input.texCoord, 0.0f, 1.0f);
            // return float4(1.0f, 0.0f, 0.0f, 1.0f);
        }
        
        ENDHLSL
    }
}