## sophia
Sophia is a real-time DirectX 11 renderer. It is not quite a rich graphics engine, only packages some low-level DirectX functions and contains some useful tools. But it is enough to implement some Real-Time algorithms more efficiently.

## Demo

### Atmosperic

Real-time atmosperic simulation based on [Accurate Atmospheric Scattering](https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter16.html) and [Simulating the Colors of the Sky](http://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/simulating-sky)

> Outside the earth

<img src="https://farm2.staticflickr.com/1815/42921965492_5bb5243225_b.jpg" width="1024" height="288" alt="outside1">

> Inside the earth

<img src="https://farm2.staticflickr.com/1804/42252751394_2a58178cb6_b.jpg" width="1024" height="288" alt="inside">

### PBR Based on IBL 

Precomputed LUT according to [Real Shading in Unreal Engine 4](https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf)

> Specular map used as mipmap up to 8 levels

<img src="https://farm2.staticflickr.com/1791/42069990655_b09f7336fc_b.jpg" width="1024" height="512" alt="specularMap">

> Split the sum of the BRDF LUT and diffuse irradiance map

<img src="https://farm1.staticflickr.com/900/29099145538_8ec61a2b6d_b.jpg" width="1024" height="341" alt="precomputed">

> Spheres with IBL

<img src="https://farm2.staticflickr.com/1768/42253004964_05d7a943ca_b.jpg" width="1024" height="288" alt="IBL">

## How to use
Only supported **Visuio Studio 2015**(Please tell me if it compatible with others). Contains **all thirdparty** no need other dependencies. 

## Thirdparty
1. [ImGui](https://github.com/ocornut/imgui)
2. [lodepng](https://github.com/lvandeve/lodepng)
3. [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
4. [tinyexr](https://github.com/syoyo/tinyexr)
5. [t3Math](https://github.com/BentleyBlanks/t3Math)
6. [t3DataStructures](https://github.com/BentleyBlanks/t3DataStructures)

## Author

``` cpp
const char* 官某某 = "Bingo";

std::string 个人博客 = "http://bentleyblanks.github.io";
```
