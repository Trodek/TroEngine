# TroEngine
[![License](http://img.shields.io/:license-mit-blue.svg)](http://doge.mit-license.org)
[![Build status](https://ci.appveyor.com/api/projects/status/m3lluga340u8kfuk?svg=true)](https://ci.appveyor.com/project/Trodek/troengine)
[![Build status](https://ci.appveyor.com/api/projects/status/m3lluga340u8kfuk/branch/master?svg=true)](https://ci.appveyor.com/project/Trodek/troengine/branch/master)

Basic 3D Engine based on OpenGL 2.1

## Current State:
Currently the engine is a geometry viewer. This means that the engine is capable
of loading FBX files and PNG texture images. If the FBX file contains the path of 
the texture it will be loaded automatically. That way, the engine may be able to 
load other texture formats.
The engine accepts one diffuse texture and as many meshes as the fbx files contain.

## Usage
To use the engine, drag and drop your files inside, the rest is handled for you.
The camera controls are:
* W, A, S, D: for front, left, back and right movement.
* Q, E: up and down movement.
* Mouse Wheel: zoom in and out
* Right Click: rotate camera with mouse movement.
* Alt + Left Click: Oribt arround center
* F: focus camera to center

You can find information about loaded fbx and texture on the opening the inspector
window from the view menu.

A performance window can be aopened at view -> performance.

Theres also a configuration widow at file menu.

Take in acount that camera will adjust has a maximum value to avoid moving outside 
vision range.

## GitHub
The engine is mantained by Trodek.

Souce code: https://github.com/Trodek/TroEngine

Releases: https://github.com/Trodek/TroEngine/releases

## License
MIT License

Copyright (c) 2017 Iban Mas Ortega

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 
