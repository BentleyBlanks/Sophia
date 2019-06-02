#include <graphics/s3Material.h>

s3Material::s3Material(std::string fileName) :
	activatedPassIndex(0)
{
	shader = new s3Shader();
	shader->load(fileName);
}

s3Material::~s3Material()
{
	S3_SAFE_DELETE(shader);
}

bool s3Material::GetBool(std::string name) const
{
	return shader->GetBool(name);
}

float32 s3Material::GetFloat(std::string name) const
{
	return shader->GetFloat(name);
}

t3Vector2f s3Material::GetFloat2(std::string name) const
{
	return shader->GetFloat2(name);
}

t3Vector3f s3Material::GetFloat3(std::string name) const
{
	return shader->GetFloat3(name);
}

t3Vector4f s3Material::GetFloat4(std::string name) const
{
	return shader->GetFloat4(name);
}

int32 s3Material::GetInt(std::string name) const
{
	return shader->GetInt(name);
}

t3Vector2i s3Material::GetInt2(std::string name) const
{
	return shader->GetInt2(name);
}

t3Vector3i s3Material::GetInt3(std::string name) const
{
	return shader->GetInt3(name);
}

t3Vector4i s3Material::GetInt4(std::string name) const
{
	return shader->GetInt4(name);
}

t3Matrix4x4 s3Material::GetMatrix(std::string name) const
{
	return shader->GetMatrix(name);
}

s3Texture* s3Material::GetTexture(std::string name) const
{
	auto iter = textureList.find(name);
	if (iter == textureList.end())
		return nullptr;
	else
		return iter->second;
}

bool s3Material::GetKeyword(std::string name) const
{
	return false;
}

bool s3Material::SetBool(std::string name, bool value)
{
	return shader->SetBool(name, value);
}

bool s3Material::SetFloat(std::string name, float32 value)
{
	return shader->SetFloat(name, value);
}

bool s3Material::SetFloat2(std::string name, t3Vector2f value)
{
	return shader->SetFloat2(name, value);
}

bool s3Material::SetFloat3(std::string name, t3Vector3f value)
{
	return shader->SetFloat3(name, value);
}

bool s3Material::SetFloat4(std::string name, t3Vector4f value)
{
	return shader->SetFloat4(name, value);
}

bool s3Material::SetInt(std::string name, int32 value)
{
	return shader->SetInt(name, value);
}

bool s3Material::SetInt2(std::string name, t3Vector2i value)
{
	return shader->SetInt2(name, value);
}

bool s3Material::SetInt3(std::string name, t3Vector3i value)
{
	return shader->SetInt3(name, value);
}

bool s3Material::SetInt4(std::string name, t3Vector4i value)
{
	return shader->SetInt4(name, value);
}

bool s3Material::SetMatrix(std::string name, t3Matrix4x4 value)
{
	return shader->SetMatrix(name, value);
}

bool s3Material::SetTexture(std::string name, s3Texture* texture)
{
	if (!texture) return false;

	auto iter = textureList.find(name);
	if (iter == textureList.end()) textureList[name] = texture;

	return true;
}

bool s3Material::SetKeyword(std::string name, bool enableKeyword)
{
	return false;
}

std::vector<void*>* s3Material::GetConstantBufferData() const
{
	return shader->GetConstantBufferDataList(activatedPassIndex);
}

std::vector<ID3D11Buffer*>* s3Material::GetConstantBuffer() const
{
	return shader->GetConstantBufferList(activatedPassIndex);
}

int32 s3Material::GetConstantBufferNum() const
{
	return shader->GetPassConstantBufferNum(activatedPassIndex);
}

ID3D11PixelShader* s3Material::GetPixelShader() const
{
	return shader->GetPixelShader(activatedPassIndex);
}

ID3D11VertexShader* s3Material::GetVertexShader() const
{
	return shader->GetVertexShader(activatedPassIndex);
}

int32 s3Material::GetPassNum() const
{
	return shader->GetPassNum();
}

s3Shader* s3Material::GetShader() const
{
	return shader;
}

bool s3Material::SetPass(int32 pass)
{
	if (pass >= 0 && pass < shader->GetPassNum())
	{
		activatedPassIndex = pass;
		return true;
	}
	else
	{
		return false;
	}
}

int32 s3Material::GetPass(int32 pass) const
{
	return activatedPassIndex;
}

