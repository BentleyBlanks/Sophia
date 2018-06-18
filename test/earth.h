#pragma once
#include <Sophia.h>
#include <core/log/s3Log.h>
#include <t3Vector3.h>
#include <t3Vector2.h>
#include <t3Matrix4x4.h>
#include <t3Math.h>
#include <imgui.h>

#define createConstantBuffer(d, cbClassName, cb)\
{\
    D3D11_BUFFER_DESC constantBufferDesc;\
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));\
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;\
    constantBufferDesc.ByteWidth = sizeof(cbClassName);\
    constantBufferDesc.CPUAccessFlags = 0;\
    constantBufferDesc.MiscFlags = 0;\
    constantBufferDesc.StructureByteStride = 0;\
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;\
    HRESULT hr = d->CreateBuffer(&constantBufferDesc, nullptr, &cb);\
    if (FAILED(hr)){\
        s3Log::error("Failed to create constant buffer\n");\
        return;\
    }\
}

struct s3EarthCB
{
    t3Matrix4x4 mvp;
};

struct s3SkyFromSpaceVSCB
{
    t3Matrix4x4 mvp;
    // -----------------------------

    // camera
    t3Vector3f cameraPosition;
    float cameraHeight2;
    // -----------------------------

    float atmosRadius, atmosRadius2;
    float earthRadius, earthRadius2;
    // -----------------------------

    float invAtmosHeight;
    t3Vector3f lightDirection;
    // -----------------------------

    t3Vector3f betaR;
    float padding1;
    // -----------------------------

    t3Vector3f betaM;
    float padding2;
    // -----------------------------

    t3Vector3f sunIntensity;
    float g;
    // -----------------------------

    float samples;
    float padding3[3];
    // -----------------------------
};

struct s3SkyFromSpacePSCB
{
    float g;
    t3Vector3f lightDirection;
};

// IA
ID3D11Device* device = nullptr;
ID3D11DeviceContext* deviceContext = nullptr;

// State
ID3D11DepthStencilState* depthStencilState = nullptr;
ID3D11RasterizerState* rasterizerState = nullptr;
ID3D11BlendState* blendState = nullptr;

// OM
ID3D11RenderTargetView* renderTargetView = nullptr;
ID3D11DepthStencilView* depthStencilView = nullptr;

ID3D11InputLayout* inputLayout = nullptr;

// Camera
float32 width = 0, height = 0;
s3Camera* camera = nullptr;

// Objects with renderer
s3Mesh *earthMesh = nullptr, *atmosMesh = nullptr;
s3Shader* earthShader = nullptr, *skyFromAtmosShader = nullptr, *skyFromSpaceShader = nullptr;
s3Shader* lutShader = nullptr;

// Constant Buffer Lists
ID3D11Buffer* skyFromSpaceVSCBGPU = nullptr, *skyFromSpacePSCBGPU = nullptr;
s3SkyFromSpaceVSCB skyFromSpaceVSSCBCPU;
s3SkyFromSpacePSCB skyFromSpacePSSCBCPU;

ID3D11Buffer* earthCBGPU = nullptr;
s3EarthCB earhCBCPU;
s3ImageDecoder* earthGround;

// Attributes
t3Vector3f lightDirection(0, 0, 1);
t3Vector3f sunIntensity(20, 20, 20);
float samples = 3;

float atmosRadius = 10.25;
float earthRadius = 10.0f;


float g = -0.990f;
// Rayleigh scattering constant
float Kr = 0.0025f, Km = 0.0010f;
// 650 nm for red / 570 nm for green / 475 nm for blue
t3Vector3f waveLength(0.650f, 0.570f, 0.475f),
           waveLength4(t3Math::pow(waveLength.x, 4.0f),
                       t3Math::pow(waveLength.y, 4.0f), 
                       t3Math::pow(waveLength.z, 4.0f));
t3Vector3f betaR = t3Vector3f(Kr * 4 * T3MATH_PI / waveLength4.x, 
                              Kr * 4 * T3MATH_PI / waveLength4.y, 
                              Kr * 4 * T3MATH_PI / waveLength4.z),
           betaM = t3Vector3f(Km * 4 * T3MATH_PI / waveLength4.x, 
                              Km * 4 * T3MATH_PI / waveLength4.y, 
                              Km * 4 * T3MATH_PI / waveLength4.z);
//t3Vector3f betaR(3.8e-6f, 13.5e-6f, 33.1e-6f), betaM(23.1e-6f);

s3ImageDecoder* lut = nullptr;
int lutAngleSamples = 180, lutAltitudeSamples = 64, lutLightSamples = 50;
float Hr = 0.25f, Hm = 0.1f;

void MakeOpticalDepthBuffer(std::vector<t3Vector4f>& data, float fInnerRadius, float fOuterRadius, float fRayleighScaleHeight, float fMieScaleHeight)
{
    const int nSize = lutAngleSamples;
    const int nSamples = lutLightSamples;
    const float fScale = 1.0f / (fOuterRadius - fInnerRadius);
    //std::ofstream ofScale1("scale1.txt");
    //std::ofstream ofScale2("scale2.txt");

    //Init(nSize, nSize, 1, 4, GL_RGBA, GL_FLOAT);
    int nIndex = 0;
    float fPrev = 0;
    for (int nAngle = 0; nAngle<nSize; nAngle++)
    {
        // As the y tex coord goes from 0 to 1, the angle goes from 0 to 180 degrees
        float fCos = 1.0f - (nAngle + nAngle) / (float)nSize;
        float fAngle = acosf(fCos);
        t3Vector3f vRay(sinf(fAngle), cosf(fAngle), 0);	// Ray pointing to the viewpoint

                                                        /*char szName[256];
                                                        sprintf(szName, "graph%-2.2d.txt", nAngle);
                                                        std::ofstream ofGraph;
                                                        if(fCos >= 0.0f)
                                                        ofGraph.open(szName);
                                                        ofGraph << "# fCos = " << fCos << std::endl;*/

        float fFirst = 0;
        for (int nHeight = 0; nHeight<nSize; nHeight++)
        {
            // As the x tex coord goes from 0 to 1, the height goes from the bottom of the atmosphere to the top
            float fHeight = 1e-6 + fInnerRadius + ((fOuterRadius - fInnerRadius) * nHeight) / nSize;
            t3Vector3f vPos(0, fHeight, 0);				// The position of the camera

                                                        // If the ray from vPos heading in the vRay direction intersects the inner radius (i.e. the planet), then this spot is not visible from the viewpoint
            float B = 2.0f * vPos.dot(vRay);
            float Bsq = B * B;
            float Cpart = vPos.dot(vPos);
            float C = Cpart - fInnerRadius*fInnerRadius;
            float fDet = Bsq - 4.0f * C;
            bool bVisible = (fDet < 0 || (0.5f * (-B - sqrtf(fDet)) <= 0) && (0.5f * (-B + sqrtf(fDet)) <= 0));
            float fRayleighDensityRatio;
            float fMieDensityRatio;
            if (bVisible)
            {
                fRayleighDensityRatio = expf(-(fHeight - fInnerRadius) * fScale / fRayleighScaleHeight);
                fMieDensityRatio = expf(-(fHeight - fInnerRadius) * fScale / fMieScaleHeight);
            }
            else
            {
                // Smooth the transition from light to shadow (it is a soft shadow after all)
                //fRayleighDensityRatio = ((float *)m_pBuffer)[nIndex - nSize*m_nChannels] * 0.5f;
                //fMieDensityRatio = ((float *)m_pBuffer)[nIndex + 2 - nSize*m_nChannels] * 0.5f;
                fRayleighDensityRatio = 0.0f;
                fMieDensityRatio = 0.0f;
            }

            // Determine where the ray intersects the outer radius (the top of the atmosphere)
            // This is the end of our ray for determining the optical depth (vPos is the start)
            C = Cpart - fOuterRadius*fOuterRadius;
            fDet = Bsq - 4.0f * C;
            float fFar = 0.5f * (-B + sqrtf(fDet));

            // Next determine the length of each sample, scale the sample ray, and make sure position checks are at the center of a sample ray
            float fSampleLength = fFar / nSamples;
            float fScaledLength = fSampleLength * fScale;
            t3Vector3f vSampleRay = vRay * fSampleLength;
            vPos += vSampleRay * 0.5f;

            // Iterate through the samples to sum up the optical depth for the distance the ray travels through the atmosphere
            float fRayleighDepth = 0;
            float fMieDepth = 0;
            for (int i = 0; i<nSamples; i++)
            {
                float fHeight = vPos.length();
                float fAltitude = (fHeight - fInnerRadius) * fScale;
                //fAltitude = Max(fAltitude, 0.0f);
                fRayleighDepth += expf(-fAltitude / fRayleighScaleHeight);
                fMieDepth += expf(-fAltitude / fMieScaleHeight);
                vPos += vSampleRay;
            }

            // Multiply the sums by the length the ray traveled
            fRayleighDepth *= fScaledLength;
            fMieDepth *= fScaledLength;

            if (!_finite(fRayleighDepth) || fRayleighDepth > 1.0e25f)
                fRayleighDepth = 0;
            if (!_finite(fMieDepth) || fMieDepth > 1.0e25f)
                fMieDepth = 0;

            // Store the results for Rayleigh to the light source, Rayleigh to the camera, Mie to the light source, and Mie to the camera
            //((float *)m_pBuffer)[nIndex++] = fRayleighDensityRatio;
            //((float *)m_pBuffer)[nIndex++] = fRayleighDepth;
            //((float *)m_pBuffer)[nIndex++] = fMieDensityRatio;
            //((float *)m_pBuffer)[nIndex++] = fMieDepth;

            data.push_back(t3Vector4f(fRayleighDensityRatio, fRayleighDepth, fMieDensityRatio, fMieDepth));
            
        }
        //ofGraph << std::endl;
    }
}

void createLUT()
{
    lut = new s3ImageDecoder();
    std::vector<t3Vector4f> data;

    //MakeOpticalDepthBuffer(data, earthRadius, atmosRadius, Hr, Hm);

    s3Sphere earth, atmos;
    earth.origin = t3Vector3f::zero();
    earth.radius = earthRadius;
    
    atmos.origin = t3Vector3f::zero();
    atmos.radius = atmosRadius;
    
    float radiusDiffer = atmosRadius - earthRadius;
    
    // Vertical Angle
    float currentAngle = 0;
    for (int i = 0; i < lutAngleSamples; i++)
    {
        // Altitude
        float currentHeight = earthRadius + 1e-6;
        for (int j = 0; j < lutAltitudeSamples; j++)
        {
            float densityR = 0.0f, densityM = 0.0f;
            float opticalDepthLightR = 0.0f, opticalDepthLightM = 0.0f;
    
            // xoy platform(same results for others)
            s3Ray sampledRay;
            sampledRay.direction = t3Vector3f(t3Math::sinDeg(currentAngle), t3Math::cosDeg(currentAngle), 0);
            sampledRay.origin = t3Vector3f(0, currentHeight, 0);
    
            // [0, t] be the ray's sampled range
            float nearTEarth = -FLT_MAX, farTEarth = FLT_MAX;
            if (s3RaySphereIntersect(sampledRay, earth, &nearTEarth, &farTEarth) && farTEarth > 0 && nearTEarth > 0)
            {
                //densityR = 0.0f;
                //densityM = 0.0f;
    
                //densityR = data[i * lutAltitudeSamples + j -1].x * 0.5f;
                //densityM = data[i * lutAltitudeSamples + j - 1].z * 0.5f;
    
                // 0.5 of last one
                if (i * lutAltitudeSamples + j - 1 > 0)
                {
                    densityR = data[i * lutAltitudeSamples + j - 1].x * 0.5f;
                    densityM = data[i * lutAltitudeSamples + j - 1].z * 0.5f;
                }
            }
            else
            {
                densityR = exp(-(currentHeight - earthRadius) / (Hr * radiusDiffer));
                densityM = exp(-(currentHeight - earthRadius) / (Hm * radiusDiffer));
            }
    
            float nearTAtmos = -FLT_MAX, farTAtmos = FLT_MAX;
            s3RaySphereIntersect(sampledRay, atmos, &nearTAtmos, &farTAtmos);
    
            // optical Depth
            float segmentLengthLight = farTAtmos / lutLightSamples, currentT = 0.0f;
            for (int k = 0; k < lutLightSamples; k++)
            {
                t3Vector3f samplesPoint = sampledRay.origin + (currentT + segmentLengthLight * 0.5f) * sampledRay.direction;
    
                float height = samplesPoint.length() - earthRadius;
    
                opticalDepthLightR += exp(-height / (Hr * radiusDiffer));
                opticalDepthLightM += exp(-height / (Hm * radiusDiffer));
    
                currentT += segmentLengthLight;
            }
    
            // sum * ds
            opticalDepthLightR *= segmentLengthLight / radiusDiffer;
            opticalDepthLightM *= segmentLengthLight / radiusDiffer;
    
            if (!_finite(opticalDepthLightR) || opticalDepthLightR > 1.0e25f)
                opticalDepthLightR = 0;
            if (!_finite(opticalDepthLightM) || opticalDepthLightM > 1.0e25f)
                opticalDepthLightM = 0;
    
            currentHeight += radiusDiffer / lutAltitudeSamples;
    
            data.push_back(t3Vector4f(densityR, opticalDepthLightR, densityM, opticalDepthLightM));
        }
    
        currentAngle += 180.0f / lutAngleSamples;
    }

    lut->load(device, lutAltitudeSamples, lutAngleSamples, data);
}

void createConstantBuffers()
{
    createConstantBuffer(device, s3EarthCB, earthCBGPU);

    createConstantBuffer(device, s3SkyFromSpaceVSCB, skyFromSpaceVSCBGPU);
    {    
        // constant buffer initialize
        skyFromSpaceVSSCBCPU.cameraPosition = camera->getOrigin();
        // default sphere in (0, 0, 0)
        skyFromSpaceVSSCBCPU.cameraHeight2 = camera->getOrigin().lengthSquared();

        skyFromSpaceVSSCBCPU.atmosRadius = atmosRadius;
        skyFromSpaceVSSCBCPU.atmosRadius2 = atmosRadius * atmosRadius;
        skyFromSpaceVSSCBCPU.earthRadius = earthRadius;
        skyFromSpaceVSSCBCPU.earthRadius2 = earthRadius * earthRadius;
        skyFromSpaceVSSCBCPU.invAtmosHeight = 1.0f / (atmosRadius - earthRadius);

        skyFromSpaceVSSCBCPU.lightDirection = lightDirection;
        skyFromSpaceVSSCBCPU.betaR = betaR;
        skyFromSpaceVSSCBCPU.betaM = betaM;
        skyFromSpaceVSSCBCPU.sunIntensity = sunIntensity;

        skyFromSpaceVSSCBCPU.g = g;
        skyFromSpaceVSSCBCPU.samples = samples;
    }

    createConstantBuffer(device, s3SkyFromSpacePSCB, skyFromSpacePSCBGPU);
    {
        skyFromSpacePSSCBCPU.g = g;
        skyFromSpacePSSCBCPU.lightDirection = lightDirection;
    }
}

void createShaders()
{
    earthShader = new s3Shader();
    earthShader->load(device, L"../Sophia/shaders/sky/earthVS.hlsl", L"../Sophia/shaders/sky/earthPS.hlsl");    
    
    // Vertex PNT Input layout 
    HRESULT hr = device->CreateInputLayout(
        s3VertexPNT::inputDesc,
        _countof(s3VertexPNT::inputDesc),
        earthShader->getVertexShaderBlob()->GetBufferPointer(),
        earthShader->getVertexShaderBlob()->GetBufferSize(),
        &inputLayout);

    if (FAILED(hr))
    {
        s3Log::error("Input Layout Create Failed\n"); 
        return;
    }

    skyFromSpaceShader = new s3Shader();
    skyFromSpaceShader->load(device, L"../Sophia/shaders/sky/skyFromSpaceVS.hlsl", L"../Sophia/shaders/sky/skyFromSpacePS.hlsl");

    lutShader = new s3Shader();
    lutShader->load(device, L"../Sophia/shaders/sky/lutVS.hlsl", L"../Sophia/shaders/sky/lutPS.hlsl");
}

void createStates()
{
    // Setup depth/stencil state.
    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
    ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
    depthStencilStateDesc.DepthEnable = TRUE;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilStateDesc.StencilEnable = FALSE;

    HRESULT hr = device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState);
    if (FAILED(hr))
    {
        s3Log::error("Failed to create DepthStencil State\n");
        return;
    }

    // Setup rasterizer state.
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state object.
    hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
    if (FAILED(hr))
    {
        s3Log::error("Failed to create Rasterrizer State\n");
        return;
    }

    //// Setup blend state.
    //D3D11_BLEND_DESC blendDesc;
    //ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
    //blendDesc.AlphaToCoverageEnable = false;
    //blendDesc.IndependentBlendEnable = false;
    //blendDesc.RenderTarget[0].BlendEnable = true;
    //blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
    //blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
    //blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    //blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    //blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_COLOR;
    //blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    //blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    //
    //hr = device->CreateBlendState(&blendDesc, &blendState);
    //if (FAILED(hr))
    //{
    //    s3Log::error("Failed to create Blend State\n");
    //    return;
    //}
}

void destroy()
{
    S3_SAFE_RELEASE(depthStencilState);
    S3_SAFE_RELEASE(rasterizerState);
    S3_SAFE_RELEASE(blendState);

    S3_SAFE_RELEASE(inputLayout);
}

class s3Sky : public s3CallbackHandle
{
public:
    void gui()
    {
        ImGui::Begin("HHHH");
        {
            ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

            //static float speedKeyboard = 0.5f;
            //if (ImGui::DragFloat("Camera Keyboard Speed", &speedKeyboard, 0.1f, 0.01f, 1))
            //    camera->setKeyboardSpeed(speedKeyboard);

            //static float speedMouse = 0.01f;
            //if (ImGui::DragFloat("Camera Mouse Speed", &speedMouse, 0.01f, 0.01f, 1.0f))
            //    camera->setMouseSpeed(speedMouse);



            ImGui::DragFloat("g", &skyFromSpacePSSCBCPU.g, 0.01f, -0.99f, 0.99f);

            if (ImGui::DragFloat3("xxx", &skyFromSpaceVSSCBCPU.lightDirection.x, 0.1f, -2, 2))
                skyFromSpaceVSSCBCPU.lightDirection.normalize();

            ImGui::End();
        }
    }

    void onHandle(const s3CallbackUserData* userData)
    {
        // lut test
        //{
        //    // IA
        //    deviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
        //    deviceContext->IASetIndexBuffer(NULL, (DXGI_FORMAT)0, 0);
        //    deviceContext->IASetInputLayout(NULL);
        //    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        //
        //    deviceContext->RSSetState(rasterizerState);
        //
        //    // vs
        //    deviceContext->VSSetShader(lutShader->getVertexShader(), nullptr, 0);
        //
        //    // ps
        //    ID3D11ShaderResourceView* textureSRV1 = lut->getShaderResouceView();
        //    ID3D11SamplerState* samplerState1 = lut->getSamplerState();
        //
        //    deviceContext->PSSetShader(lutShader->getPixelShader(), nullptr, 0);
        //    deviceContext->PSSetSamplers(0, 1, &samplerState1);
        //    deviceContext->PSSetShaderResources(0, 1, &textureSRV1);
        //
        //    // OM
        //    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
        //    deviceContext->OMSetDepthStencilState(depthStencilState, 1);
        //
        //    deviceContext->Draw(3, 0);
        //}

        // Earth Ground
        //{        
        //    // IA
        //    deviceContext->IASetInputLayout(inputLayout);
        //
        //    // VS
        //    deviceContext->VSSetShader(earthShader->getVertexShader(), nullptr, 0);
        //
        //    earhCBCPU.mvp = camera->getProjectionMatrix() * camera->getWorldToCamera() * earthMesh->getObjectToWorld();
        //    deviceContext->UpdateSubresource(earthCBGPU, 0, nullptr, &earhCBCPU, 0, 0);
        //    deviceContext->VSSetConstantBuffers(0, 1, &earthCBGPU);
        //
        //    // PS
        //    ID3D11SamplerState* sampler = earthGround->getSamplerState();
        //    ID3D11ShaderResourceView* srv = earthGround->getShaderResouceView();
        //
        //    deviceContext->PSSetShader(earthShader->getPixelShader(), nullptr, 0);
        //    deviceContext->PSSetSamplers(0, 1, &sampler);
        //    deviceContext->PSSetShaderResources(0, 1, &srv);
        //
        //    deviceContext->RSSetState(rasterizerState);
        //
        //    // OM
        //    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
        //    deviceContext->OMSetDepthStencilState(depthStencilState, 1);
        //    // restore
        //    deviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
        //
        //    earthMesh->draw(deviceContext);
        //}
        
        // Camera in Atmos or space
        if (camera->getOrigin().length() > atmosRadius)
        {
            // IA
            deviceContext->IASetInputLayout(inputLayout);
        
            // VS
            deviceContext->VSSetShader(skyFromSpaceShader->getVertexShader(), nullptr, 0);
        
            skyFromSpaceVSSCBCPU.mvp = camera->getProjectionMatrix() * camera->getWorldToCamera() * earthMesh->getObjectToWorld();
            skyFromSpaceVSSCBCPU.cameraPosition = camera->getOrigin();
            skyFromSpaceVSSCBCPU.cameraHeight2 = camera->getOrigin().lengthSquared();
        
            deviceContext->UpdateSubresource(skyFromSpaceVSCBGPU, 0, nullptr, &skyFromSpaceVSSCBCPU, 0, 0);
            deviceContext->VSSetConstantBuffers(0, 1, &skyFromSpaceVSCBGPU);
            // lut used in vs
            ID3D11ShaderResourceView* srv = lut->getShaderResouceView();
            ID3D11SamplerState* state = lut->getSamplerState();
            deviceContext->VSSetSamplers(0, 1, &state);
            deviceContext->VSSetShaderResources(0, 1, &srv);
        
            // PS
            deviceContext->PSSetShader(skyFromSpaceShader->getPixelShader(), nullptr, 0);
            deviceContext->UpdateSubresource(skyFromSpacePSCBGPU, 0, nullptr, &skyFromSpacePSSCBCPU, 0, 0);
            deviceContext->PSSetConstantBuffers(0, 1, &skyFromSpacePSCBGPU);

            ID3D11SamplerState* sampler = earthGround->getSamplerState();
            ID3D11ShaderResourceView* srv1 = earthGround->getShaderResouceView();
            deviceContext->PSSetSamplers(0, 1, &sampler);
            deviceContext->PSSetShaderResources(0, 1, &srv1);
        
            deviceContext->RSSetState(rasterizerState);
        
            // OM
            deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
            deviceContext->OMSetDepthStencilState(depthStencilState, 1);
            //deviceContext->OMSetBlendState(blendState, nullptr, 0xffffffff);
        
            atmosMesh->draw(deviceContext);
        }
        else
        {
            s3Log::warning("Not Implemented\n");
        }
        
        gui();
    }
};

int main()
{
    s3App app;
    if (!app.init(t3Vector2f(1600, 900), t3Vector2f(10, 10)))
        return 0;

    s3Window* window = app.getWindow();
    width = window->getWindowSize().x;
    height = window->getWindowSize().y;

    s3Renderer& renderer = s3Renderer::get();
    device = renderer.getDevice();
    deviceContext = renderer.getDeviceContext();
    renderTargetView = renderer.getRenderTargetView();
    depthStencilView = renderer.getDepthStencilView();

    camera = new s3Camera(t3Vector3f(0, 0, -25), t3Vector3f(0, 0, 1), t3Vector3f(0, 1, 0),
        width / height, 65, 0.01f, 1000.0f);
    //camera->setMouseEventState(false);

    createLUT();
    createShaders();
    createStates();
    createConstantBuffers();

    earthMesh = s3Mesh::createSphere(renderer.getDeviceContext(), earthRadius, 32);
    earthMesh->setObjectToWorld(t3Matrix4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1));

    atmosMesh = s3Mesh::createSphere(renderer.getDeviceContext(), atmosRadius, 256);
    atmosMesh->setObjectToWorld(t3Matrix4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1));

    earthGround = new s3ImageDecoder();
    earthGround->load(device, "../resources/EarthNoCloud2k.png");

    s3Sky mc;
    s3CallbackManager::callBack.onBeginRender += mc;

    app.run();

    destroy();
    return 0;
}