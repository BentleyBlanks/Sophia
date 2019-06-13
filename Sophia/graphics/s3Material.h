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

	bool isLoaded() const;

	bool getBool(std::string name) const;
	float32 getFloat(std::string name) const;
	t3Vector2f getFloat2(std::string name) const;
	t3Vector3f getFloat3(std::string name) const;
	t3Vector4f getFloat4(std::string name) const;
	int32 getInt(std::string name) const;
	t3Vector2i getInt2(std::string name) const;
	t3Vector3i getInt3(std::string name) const;
	t3Vector4i getInt4(std::string name) const;
	t3Matrix4x4 getMatrix(std::string name) const;
	s3Texture* getTexture(std::string name) const;
	bool getKeyword(std::string name) const;

	bool setBool(std::string name, bool value);
	bool setFloat(std::string name, float32 value);
	bool setFloat2(std::string name, t3Vector2f value);
	bool setFloat3(std::string name, t3Vector3f value);
	bool setFloat4(std::string name, t3Vector4f value);
	bool setInt(std::string name, int32 value);
	bool setInt2(std::string name, t3Vector2i value);
	bool setInt3(std::string name, t3Vector3i value);
	bool setInt4(std::string name, t3Vector4i value);
	bool setMatrix(std::string name, t3Matrix4x4 value);
	bool setTexture(std::string name, s3Texture* texture);
	bool setKeyword(std::string name, bool enableKeyword);

	std::vector<void*>* getConstantBufferData() const;
	std::vector<ID3D11Buffer*>* getConstantBuffer() const;
	int32 getConstantBufferNum() const;

	ID3D11PixelShader* getPixelShader() const;
	ID3D11VertexShader* getVertexShader() const;
	int32 getPassNum() const;
	s3Shader* getShader() const;

	// Get/Set activated pass for rendering.
	bool setPass(int32 pass);
	int32 getPass(int32 pass) const;

	// for debug use
	void print() const;

private:
	std::map<std::string, s3Texture*> textureList;
	s3Shader* shader;

	int32 activatedPassIndex;
};