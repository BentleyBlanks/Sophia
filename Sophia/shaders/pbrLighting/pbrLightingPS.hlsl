cbuffer cb
{ 
	// material parameters
    float3 albedoCB;
    float metallicCB;
    float roughnessCB;
    float aoCB;
    int useTexture;

    // lights
    float3 lightPositions[4];
    float3 lightColors[4];

    float3 cameraPosition;
}

struct input
{
    float3 fragPos : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

Texture2D albedoMap, normalMap, metallicMap, roughnessMap, aoMap;
sampler textureSampler;

static const float PI = 3.14159265359;

float3 getNormal(input i)
{
    float3 tangentNormal = normalMap.Sample(textureSampler, i.texCoord).xyz * 2.0 - 1.0;

    float3 Q1 = ddx(i.fragPos);
    float3 Q2 = ddy(i.fragPos);
    float2 st1 = ddx(i.texCoord);
    float2 st2 = ddy(i.texCoord);

    float3 N = normalize(i.normal);
    float3 T = normalize(Q1 * st2.y - Q2 * st1.y);
    float3 B = -normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);

    return normalize(mul(TBN, tangentNormal));
}

float normalDistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float cosH = max(dot(N, H), 0.0f);
    float cosH2 = cosH * cosH;

    float nom = a2;
    float denom = (cosH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

	// prevent divide by zero for roughness=0.0 and NdotH=1.0
    return nom / max(denom, 0.001f); 
}

float geometrySchlickGGX(float cosV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    return cosV / (cosV * (1 - k) + k);
}

float geometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float cosV = max(dot(N, V), 0.0f);
    float cosL = max(dot(N, L), 0.0f);
	
    float ggx1 = geometrySchlickGGX(cosV, roughness);
    float ggx2 = geometrySchlickGGX(cosL, roughness);

    return ggx1 * ggx2;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float4 main(input i) : SV_TARGET
{
    float3 normal = i.normal;
    float3 V = normalize(cameraPosition - i.fragPos);

    float3 albedo = albedoCB;
    float metallic = metallicCB;
    float roughness = roughnessCB;
    float ao = aoCB;

    if(useTexture)
    {
		// albedo in sRGB(need to convert to linear)
        albedo = pow(albedoMap.Sample(textureSampler, i.texCoord).xyz, float3(2.2f, 2.2f, 2.2f));
        metallic = metallicMap.Sample(textureSampler, i.texCoord).r;
        roughness = roughnessMap.Sample(textureSampler, i.texCoord).r;
        ao = aoMap.Sample(textureSampler, i.texCoord).r;
        normal = getNormal(i);
    }
	
	// interpolate by the metallic
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo, metallic);

	// numerical integration
    float3 Lo = float3(0.0f, 0.0f, 0.0f);
    for (int j = 0; j < 4; j++)
    {
        float3 L = normalize(lightPositions[j] - i.fragPos);
        float3 H = normalize(V + L);

		// attenuation of the point light
        float distance = length(lightPositions[j] - i.fragPos);
        float attenuation = 1.0 / (distance * distance);
        float3 radiance = lightColors[j] * attenuation;

		// Cook-Torrance BRDF
        float N = normalDistributionGGX(normal, H, roughness);
        float G = geometrySmith(normal, V, L, roughness);
        float3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

		// specular and diffuse
        float3 ks = F;
        float3 kd = float3(1.0f, 1.0f, 1.0f) - ks;
        kd *= 1.0f - metallic;

        float cosL = max(dot(normal, L), 0.0);
        float cosV = max(dot(normal, V), 0.0);

		// (kd * diffuse + NDF/4*cosV*cosL) * L * cosL
        Lo += radiance * cosL * (kd * albedo / PI + N * G * F / max(4.0f * cosV * cosL, 0.001));
    }

	 // ambient lighting
    float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo * ao;

    float3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + float3(1.0f, 1.0f, 1.0f));
    // gamma correct
    color = pow(color, float3(1.0 / 2.2f, 1.0 / 2.2f, 1.0 / 2.2f));

    return float4(color, 1.0);
}