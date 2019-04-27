#pragma once
#include <core/s3Settings.h>
#include <directx/texture/s3TextureMode.h>

class s3Texture
{
public:
	s3Texture();
	s3Texture(const s3Texture& copy);
	virtual ~s3Texture();

	int getWidth() const;
	int getHeight() const;
	 
	std::string getName() const;
	void setName(std::string name);

	s3TextureWrapMode getWrapMode() const;
	void setWrapMode(const s3TextureWrapMode& wrapMode);

	s3TextureFilterMode getFilterMode() const;
	void setFilterMode(const s3TextureFilterMode& filterMode);

	s3TextureDimension getDimension() const;
	void setDimension(const s3TextureDimension& dimension);

protected:
	int width;
	int height;

	s3TextureWrapMode wrapMode;
	s3TextureFilterMode filterMode;
	s3TextureDimension dimension;

	std::string name;
};
