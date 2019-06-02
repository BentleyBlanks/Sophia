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
	bool IsLoaded() const;

	float32 GetFloat(std::string name) const;
	t3Vector2f GetFloat2(std::string name) const;	
	t3Vector3f GetFloat3(std::string name) const;
	t3Vector4f GetFloat4(std::string name) const;
	int32 GetInt(std::string name) const;
	t3Vector2i GetInt2(std::string name) const;
	t3Vector3i GetInt3(std::string name) const;
	t3Vector4i GetInt4(std::string name) const;
	t3Matrix4x4 GetMatrix(std::string name) const;
	bool GetKeyword(std::string name) const;
	bool GetBool(std::string name) const;

	bool SetKeyword(std::string name, bool enableKeyword);
	bool SetBool(std::string name, bool value);
	bool SetInt(std::string name, int32 value);
	bool SetInt2(std::string name, t3Vector2i value);
	bool SetInt3(std::string name, t3Vector3i value);
	bool SetInt4(std::string name, t3Vector4i value);
	bool SetFloat(std::string name, float32 value);
	bool SetFloat2(std::string name, t3Vector2f value);
	bool SetFloat3(std::string name, t3Vector3f value);
	bool SetFloat4(std::string name, t3Vector4f value);
	bool SetMatrix(std::string name, t3Matrix4x4 value);

	// activate pas index related
	std::vector<void*>* GetConstantBufferDataList(int32 pass) const;
	std::vector<ID3D11Buffer*>* GetConstantBufferList(int32 pass) const;
	int32 GetPassConstantBufferNum(int32 pass) const;

	ID3D11PixelShader* GetPixelShader(int32 pass) const;
	ID3D11VertexShader* GetVertexShader(int32 pass) const;
	int32 GetPassNum() const;

	void print() const;

private:
	bool GetValue(std::string name, int32 type, s3ShaderValue& value) const;
	bool SetValue(std::string name, s3ShaderValue& value);

	bool isLoaded;
	class s3ShaderParser* shaderParser;
};