
#include "GPUTexture.h"
#include "ThirdParty/glad/glad.h"


GLenum sTexTarget[] =
{
    GL_TEXTURE_2D,
    GL_TEXTURE_3D
};

GLenum sTexFormat[] =
{
    GL_R8,
    GL_RGBA8,
    GL_RGBA16F,
    GL_RGBA32F,
};

GLenum sTexFilter[] =
{
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_LINEAR
};

GLenum sTexWrap[] = 
{
    GL_REPEAT,
    GL_CLAMP_TO_EDGE
};

CGPUTexture::CGPUTexture(TGPUTextureConfig const& aConfig)
    : mConfig(aConfig)
{
    mTarget = sTexTarget[mConfig.mTarget];
    glCreateTextures(mTarget, 1, &mTextureHandler);

    glTextureParameteri(mTextureHandler, GL_TEXTURE_MIN_FILTER, sTexFilter[mConfig.mMinFilter]);
    glTextureParameteri(mTextureHandler, GL_TEXTURE_MAG_FILTER, sTexFilter[mConfig.mMagFilter]);
    glTextureParameteri(mTextureHandler, GL_TEXTURE_WRAP_S, sTexWrap[mConfig.mWrapS]);
    glTextureParameteri(mTextureHandler, GL_TEXTURE_WRAP_T, sTexWrap[mConfig.mWrapT]);
    if (mTarget == GL_TEXTURE_3D)
    {
        glTextureParameteri(mTextureHandler, GL_TEXTURE_WRAP_R, sTexWrap[mConfig.mWrapR]);
        glTextureStorage3D(mTextureHandler, mConfig.mMips, sTexFormat[mConfig.mFormat], mConfig.mExtentX, mConfig.mExtentY, mConfig.mSlices);
    }
    else if (mTarget == GL_TEXTURE_2D)
    {
        glTextureStorage2D(mTextureHandler, mConfig.mMips, sTexFormat[mConfig.mFormat], mConfig.mExtentX, mConfig.mExtentY);
    }

}

CGPUTexture::~CGPUTexture()
{
    glDeleteTextures(1, &mTextureHandler);
}

void CGPUTexture::Bind(uint32_t aLocation)
{

}