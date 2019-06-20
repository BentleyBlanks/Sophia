#include <graphics/s3Material.h>
#include <texture/s3Texture.h>
#include <core/log/s3Log.h>

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

bool s3Material::isLoaded() const
{
	return shader->isLoaded();
}

bool s3Material::getBool(std::string name) const
{
	return shader->getBool(name);
}

float32 s3Material::getFloat(std::string name) const
{
	return shader->getFloat(name);
}

t3Vector2f s3Material::getFloat2(std::string name) const
{
	return shader->getFloat2(name);
}

t3Vector3f s3Material::getFloat3(std::string name) const
{
	return shader->getFloat3(name);
}

t3Vector4f s3Material::getFloat4(std::string name) const
{
	return shader->getFloat4(name);
}

int32 s3Material::getInt(std::string name) const
{
	return shader->getInt(name);
}

t3Vector2i s3Material::getInt2(std::string name) const
{
	return shader->getInt2(name);
}

t3Vector3i s3Material::getInt3(std::string name) const
{
	return shader->getInt3(name);
}

t3Vector4i s3Material::getInt4(std::string name) const
{
	return shader->getInt4(name);
}

t3Matrix4x4 s3Material::getMatrix(std::string name) const
{
	return shader->getMatrix(name);
}

s3Texture* s3Material::getTexture(std::string name) const
{
	auto iter = textureList.find(name);
	if (iter == textureList.end())
		return nullptr;
	else
		return iter->second;
}

bool s3Material::getKeyword(std::string name) const
{
	return false;
}

bool s3Material::setBool(std::string name, bool value)
{
	return shader->setBool(name, value);
}

bool s3Material::setFloat(std::string name, float32 value)
{
	return shader->setFloat(name, value);
}

bool s3Material::setFloat2(std::string name, t3Vector2f value)
{
	return shader->setFloat2(name, value);
}

bool s3Material::setFloat3(std::string name, t3Vector3f value)
{
	return shader->setFloat3(name, value);
}

bool s3Material::setFloat4(std::string name, t3Vector4f value)
{
	return shader->setFloat4(name, value);
}

bool s3Material::setInt(std::string name, int32 value)
{
	return shader->setInt(name, value);
}

bool s3Material::setInt2(std::string name, t3Vector2i value)
{
	return shader->setInt2(name, value);
}

bool s3Material::setInt3(std::string name, t3Vector3i value)
{
	return shader->setInt3(name, value);
}

bool s3Material::setInt4(std::string name, t3Vector4i value)
{
	return shader->setInt4(name, value);
}

bool s3Material::setMatrix(std::string name, t3Matrix4x4 value)
{
	return shader->setMatrix(name, value);
}

bool s3Material::setTexture(std::string name, s3Texture* texture)
{
	if (!texture) return false;

	auto iter = textureList.find(name);
	if (iter == textureList.end()) textureList[name] = texture;

	return true;
}

bool s3Material::setKeyword(std::string name, bool enableKeyword)
{
	return false;
}

std::vector<void*>* s3Material::getConstantBufferData() const
{
	return shader->getConstantBufferDataList(activatedPassIndex);
}

std::vector<int32>* s3Material::getConstantBufferDataSize() const
{
	return shader->getConstantBufferDataSizeList(activatedPassIndex);
}

std::vector<ID3D11Buffer*>* s3Material::getConstantBuffer() const
{
	return shader->getConstantBufferList(activatedPassIndex);
}

int32 s3Material::getConstantBufferNum() const
{
	return shader->getPassConstantBufferNum(activatedPassIndex);
}

ID3D11PixelShader* s3Material::getPixelShader() const
{
	return shader->getPixelShader(activatedPassIndex);
}

ID3D11VertexShader* s3Material::getVertexShader() const
{
	return shader->getVertexShader(activatedPassIndex);
}

int32 s3Material::getPassNum() const
{
	return shader->getPassNum();
}

s3Shader* s3Material::getShader() const
{
	return shader;
}

bool s3Material::setPass(int32 pass)
{
	if (pass >= 0 && pass < shader->getPassNum())
	{
		activatedPassIndex = pass;
		return true;
	}
	else
	{
		return false;
	}
}

int32 s3Material::getPass(int32 pass) const
{
	return activatedPassIndex;
}

void s3Material::print() const
{
	if (!isLoaded()) return;

	s3Log::debug("=============================== s3Material::print() begin ===============================\n");
	s3Log::debug("activatedPassIndex: %d\n", activatedPassIndex);
	auto& textureIter = textureList.begin();
	while (textureIter != textureList.end())
	{
		s3Log::debug("shader.name: %s\n", textureIter->second->name.c_str());
		textureIter++;
	}

	shader->print();

	s3Log::debug("=============================== s3Material::print() end ===============================\n");
}

