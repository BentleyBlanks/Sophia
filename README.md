## Sophia
Sophia is a real-time DirectX 11 renderer. It is not quite a rich graphics engine, only packages some low-level DirectX functions and contains some useful tools. But it is enough to implement some Real-Time algorithms more efficiently.

## Demo

### Volumetric Cloudscapes

>   Placeholder



## How to use
Supported **Visual Studio 2015, 2017, 2019**(Please contact me if it compatible with others). Contains **all thirdparty** no need other dependencies. 



## Deprecated

>   Demo before renderer was rewriting, so please check older version [Sophia](https://github.com/BentleyBlanks/Sophia/tree/064eff64b0e18f45cf01cf06e9c98386640871ac) for more details.

#### Atmosperic

Real-time atmosperic simulation based on [Accurate Atmospheric Scattering](https://developer.nvidia.com/gpugems/GPUGems2/gpugems2_chapter16.html) and [Simulating the Colors of the Sky](http://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/simulating-sky). 

>   See more explain in my blog [Atmosperical simulation](http://bentleyblanks.github.io/2018/06/26/2018-06-26-Atmosperical%20simulation/)

>   Outside the earth

<img src="https://farm2.staticflickr.com/1815/42921965492_5bb5243225_b.jpg" width="1024" height="288" alt="outside1">

>   Inside the earth

<img src="https://farm2.staticflickr.com/1804/42252751394_2a58178cb6_b.jpg" width="1024" height="288" alt="inside">

#### PBR Based on IBL

Precomputed LUT according to [Real Shading in Unreal Engine 4](https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf)

>   See more explain in my blog [PBR based on IBl](http://bentleyblanks.github.io/2018/06/24/2018-06-24-PBR%20based%20on%20IBl/)

>   Specular map used as mipmap up to 8 levels

<img src="https://farm2.staticflickr.com/1791/42069990655_b09f7336fc_b.jpg" width="1024" height="512" alt="specularMap">

>   Split the sum of the BRDF LUT and diffuse irradiance map

<img src="https://farm2.staticflickr.com/1801/42922399172_9a5ed87ddb_b.jpg" width="1024" height="342" alt="precomputed">

>   Spheres with IBL

<img src="https://farm2.staticflickr.com/1768/42253004964_05d7a943ca_b.jpg" width="1024" height="288" alt="IBL">



## Thirdparty
1. [imGui](https://github.com/ocornut/imgui)
2. [lodepng](https://github.com/lvandeve/lodepng)
3. [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
4. [tinyexr](https://github.com/syoyo/tinyexr)
5. [t3Math](https://github.com/BentleyBlanks/t3Math)
6. [t3DataStructures](https://github.com/BentleyBlanks/t3DataStructures)

>   The project file of the thirdparty only used for previewing the source file(t3Math and t3DataStrutures as submodule of Sophia)



## Author

``` cpp
const char* 官某某 = "Bingo";

std::string 个人博客 = "http://bentleyblanks.github.io";
```
