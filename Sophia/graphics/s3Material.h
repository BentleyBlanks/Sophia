#pragma once
#include <core/s3Settings.h>
#include <graphics/s3Shader.h>
#include <t3Vector2.h>
#include <t3Vector3.h>
#include <t3Vector4.h>
#include <t3Matrix4x4.h>
#include <map>

class s3Texture;
class s3Material
{
public:
	s3Material(std::string fileName);
	~s3Material();

	bool GetBool(std::string name) const;
	float32 GetFloat(std::string name) const;
	t3Vector2f GetFloat2(std::string name) const;
	t3Vector3f GetFloat3(std::string name) const;
	t3Vector4f GetFloat4(std::string name) const;
	int32 GetInt(std::string name) const;
	t3Vector2i GetInt2(std::string name) const;
	t3Vector3i GetInt3(std::string name) const;
	t3Vector4i GetInt4(std::string name) const;
	t3Matrix4x4 GetMatrix(std::string name) const;
	s3Texture* GetTexture(std::string name) const;
	bool GetKeyword(std::string name) const;

	bool SetBool(std::string name, bool value);
	bool SetFloat(std::string name, float32 value);
	bool SetFloat2(std::string name, t3Vector2f value);
	bool SetFloat3(std::string name, t3Vector3f value);
	bool SetFloat4(std::string name, t3Vector4f value);
	bool SetInt(std::string name, int32 value);
	bool SetInt2(std::string name, t3Vector2i value);
	bool SetInt3(std::string name, t3Vector3i value);
	bool SetInt4(std::string name, t3Vector4i value);
	bool SetMatrix(std::string name, t3Matrix4x4 value);
	bool SetTexture(std::string name, s3Texture* texture);
	bool SetKeyword(std::string name, bool enableKeyword);

	std::vector<void*>* GetConstantBufferData() const;
	std::vector<ID3D11Buffer*>* GetConstantBuffer() const;
	int32 GetConstantBufferNum() const;

	ID3D11PixelShader* GetPixelShader() const;
	ID3D11VertexShader* GetVertexShader() const;
	int32 GetPassNum() const;
	s3Shader* GetShader() const;

	// Get/Set activated pass for rendering.
	bool SetPass(int32 pass);
	int32 GetPass(int32 pass) const;

private:
	std::map<std::string, s3Texture*> textureList;
	s3Shader* shader;

	int32 activatedPassIndex;
};