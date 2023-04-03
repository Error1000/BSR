# Software Rasterizer (Florescu Bogdan - 143)



This project is a software rasterizer able to render 3D graphics in *real time (hopefully)* and save the rendered results to disk.



## Requirements

The only supported platform is Windows (7 all the way up to 11 on both 32 and 64 bit architectures).
It is necessary to be compiled with MSVC. The C++ version used is C++20.



## Getting started

It works out of the box since I only use relative paths and the project does not have external dependencies.



## Features

Currently the features that are available are the math lib and the asset manager and loader that can load 3D models and textures.



## Classes and structs

Here I will list all of the classes and structures found in the project and a short description of them. This software was designed in a modular way so I will also provide the module name before the class name.

* class Vector

This class is a container that has exactly the same functionality as the std::vector from STL. It has support for move semantics and it is templated.

* struct Math::Vec2f

A simple 2 component point struct. It has operator overloading and helpers for convenience.

* struct Math::Vec3f

A simple 3 component point struct. It has operator overloading and helpers for convenience.

* struct Math::Vec4f

A simple 4 component point struct. It has operator overloading and helpers for convenience.

* class Math::Mat2f

This is a class for 2x2 matrices with helper functions to retrieve transformation matrices. A lot of operators were overloaded for convenience.

* class Math::Mat3f

This is a class for 3x3 matrices with helper functions to retrieve transformation matrices. A lot of operators were overloaded for convenience.

* class Math::Mat4f

This is a class for 4x4 matrices with helper functions to retrieve transformation matrices. A lot of operators were overloaded for convenience.

* struct Asset

Used to store an address to an asset and it's name.

* class AssetManager

Used to handle assets in order to avoid copying data all around the place.

* struct Image::ImageSRGBA

Data about an image on the sRGB color space with alpha channel.

* struct Image::ImageHDR

Stores a high dynamic range image.

* struct Rasterizer::Camera

Used to store and manage the position and orientation data from which to render the scene.

* struct Rasterizer::VertexData

The properties of a point on the 3D model.

* class Rasterizer::VertexBuffer

A pool of verteces.

* struct Rasterizer::IndexData

A set of 3 indexs into the vertex buffer. Such a triplet forms a triangle.

* class Rasterizer::IndexBuffer

A pool of indexed triangles.

* struct Rasterizer::Mesh

Quick and dirty hack to pair up a VBO and an IBO.

* class Rasterizer::Model

A set of meshes that describe a 3D model.