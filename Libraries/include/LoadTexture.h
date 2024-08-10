#ifndef LOAD_TEXTURE_H
#define LOAD_TEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include <stb_image.h>

class TextureLoader
{
public:

    static unsigned int textureLoad(char const* path);
};

#endif