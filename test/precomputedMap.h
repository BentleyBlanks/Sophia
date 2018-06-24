#pragma once
#include <Sophia.h>
#include <t3Vector2.h>
#include <t3Vector3.h>
#include <t3Matrix4x4.h>
#include <core/s3Random.h>
#include <core/log/s3Log.h>

//#define DIFFUSE
#define SPECULAR
//#define SPUCULAR_BRDF

float32 distributionGGX(t3Vector3f N, t3Vector3f H, float roughness)
{
    float32 a = roughness*roughness;
    float32 a2 = a*a;
    float32 NdotH = t3Math::Max(N.dot(H), 0.0f);
    float32 NdotH2 = NdotH*NdotH;

    float32 nom = a2;
    float32 denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float32 geometrySchlickGGX(float32 NoV, float32 roughness)
{
    float32 a = roughness;
    float32 k = (a * a) / 2.0f;

    float32 nom = NoV;
    float32 denom = NoV * (1.0f - k) + k;

    return nom / denom;
}

float32 geometrySmith(t3Vector3f N, t3Vector3f V, t3Vector3f L, float roughness)
{
    float32 NoV = t3Math::Max(N.dot(V), 0.0f);
    float32 NoL = t3Math::Max(N.dot(L), 0.0f);
    float32 ggx2 = geometrySchlickGGX(NoV, roughness);
    float32 ggx1 = geometrySchlickGGX(NoL, roughness);

    return ggx1 * ggx2;
}

float32 radicalInverseVdC(uint32 bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float32(bits) * 2.3283064365386963e-10; // / 0x100000000
}

t3Vector2f hammersley(uint32 i, uint32 N)
{
    return t3Vector2f(float32(i) / float32(N), radicalInverseVdC(i));
}

t3Vector3f importanceSampleGGX(t3Vector2f Xi, t3Vector3f N, float roughness)
{
    float a = roughness*roughness;

    float phi = 2.0 * PI * Xi.x;
    float cosTheta = t3Math::sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = t3Math::sqrt(1.0 - cosTheta*cosTheta);

    // from spherical coordinates to cartesian coordinates
    t3Vector3f H;
    H.x = t3Math::cosRad(phi) * sinTheta;
    H.y = cosTheta;
    H.z = t3Math::sinRad(phi) * sinTheta;

    // from tangent-space vector to world-space sample vector
    t3Vector3f dir = t3Math::Abs(N.z) < 0.999 ? t3Vector3f(0.0, 0.0, 1.0) : t3Vector3f(1.0, 0.0, 0.0);
    t3Vector3f right = N.getCrossed(dir).getNormalized();
    dir = right.getCrossed(N);

    t3Vector3f sampleVec = right * H.x + N * H.y + dir * H.z;
    return sampleVec.getNormalized();
}

t3Vector4f integrateBRDF(float32 NoV, float32 roughness, int32 samples)
{
    // V on XoY plane
    t3Vector3f V(t3Math::sqrt(1.0f - NoV * NoV), NoV, 0.0f);

    float32 A = 0.0f, B = 0.0f;

    t3Vector3f N(0.0f, 1.0f, 0.0f);

    for (int32 i = 0; i < samples; ++i)
    {
        t3Vector2f random = hammersley(i, samples);
        t3Vector3f H = importanceSampleGGX(random, N, roughness);
        t3Vector3f L = (2.0 * V.dot(H) * H - V).getNormalized();

        float32 NoL = t3Math::Max(L.y, 0.0f);
        float32 NoH = t3Math::Max(H.y, 0.0f);
        float32 VoH = t3Math::Max(V.dot(H), 0.0f);

        if (NoL > 0.0f)
        {
            float32 G = geometrySmith(N, V, L, roughness);
            float32 GVisibility = (G * VoH) / (NoV * NoH);
            float32 Fc = t3Math::pow(1.0f - VoH, 5.0f);

            A += (1.0f - Fc) * GVisibility;
            B += Fc * GVisibility;
        }
    }

    return t3Vector4f(A, B, 0.0f, 0.0f) / (float32) samples;
}

int main()
{
    s3App app;
    if (!app.init(t3Vector2f(100, 100), t3Vector2f(10, 10)))
        return 0;
    app.setClearColor(t3Vector4f(0.2f));

    s3Renderer& renderer = s3Renderer::get();

    int32 width = 0, height = 0;
    s3ImageDecoder decoder;
    decoder.load(renderer.getDevice(), "../resources/grace-new_latlong.exr");
    //decoder.load(renderer.getDevice(), "../resources/skylightBlue.exr");
    width = decoder.getWidth();
    height = decoder.getHeight();


#ifdef DIFFUSE
    float32 samples = 100;
    s3ImageEncoder encoder(width, height, s3ImageType::S3_IMAGE_EXR);
    // whole sphere
    //for (float32 phi = 0.0f; phi < T3MATH_2PI; phi += T3MATH_2PI / width)
#pragma omp parallel for schedule(dynamic)

    for (int32 i = 0; i < width; i++)
    {
        // tangent coordinate
        t3Vector3f normal, up, right, sampledLocal, sampledWorld;
        t3Vector4f irradiance;

        //s3Log::info("Process: %8.2f \r", phi / T3MATH_2PI * 100);
        s3Log::info("Process: %8.2f \r", i / (float32)width * 100);

        for (int32 j = 0; j < height; j++)
            //for (float32 theta = 0.0f; theta < T3MATH_PI; theta += T3MATH_PI / height)
        {
            float32 phi = T3MATH_2PI * i / (float32)width, theta = T3MATH_PI * j / (float32)height;

            up.set(t3Math::sinRad(theta) * t3Math::cosRad(phi),
                t3Math::cosRad(theta),
                t3Math::sinRad(theta) * t3Math::sinRad(phi));
            up.normalize();

            // avoid coincide normal and up
            if (phi != T3MATH_2PI && theta != T3MATH_2PI)
                normal.set(0.0f, 0.0f, 1.0f);
            else
                normal.set(0.0f, -1.0f, 0.0f);

            right = up.getCrossed(normal).getNormalized();
            normal = right.getCrossed(up).getNormalized();

            // diffuse irradiance compute hemisphere
            for (float32 phiLocal = 0.0f; phiLocal < T3MATH_2PI; phiLocal += T3MATH_2PI / samples)
            {
                for (float32 thetaLocal = 0.0f; thetaLocal < T3MATH_PI_DIV2; thetaLocal += T3MATH_PI_DIV2 / samples)
                {
                    sampledLocal.set(t3Math::sinRad(thetaLocal) * t3Math::cosRad(phiLocal),
                        t3Math::cosRad(thetaLocal),
                        t3Math::sinRad(thetaLocal) * t3Math::sinRad(phiLocal));
                    sampledLocal.normalize();

                    t3Matrix4x4 test(right.x, up.x, normal.x, 0,
                        right.y, up.y, normal.y, 0,
                        right.z, up.z, normal.z, 0,
                        0, 0, 0, 1);

                    sampledWorld = test * sampledLocal;

                    //sampledWorld.set(sampledLocal.x * right + sampledLocal.y * up + sampledLocal.z * normal);
                    sampledWorld.normalize();

                    int32 x = s3SphericalPhi(sampledWorld) / T3MATH_2PI * (decoder.getWidth() - 1), y = s3SphericalTheta(sampledWorld) / T3MATH_PI * (decoder.getHeight() - 1);
                    irradiance += decoder.getColor(x, y) * t3Math::cosRad(thetaLocal) * t3Math::sinRad(thetaLocal);
                }
            }
            irradiance = T3MATH_PI * irradiance / (samples * samples);
            //int32 x = phi * (width - 1) / T3MATH_2PI, y = theta * (height - 1) / T3MATH_PI;
            //encoder.setColor(x, y, irradiance);
            encoder.setColor(i, j, irradiance);
        }
    }

    //encoder.print();
    encoder.write("../resources/irradiance1.exr");
    //encoder.write("../resources/irradianceDiffuseMap.exr");
#elif defined SPECULAR
    int32 sampleCount = 30000;
    int32 maxMipLevels = 8;

#pragma omp parallel for schedule(dynamic)
    for (int32 mip = 0; mip < maxMipLevels; mip++)
    {
        s3ImageEncoder encoder(width, height, s3ImageType::S3_IMAGE_EXR);

        float32 roughness = (float32)mip / (float32)(maxMipLevels - 1);
        //float32 roughness = 0.1f;

        for (int32 i = 0; i < width; i++)
        {
            s3Log::info("Process: %8.2f \r", i / (float32)width * 100);

            for (int32 j = 0; j < height; j++)
            {
                float32 phi = T3MATH_2PI * i / (float32)width, theta = T3MATH_PI * j / (float32)height;

                t3Vector3f N(t3Math::sinRad(theta) * t3Math::cosRad(phi),
                    t3Math::cosRad(theta),
                    t3Math::sinRad(theta) * t3Math::sinRad(phi));
                t3Vector3f V = N;

                float32 sumWeight = 0.0;
                t3Vector4f prefilteredColor;
                for (int32 s = 0; s < sampleCount; s++)
                {
                    t3Vector2f random = hammersley(s, sampleCount);
                    t3Vector3f H = importanceSampleGGX(random, N, roughness);

                    t3Vector3f L = (2.0 * V.dot(H) * H - V).getNormalized();

                    float32 cosL = t3Math::Max(N.dot(L), 0.0f);
                    if (cosL > 0.0)
                    {
                        // sample from the environment's mip level based on roughness/pdf
                        //float32 D = distributionGGX(N, H, roughness);
                        //float32 NdotH = t3Math::Max(N.dot(H), 0.0f);
                        //float32 HdotV = t3Math::Max(H.dot(V), 0.0f);
                        //float32 pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

                        //float32 resolution = 512.0; // resolution of source cubemap (per face)
                        //float32 saTexel = 4.0 * PI / (6.0 * resolution * resolution);
                        //float32 saSample = 1.0 / (float32(sampleCount) * pdf + 0.0001);

                        //float32 mipLevel = roughness == 0.0 ? 0.0 : 0.5 * t3Math::log2(saSample / saTexel);

                        t3Vector2f texCoord(s3SphericalPhi(L) / T3MATH_2PI * (decoder.getWidth() - 1), s3SphericalTheta(L) / T3MATH_PI * (decoder.getHeight() - 1));
                        prefilteredColor += decoder.getColor(texCoord.x, texCoord.y) * cosL;

                        sumWeight += cosL;
                    }
                }

                prefilteredColor = prefilteredColor / sumWeight;
                encoder.setColor(i, j, prefilteredColor);
            }
        }

        std::string fileName = "../resources/lut/specular2/";
        fileName += s3ToString(mip);
        fileName += ".exr";
        encoder.write(fileName);
        s3Log::print("\n");
    }
#elif defined SPUCULAR_BRDF

    int32 samples = 1000;
    s3ImageEncoder encoder(1024, 1024, s3ImageType::S3_IMAGE_EXR);
    int32 encoderWidth = encoder.getWidth(), encoderHeight = encoder.getHeight();

#pragma omp parallel for schedule(dynamic)
    for (int32 i = 0; i < encoderWidth; i++)
    {
        s3Log::info("Process: %8.2f \r", i / (float32)encoderWidth * 100);

        for (int32 j = 0; j < encoderHeight; j++)
        {
            float32 NoV = ((float32)i + 0.5f) / (float32)encoderWidth;
            float32 roughness = ((float32)j + 0.5f) / (float32)encoderHeight;

            encoder.setColor(i, encoderHeight - 1 - j, integrateBRDF(NoV, roughness, samples));
        }
    }

    encoder.write("../resources/specularMap/brdfMap.exr");
#endif
    return 0;
}