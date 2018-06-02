#define PI 3.141592653
#define FLOAT_MAX 3.402823466e+38F

struct input
{
    float2 texCoord : TEXCOORD0;
};

struct s3Ray
{
    float3 origin;
    float3 direction;
};

struct s3Sphere
{
    float3 origin;
    float radius;
};

cbuffer skyCB
{
    // For computing perspective rays
    float canvasDistance;
    float tanHalfFovX, tanHalfFovY;

    // atmos + earth's origin
    float3 sphereOrigin;
    matrix cameraToWorld;

	// atmospheric
    float3 sunDirection;
    float earthRadius, atmosphereRadius;
	// Thickness of the atmosphere if density was uniform (Rayleigh + Mie)
    float thicknessR, thicknessM;
	// Rayleigh + Mie extinction coefficient
    float3 betaR, betaM;

	// samples
    float numSampleViewDir, numSampleLight;
    int isToneMapping;
};

bool solveQuadratic(float A, float B, float C, inout float t0, inout float t1)
{
    // --!Copied from Mitsuba(util.cpp 447)
    /* Linear case */
    if (A == 0)
    {
        if (B != 0)
        {
            t0 = t1 = -C / B;
            return true;
        }
        return false;
    }

    float discrim = B * B - 4.0f * A * C;

    /* Leave if there is no solution */
    if (discrim < 0)
        return false;

    float temp, sqrtDiscrim = sqrt(discrim);

    /* Numerically stable version of (-b (+/-) sqrtDiscrim) / (2 * a)
    *
    * Based on the observation that one solution is always
    * accurate while the other is not. Finds the solution of
    * greater magnitude which does not suffer from loss of
    * precision and then uses the identity x1 * x2 = c / a
    */
    // 通过B的状态求得更大的temp来避免出现精度丢失问题
    if (B < 0)
        temp = -0.5f * (B - sqrtDiscrim);
    else
        temp = -0.5f * (B + sqrtDiscrim);

    t0 = temp / A;
    t1 = C / temp;

    /* Return the results so that t0 < t1 */
    if (t0 > t1)
    {
        float t = t0;
        t0 = t1;
        t1 = t;
    }

    return true;
}

// world space
bool raySphereIntersect(s3Ray r, s3Sphere s, inout float nearT, inout float farT)
{
    float3 o = r.origin - s.origin;
    float3 d = r.direction;

    // direction为单位向量，长度为1
    float A = length(r.direction);
    float B = 2 * dot(o, r.direction);
    float C = dot(o, o) - s.radius * s.radius;

    if (!solveQuadratic(A, B, C, nearT, farT))
        return false;

    return true;
}

float4 tonemapping(float4 color)
{
	color.x = color.x < 1.413f ? pow(color.x * 0.38317f, 1.0f / 2.2f) : 1.0f - exp(-color.x);
    color.y = color.y < 1.413f ? pow(color.y * 0.38317f, 1.0f / 2.2f) : 1.0f - exp(-color.y);
    color.z = color.z < 1.413f ? pow(color.z * 0.38317f, 1.0f / 2.2f) : 1.0f - exp(-color.z);
    color.w = 1.0f;

    return color;
}

float4 main(input i) : SV_TARGET
{
    //i.texCoord.x = 393 / 1280.0f;
    //i.texCoord.y = 168 / 720.0f;

    // convert texCoord to ndc
    float ndcX = 2 * i.texCoord.x - 1;
    float ndcY = 1 - 2 * i.texCoord.y;
        
    // correct aspect ratio
    float vx = ndcX * tanHalfFovX * canvasDistance;
    float vy = ndcY * tanHalfFovY * canvasDistance;
	
	// ray camera to world
    s3Ray r;
    r.origin = float3(0, 0, 0);
    r.origin = mul(float4(r.origin, 1.0f), cameraToWorld);
	
	// for percison
    double3 pixelPosition = double3(vx, vy, canvasDistance);
    pixelPosition = mul(double4(pixelPosition, 1.0), cameraToWorld);
    r.direction = normalize(pixelPosition - r.origin);
	
	// km
    s3Sphere earth, atmos;
    earth.origin = sphereOrigin;
    earth.radius = earthRadius;

    atmos.origin = sphereOrigin;
    atmos.radius = atmosphereRadius;

    // compute incident light
    float nearTEarth = -FLOAT_MAX, farTEarth = FLOAT_MAX, minT = 0.0f, maxT = FLOAT_MAX;

	// intersect earth
    if (raySphereIntersect(r, earth, nearTEarth, farTEarth) && farTEarth > 0)
		// atmos range between [0, nearT]
        maxT = max(0.0f, nearTEarth);

	// intersect atmos
    float nearTAtmos = -FLOAT_MAX, farTAtmos = FLOAT_MAX;
    if (!raySphereIntersect(r, atmos, nearTAtmos, farTAtmos) || farTAtmos < 0)
        return float4(0, 0, 0, 1);

    //return float4(0, 0, 1, 1);

    if (nearTAtmos > minT)
        minT = nearTAtmos;
	if (farTAtmos < maxT)
        maxT = farTAtmos;
	
	// attributes for atmospheric
    float segmentLength = (maxT - minT) / numSampleViewDir;
    float currentT = minT;
	
    float3 sumR = float3(0, 0, 0), sumM = float3(0, 0, 0);
    float opticalDepthR = 0, opticalDepthM = 0;

	// phase function for Rayleigh ang Mie
    float mu = dot(r.direction, sunDirection);
    float phaseR = 3.f / (16.f * PI) * (1 + mu * mu);
    float g = 0.76f;
    float phaseM = 3.f / (8.f * PI) * ((1.f - g * g) * (1.f + mu * mu)) / ((2.f + g * g) * pow(1.f + g * g - 2.f * g * mu, 1.5f));
	
    for (int i = 0; i < numSampleViewDir; i++)
    {
        float3 samplePosition = r.origin + (currentT + segmentLength * 0.5f) * r.direction;

		// height of sampled point(m)
        float height = (length(samplePosition - sphereOrigin) - earth.radius);
		// accumulative opticalDepth
        float hr = exp(-height / thicknessR) * segmentLength;
        float hm = exp(-height / thicknessM) * segmentLength;
        opticalDepthR += hr;
        opticalDepthM += hm;

        s3Ray sampledRay;
        sampledRay.direction = normalize(sunDirection);
        sampledRay.origin = samplePosition;
		
        // light optical depth
        float lightAtmosT0 = -FLOAT_MAX, lightAtmosT1 = FLOAT_MAX;
        // earth intersect would be discarded
        raySphereIntersect(sampledRay, atmos, lightAtmosT0, lightAtmosT1);
        float segmentLengthLight = lightAtmosT1 / numSampleLight, currentLightT = 0;
        float opticalDepthLightR = 0, opticalDepthLightM = 0;
		
        int j;
        for (j = 0; j < numSampleLight; j++)
        {
            float3 samplePositionLight = samplePosition + (currentLightT + segmentLengthLight * 0.5f) * sunDirection;
            
			// m
			float lightHeight = (length(samplePositionLight - earth.origin) - earth.radius);
            if (lightHeight < 0)
                break;
			
            opticalDepthLightR += exp(-lightHeight / thicknessR) * segmentLengthLight;
            opticalDepthLightM += exp(-lightHeight / thicknessM) * segmentLengthLight;

            currentLightT += segmentLengthLight;
        }

        if (j == numSampleLight)
        {
            float3 tau = betaR * (opticalDepthR + opticalDepthLightR) + betaM * 1.1f * (opticalDepthM + opticalDepthLightM);
            float3 attenuation = exp(-tau);
            sumR += attenuation * hr;
            sumM += attenuation * hm;
        }
        currentT += segmentLength;
    }

	if(isToneMapping)
		return tonemapping(float4(float3(sumR * betaR * phaseR + sumM * betaM * phaseM) * 20, 1));
	else
        return float4(float3(sumR * betaR * phaseR + sumM * betaM * phaseM) * 20, 1);

    //return float4(0, 0, 1, 1);
    //return float4(i.texCoord.x, i.texCoord.y, 0, 1.0f);
}