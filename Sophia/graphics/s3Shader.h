#pragma once
#include <core/s3Settings.h>
#include <t3Vector3.h>
#include <t3Vector4.h>
#include <t3Matrix4x4.h>

// Initilalization would be done by Material
class s3Shader
{
public:
	s3Shader();
	~s3Shader();

	bool load(std::string fileName);

	float32 GetFloat(std::string name) const;
	int32 GetInt(std::string name) const;
	t3Matrix4x4 GetMatrix(std::string name) const;
	t3Vector4f GetVector(std::string name) const;
	s3Shader* GetShader() const;

	bool IsKeywordEnabled(std::string name) const;
	bool IsLoaded() const;

	bool SetKeyword(std::string name, bool enableKeyword);
	bool SetInt(std::string name, int32 value);
	bool SetFloat(std::string name, float32 value);
	bool SetMatrix(std::string name, t3Matrix4x4 value);
	bool SetVector(std::string name, t3Vector3f value);

	void print() const;

private:
	bool isLoaded;

	std::string shaderName;
};