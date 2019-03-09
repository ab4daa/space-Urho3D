# space-Urho3D
Urho3D implementation of https://github.com/wwwtyro/space-3d

![screen](screen.png?raw=true "Optional Title")

The green arrow is the direction of sun, the directional light in the scene will also change its direction and color depends on generated sun.

Note: It only works in OpenGL and D3D11, I don't know how to do the blend mode in D3D9.

## Build sample
Cmake as ordinary Urho3D project

## Usage
Necessary files for SpaceBoxGen class:

    SpaceBoxGen.cpp

    SpaceBoxGen.h

    bin/CoreData/RenderPaths/SpaceBox.xml

    bin/CoreData/Shaders/GLSL/point_stars.glsl

    bin/CoreData/Shaders/GLSL/star.glsl

    bin/CoreData/Shaders/GLSL/nebula.glsl

    bin/CoreData/Shaders/GLSL/classicnoise4D.glsl

    bin/CoreData/Shaders/GLSL/sun.glsl

    bin/CoreData/Shaders/HLSL/point_stars.hlsl

    bin/CoreData/Shaders/HLSL/star.hlsl

    bin/CoreData/Shaders/HLSL/nebula.hlsl

    bin/CoreData/Shaders/HLSL/classicnoise4D.hlsl

    bin/CoreData/Shaders/HLSL/sun.hlsl

    bin/CoreData/Techniques/NoTextureAlphaPointStar.xml

    bin/CoreData/Techniques/NoTextureAlphaStar.xml

    bin/CoreData/Techniques/NoTextureAlphaNebular.xml

    bin/CoreData/Techniques/NoTextureAlphaSun.xml

    bin/Data/Materials/point_stars.xml

    bin/Data/Materials/star.xml

    bin/Data/Materials/nebular.xml

    bin/Data/Materials/sun.xml

See RenderToTexture.cpp to use it.
