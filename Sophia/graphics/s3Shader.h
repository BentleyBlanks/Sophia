#pragma once
#include <core/s3Settings.h>
#include <t3Vector2.h>
#include <t3Vector3.h>
#include <t3Vector4.h>
#include <t3Matrix4x4.h>

// Initilalization would be done by Material
class s3ShaderValue;
class s3Shader
{
public:
	s3Shader();
	~s3Shader();

	bool load(std::string fileName);
	bool isLoaded() const;

	float32 getFloat(std::string name) const;
	t3Vector2f getFloat2(std::string name) const;	
	t3Vector3f getFloat3(std::string name) const;
	t3Vector4f getFloat4(std::string name) const;
	int32 getInt(std::string name) const;
	t3Vector2i getInt2(std::string name) const;
	t3Vector3i getInt3(std::string name) const;
	t3Vector4i getInt4(std::string name) const;
	t3Matrix4x4 getMatrix(std::string name) const;
	bool getKeyword(std::string name) const;
	bool getBool(std::string name) const;

	bool setKeyword(std::string name, bool enableKeyword);
	bool setBool(std::string name, bool value);
	bool setInt(std::string name, int32 value);
	bool setInt2(std::string name, t3Vector2i value);
	bool setInt3(std::string name, t3Vector3i value);
	bool setInt4(std::string name, t3Vector4i value);
	bool setFloat(std::string name, float32 value);
	bool setFloat2(std::string name, t3Vector2f value);
	bool setFloat3(std::string name, t3Vector3f value);
	bool setFloat4(std::string name, t3Vector4f value);
	bool setMatrix(std::string name, t3Matrix4x4 value);

	// activate pas index related
	std::vector<void*>* getConstantBufferDataList(int32 pass) const;
	std::vector<ID3D11Buffer*>* getConstantBufferList(int32 pass) const;
	int32 getPassConstantBufferNum(int32 pass) const;

	ID3D11PixelShader* getPixelShader(int32 pass) const;
	ID3D11VertexShader* getVertexShader(int32 pass) const;
	int32 getPassNum() const;

	// for debug use
	void print() const;
	static void reloadShaderSystem();

private:
	bool getValue(std::string name, int32 type, s3ShaderValue& value) const;
	bool setValue(std::string name, s3ShaderValue& value);

	bool bIsLoaded;
	class s3ShaderParser* shaderParser;
};