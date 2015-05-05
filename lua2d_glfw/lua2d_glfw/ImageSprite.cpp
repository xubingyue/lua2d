//
//  ImageSprite.cpp
//  Lua2d
//
//  Created by chenbingfeng on 15/4/27.
//  Copyright (c) 2015年 chenbingfeng. All rights reserved.
//

#include "ImageSprite.h"
#include <OpenGl/gl3.h>
#include "utils.h"
#include <iostream>

NS_L2D_BEGIN

SPImageSprite ImageSprite::create(const std::string &fileName, int width, int height)
{
    if (!_initedStatic) initStatic();
    return SPImageSprite(new ImageSprite(fileName, width, height));
}

ImageSprite::ImageSprite(const std::string& fileName, int width, int height):
_fileName(fileName), _width(width),_height(height)
{
    //empty
}

void ImageSprite::load()
{
    if (_loaded) LOG("ImageSprite", _fileName, "already loaded");
    assert(!_loaded);

    auto image = cppgl::Image::create(_fileName);
    image->load();
    _texture = cppgl::Texture::create(image);
    _texture->setWrapping(cppgl::Wrapping::Repeat, cppgl::Wrapping::Repeat);
    _texture->setFilters(cppgl::Filter::Linear, cppgl::Filter::Linear);
}

void ImageSprite::draw(SPRenderer& renderer)
{
    auto context = renderer->getContext();
    context->useProgram(_program);
    context->bindTexture(_texture, 0);
    context->drawArrays(*_vao, cppgl::Primitive::Triangles, 0, 6);
}

//static area
bool ImageSprite::_initedStatic = false;

void ImageSprite::initStatic()
{
    _initedStatic = true;
    const char* vertexSource =
    "#version 150\n"
    "\n"
    "in vec2 position;\n"
    "in vec2 texcoord;\n"
    "out vec2 Texcoord;\n"
    "\n"
    "\n"
    "void main()\n"
    "{\n"
    "    Texcoord = vec2(texcoord.x, -texcoord.y);\n"
    "    gl_Position = vec4(position.x, position.y, 0.0, 1.0);\n"
    "}\n";

    const char* fragmentSource =
    "#version 150\n"
    "\n"
    "in vec2 Texcoord;\n"
    "\n"
    "out vec4 outColor;\n"
    "uniform sampler2D tex;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    outColor = texture(tex, Texcoord);\n"
    "}\n";

    auto vert = cppgl::Shader::create(cppgl::ShaderType::Vertex, vertexSource);
    auto frag = cppgl::Shader::create(cppgl::ShaderType::Fragment, fragmentSource);
    _program = cppgl::Program::create(vert, frag);

    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f
    };
    _vbo = cppgl::VertexBuffer::create(vertices, sizeof(vertices), cppgl::BufferUsage::StaticDraw);
    _vao = cppgl::VertexArray::create();
    _vao->bindAttribute(_program->getAttribute("position"), *_vbo, cppgl::Type::Float, 2, 4*sizeof(float), NULL);
    _vao->bindAttribute(_program->getAttribute("texcoord"), *_vbo, cppgl::Type::Float, 2, 4*sizeof(float), 2*sizeof(float));
}

cppgl::SPVertexArray ImageSprite::_vao;
cppgl::SPVertexBuffer ImageSprite::_vbo;
cppgl::SPProgram ImageSprite::_program;

NS_L2D_END
