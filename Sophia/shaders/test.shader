Shader "Hidden/Skybox"
{
    Properties
    {
        _MainTex("Main Texture", 2D) = "white"{}
    }

    Pass
    {
        HLSLPROGRAM

        Name "Skybox"
        #pragma vertex   vs_main
        #pragma fragment ps_main

        #define TWO_PI 6.2831853072
        #define PI     3.1415926536

        struct vs_output
        {
            float2 uv       : TEXCOORD0;
            float4 vertex   : SV_POSITION;
        };

        float4x4     cameraToWorld;
        float        tanHalfFovX;
        float        tanHalfFovY;
        Texture2D    envMap;
        sampler      sampler_envMap;

        vs_output vs_main(uint id : SV_VERTEXID)
        {
            vs_output output;

            // generate clip space position
            output.vertex.x = (float)(id / 2) * 4.0f - 1.0f;
            output.vertex.y = (float)(id % 2) * 4.0f - 1.0f;
            output.vertex.z = 1.0f;
            output.vertex.w = 1.0f;

            // texture coordinate
            output.uv.x = (float)(id / 2) * 2.0f;
            output.uv.y = 1.0f - (float)(id % 2) * 2.0f;
            
            return output;
        }

        float4 ps_main(float2 uv : TEXCOORD0) : SV_Target
        {
            float3 camera_world_pos = mul(float4(0.0f,0.0f,0.0f,1.0f), cameraToWorld).xyz;
            float2 ndc = float2(2 * uv.x - 1.0f , 1.0f - 2 * uv.y);
            float3 pixel_world_pos = mul(float4(ndc.x * tanHalfFovX, ndc.y * tanHalfFovY, 1.0f, 1.0f), cameraToWorld).xyz;

            float3 ray_world = normalize(pixel_world_pos - camera_world_pos);

            float theta = acos(clamp(ray_world.y,-1.0f, 1.0f));
            float phi = atan2(ray_world.z, ray_world.x);
            phi = phi < 0.0f ? phi + TWO_PI : phi;

            float4 c = envMap.Sample(sampler_envMap, float2(phi / TWO_PI, theta / PI));

            // tone mapping
            c = c / (c + 1.0f);
            // gamma correct
            c = pow(c, 1.0f/2.2f);

            return c;
        }
            
        ENDHLSL
    }
}