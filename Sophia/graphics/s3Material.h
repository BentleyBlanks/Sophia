#pragma once
#include <core/s3Settings.h>
#include <t3Vector3.h>
#include <t3Vector4.h>
#include <t3Matrix4x4.h>

class s3Texture;
class s3Shader;
class s3Material
{
public:
	s3Material(std::string filePath);
	~s3Material();

	float32 GetFloat(std::string name) const;
	int32 GetInt(std::string name) const;
	t3Matrix4x4 GetMatrix(std::string name) const;
	t3Vector4f GetVector(std::string name) const;
	s3Texture* GetTexture(std::string name) const;
	s3Shader* GetShader() const;

	bool IsKeywordEnabled(std::string name) const;

	bool SetKeyword(std::string name, bool enableKeyword);
	bool SetTexture(std::string name, s3Texture* texture);
	bool SetInt(std::string name, int32 value);
	bool SetFloat(std::string name, float32 value);
	bool SetMatrix(std::string name, t3Matrix4x4 value);
	bool SetVector(std::string name, t3Vector3f value);
};