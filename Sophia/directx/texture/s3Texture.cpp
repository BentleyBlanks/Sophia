#include <directx/texture/s3Texture.h>

s3Texture::s3Texture()
{
}

s3Texture::s3Texture(const s3Texture & copy)
{}

s3Texture::~s3Texture()
{}

int s3Texture::getWidth() const
{
	return width;
}

int s3Texture::getHeight() const
{
	return height;
}

std::string s3Texture::getName() const
{
	return name;
}

void s3Texture::setName(std::string name)
{
	this->name = name;
}

s3TextureWrapMode s3Texture::getWrapMode() const
{
	return wrapMode;
}

void s3Texture::setWrapMode(const s3TextureWrapMode& wrapMode)
{
	this->wrapMode = wrapMode;
}

s3TextureFilterMode s3Texture::getFilterMode() const
{
	return filterMode;
}

void s3Texture::setFilterMode(const s3TextureFilterMode& filterMode)
{
	this->filterMode = filterMode;
}

s3TextureDimension s3Texture::getDimension() const
{
	return dimension;
}

void s3Texture::setDimension(const s3TextureDimension& dimension)
{
	this->dimension = dimension;
}
