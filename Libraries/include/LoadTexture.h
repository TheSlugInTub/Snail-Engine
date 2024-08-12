#ifndef LOAD_TEXTURE_H
#define LOAD_TEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>

/*
This is a horrible way of going about things but, this is here so that including
SnailFunctions for the loadTexture function doesn't mess up your includes and cause
recursive includes.
*/
class TextureLoader
{
public:
    static unsigned int textureLoad(char const* path);
};

#endif