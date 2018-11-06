#include "EnginePCH.h"
#include "PointLight.h"



PointLight::PointLight()
{
	m_nearPlane = 1.0f;
	m_farPlane = 50.0f;
	
}
PointLight::PointLight(float * translation, float * color, float intensity)
{
	m_nearPlane = 1.0f;
	m_farPlane = 20.0f;
	this->m_position = DirectX::XMFLOAT4A(translation[0], translation[1], translation[2], 1);
	this->m_color = DirectX::XMFLOAT4A(color[0], color[1], color[2], 1.0);
	this->m_dropOff = 1.0f;
	this->m_intensity = intensity;
	this->m_pow = 2.0f;
	//CreateShadowDirection(PointLight::Y_POSITIVE);
	CreateShadowDirection(PointLight::XYZ_ALL);
	for (int i = 0; i < 6; i++)
	{
		m_useSides[i] = TRUE;
	}
	this->m_dropOff = .5f;
	_initDirectX(128U,128U);
}

PointLight::~PointLight()
{
	for (int i = 0; i < m_sides.size(); i++)
	{
		delete m_sides[i];
	}
	DX::SafeRelease(m_shadowShaderResourceView);
	DX::SafeRelease(m_shadowDepthStencilView);
	DX::SafeRelease(m_shadowDepthBufferTex);
}

void PointLight::CreateShadowDirection(ShadowDir direction)
{
	switch (direction)
	{
	case PointLight::Y_POSITIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f)); // UP
		break;
	case PointLight::Y_NEGATIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, -1.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f)); // Down
		break;
	case PointLight::X_POSITIVE:
		_createSide(DirectX::XMFLOAT4A(1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Right
		break;
	case PointLight::X_NEGATIVE:
		_createSide(DirectX::XMFLOAT4A(-1.0f, 0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Left
		break;
	case PointLight::Z_POSITIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Forward
		break;
	case PointLight::Z_NEGATIVE:
		_createSide(DirectX::XMFLOAT4A(0.0f, 0.0f, -1.0f, 0.0f), DirectX::XMFLOAT4A(0.0f, 1.0f, 0.0f, 0.0f)); // Back
		break;
	case PointLight::XYZ_ALL:
		this->_createSides();
		break;
	}


}

void PointLight::Init(DirectX::XMFLOAT4A position, DirectX::XMFLOAT4A color, float power)
{
	this->m_position = position;
	this->m_color = color;
	this->m_dropOff = 1.0f;
	this->m_intensity = 1.0f;
	this->m_pow = 2.0f;
	//_createSides();
}

const DirectX::XMFLOAT4A & PointLight::getPosition() const
{
	return m_position;
}

const DirectX::XMFLOAT4A & PointLight::getColor() const
{
	return m_color;
}

const std::vector<Camera*>& PointLight::getSides() const
{
	return m_sides;
}

const float & PointLight::getDropOff() const
{
	return m_dropOff;
}

const float & PointLight::getPow() const
{
	return this->m_pow;
}

const float & PointLight::getIntensity() const
{
	return this->m_intensity;
}

const float & PointLight::getFarPlane() const
{
	return this->m_farPlane;
}

const float & PointLight::getFOV() const
{
	return this->FOV;
}



void PointLight::CreateShadowDirection(const std::vector<ShadowDir> & shadowDir)
{
	for (unsigned int i = 0; i < shadowDir.size(); i++)
	{
		CreateShadowDirection(shadowDir[i]);
	}
}

float PointLight::TourchEffect(double deltaTime, float base, float amplitude)
{
	static double time = 0.0f;
	static DirectX::XMFLOAT2 current(0.0, 0.0);
	static DirectX::XMFLOAT2 target(1.0, 1.0);
	static double timer = 0.0f;
	timer += deltaTime;
	static float ran = 5.5f;

	if (abs(current.x - target.x) < 0.1)
	{
		timer = 0.0;

		ran = (float)(rand() % 100) / 100.0f;

		target.x = ran;

	}

	auto v1 = DirectX::XMLoadFloat2(&current);
	auto v2 = DirectX::XMLoadFloat2(&target);
	DirectX::XMVECTOR vec;

	vec = DirectX::XMVectorLerp(v1, v2, (float)deltaTime * 5.0f);


	current.x = DirectX::XMVectorGetX(vec);

	float temp = base + sin(current.x) * amplitude;
	return temp;
}

ID3D11ShaderResourceView * PointLight::getSRV() const
{
	return m_shadowShaderResourceView;
}

ID3D11DepthStencilView * PointLight::getDSV() const
{
	return m_shadowDepthStencilView;
}

ID3D11Texture2D * PointLight::getTEX() const
{
	return m_shadowDepthBufferTex;
}

void PointLight::setUpdate(const bool & update)
{
	this->m_update = update;
}

bool PointLight::getUpdate() const
{
	if (!m_firstRun)
		return this->m_update;
	else
	{
		return true;
	}
}

void PointLight::FirstRun()
{
	m_firstRun = false;
}

void PointLight::Clear()
{
	if(m_update)
		DX::g_deviceContext->ClearDepthStencilView(m_shadowDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

const BOOL * PointLight::useSides() const
{
	return m_useSides;
}

void PointLight::RayTrace(b3Body & object, RayCastListener * rayCastListener)
{
	using namespace DirectX;

	XMFLOAT4A objPos = XMFLOAT4A(	object.GetTransform().translation.x,
													object.GetTransform().translation.y,
													object.GetTransform().translation.z, 1);

	XMVECTOR vObjPos = XMLoadFloat4A(&objPos);
	XMVECTOR vLightPos = XMLoadFloat4A(&getPosition());
	XMVECTOR vdir = XMVector3Normalize(XMVectorSubtract(vObjPos, vLightPos));

	XMFLOAT4A dir; 
	XMStoreFloat4A(&dir, vdir);
	//rayCastListener->ShotRay(&object, getPosition(), dir, m_farPlane / cos(FOV / 2));
	
}

DirectX::XMFLOAT4A PointLight::getDir(b3Body & object) const
{
	using namespace DirectX;

	XMFLOAT4A objPos = XMFLOAT4A(object.GetTransform().translation.x,
		object.GetTransform().translation.y,
		object.GetTransform().translation.z, 1);

	XMVECTOR vObjPos = XMLoadFloat4A(&objPos);
	XMVECTOR vLightPos = XMLoadFloat4A(&getPosition());
	XMVECTOR vdir = XMVector3Normalize(XMVectorSubtract(vObjPos, vLightPos));

	XMFLOAT4A dir;
	XMStoreFloat4A(&dir, vdir);
	return dir;
}


void PointLight::QueueLight()
{
	DX::g_lights.push_back(this);
}

void PointLight::setPosition(const DirectX::XMFLOAT4A & pos)
{
	this->m_position = pos;
	_updateCameras();
}

void PointLight::setPosition(float x, float y, float z, float w)
{
	this->setPosition(DirectX::XMFLOAT4A(x, y, z, w));
}

void PointLight::setColor(const DirectX::XMFLOAT4A & color)
{
	this->m_color = color;
}

void PointLight::setColor(float x, float y, float z, float w)
{
	this->setColor(DirectX::XMFLOAT4A(x, y, z, w));
}

void PointLight::setIntensity(float intencsity)
{
	this->m_intensity = intencsity;
}

void PointLight::setPower(float pow)
{
	this->m_pow = pow;
}

void PointLight::setDropOff(float dropOff)
{
	this->m_dropOff = dropOff;
}

void PointLight::setNearPlane(float nearPlane)
{
	for (unsigned int i = 0; i < m_sides.size(); i++)
	{
		this->m_sides[i]->setNearPlane(nearPlane);
	}
}

void PointLight::setFarPlane(float farPlane)
{
	for (unsigned int i = 0; i < m_sides.size(); i++)
	{
		this->m_sides[i]->setFarPlane(farPlane);
	}
}

float PointLight::getDistanceFromCamera(Camera& camera)
{
	DirectX::XMVECTOR vec1 = DirectX::XMLoadFloat4A(&camera.getPosition());
	DirectX::XMVECTOR vec2 = DirectX::XMLoadFloat4A(&this->m_position);
	DirectX::XMVECTOR vecSubs = DirectX::XMVectorSubtract(vec1, vec2);
	DirectX::XMVECTOR lenght = DirectX::XMVector4Length(vecSubs);

	return DirectX::XMVectorGetX(lenght);
}

float PointLight::getDistanceFromObject(const DirectX::XMFLOAT4A& oPos)
{
	DirectX::XMVECTOR vec1 = DirectX::XMLoadFloat4A(&oPos);
	DirectX::XMVECTOR vec2 = DirectX::XMLoadFloat4A(&this->m_position);
	DirectX::XMVECTOR vecSubs = DirectX::XMVectorSubtract(vec1, vec2);
	DirectX::XMVECTOR lenght = DirectX::XMVector4Length(vecSubs);

	return DirectX::XMVectorGetX(lenght);
}

void PointLight::_createSides()
{
	using namespace DirectX;
	Camera * cam;

	float fov = 0.5f;
	

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setUP(0, 0, 1);
	cam->setDirection(0, 1, 0);
	m_sides.push_back(cam);

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setUP(1, 0, 0);
	cam->setDirection(0, -1, 0);
	m_sides.push_back(cam);

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setDirection(1, 0, 0);
	m_sides.push_back(cam);

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setDirection(-1, 0, 0);
	m_sides.push_back(cam);

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setDirection(0, 0, 1);
	m_sides.push_back(cam);

	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setDirection(0, 0, -1);
	m_sides.push_back(cam);

}

void PointLight::_createSide(const DirectX::XMFLOAT4A & dir, const DirectX::XMFLOAT4A & up)
{
	using namespace DirectX;
	Camera * cam;
	cam = new Camera(FOV, 1.0f, m_nearPlane, m_farPlane);
	cam->setPosition(this->m_position);
	cam->setUP(up);
	cam->setDirection(dir);
	m_sides.push_back(cam);
}

void PointLight::_updateCameras()
{
	for (unsigned int i = 0; i < m_sides.size(); i++)
	{
		m_sides[i]->setPosition(this->m_position);
	}
}

void PointLight::_initDirectX(UINT width, UINT hight)
{
	HRESULT hr;
	hr = DXRHC::CreateTexture2D(this->m_shadowDepthBufferTex, hight, width, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, 1, 1, 0, SHADOW_SIDES, 0, 0, DXGI_FORMAT_R32_TYPELESS);
	hr = DXRHC::CreateDepthStencilView(m_shadowDepthBufferTex, this->m_shadowDepthStencilView, 0, DXGI_FORMAT_D32_FLOAT, D3D11_DSV_DIMENSION_TEXTURE2DARRAY, 0, SHADOW_SIDES);
	hr = DXRHC::CreateShaderResourceView(m_shadowDepthBufferTex, m_shadowShaderResourceView, 0, DXGI_FORMAT_R32_FLOAT, D3D11_SRV_DIMENSION_TEXTURE2DARRAY, SHADOW_SIDES, 0, 0, 1);
	
}
