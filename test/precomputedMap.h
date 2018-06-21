#pragma once
#include <Sophia.h>
#include <t3Vector2.h>
#include <t3Vector3.h>
#include <t3Matrix4x4.h>
#include <core/log/s3Log.h>

#define DIFFUSE
#define SPECULAR

int main()
{
    s3App app;
    if (!app.init(t3Vector2f(1600, 900), t3Vector2f(10, 10)))
        return 0;
    app.setClearColor(t3Vector4f(0.2f));

    s3Renderer& renderer = s3Renderer::get();

    int32 width = 0, height = 0;
    s3ImageDecoder decoder;
    decoder.load(renderer.getDevice(), "../resources/newport_loft.hdr");
    //decoder.load(renderer.getDevice(), "../resources/skylightBlue.exr");
    width = decoder.getWidth();
    height = decoder.getHeight();

    s3ImageEncoder encoder(width, height, s3ImageType::S3_IMAGE_EXR);

    // tangent coordinate
    t3Vector3f normal, up, right, sampledLocal, sampledWorld;
    t3Vector4f irradiance;
    float32 samples = 100;
    // whole sphere
    //for (float32 phi = 0.0f; phi < T3MATH_2PI; phi += T3MATH_2PI / width)
    for (int32 i = 0; i < width; i++)
    {
        //s3Log::info("Process: %8.2f \r", phi / T3MATH_2PI * 100);
        s3Log::info("Process: %8.2f \r", i / (float32)width * 100);

        for (int32 j = 0; j < height; j++)
        //for (float32 theta = 0.0f; theta < T3MATH_PI; theta += T3MATH_PI / height)
        {
            float32 phi = T3MATH_2PI * i / (float32) width, theta = T3MATH_PI * j / (float32) height;

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
            int32 x = phi * (width -1)/ T3MATH_2PI, y = theta * (height - 1) / T3MATH_PI;
            encoder.setColor(x, y, irradiance);
        }
    }

    //encoder.print();
    encoder.write("../resources/irradianceDiffuseMap1.exr");
    //encoder.write("../resources/irradianceDiffuseMap.exr");

    return 0;
}