#pragma once
#include <vector>
#include "../../Physics/Bounce.h"

struct BodyDefine
{
	float posX = 0;
	float posY = 0;
	float posZ = 0;
	b3BodyType bodyType = b3BodyType::e_dynamicBody;
	b3Vec3 angularVelocity = b3Vec3(0,0,0);
	b3Vec3 linearVelocity = b3Vec3(0, 0, 0);
	bool awake = true;
	float gravityScale = 1;
};

namespace ImporterLibrary {
	struct CollisionBoxes;
};

class Transform;

struct CollisionObject
{
	CollisionObject(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scale)
	{
		this->pos = pos;
		this->scale = scale;
	}
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 scale;
};

class PhysicsComponent
{
private:
	bool singelCollider = true;
	bool isInited = false;

	b3Body*			 m_body = nullptr;
	b3Shape *		 m_shape = nullptr;

	b3BodyDef *		 m_bodyDef = nullptr;
					 
	b3Hull *		 m_bodyBox = nullptr;
	b3Polyhedron *	 m_poly = nullptr;
					 
	b3ShapeDef*		 m_bodyBoxDef = nullptr;

	std::vector<b3Hull*> m_hulls;
	std::vector<b3Polyhedron*> m_polys;
	std::vector<b3ShapeDef*> m_shapeDefs;
	std::vector <b3Body*>	m_bodys;
	std::vector <b3Shape *> m_shapes;
	std::vector <b3Vec3> m_scales;

protected:
	
	
public:
	virtual void p_updatePhysics(Transform * transform);
	virtual void p_setPosition(const float & x, const float & y, const float & z);
	virtual void p_setPositionRot(const float & x, const float & y, const float & z, const float & pitch, const float & yaw, const float & roll);

	virtual void p_addRotation(const float & pitch, const float & yaw, const float & roll);

	virtual void p_setRotation(const float & pitch, const float & yaw, const float & roll);
	PhysicsComponent();
	virtual ~PhysicsComponent();

	virtual b3BodyType getBodyType() { return this->m_bodyDef->type; };
	virtual void Init(b3World & world, b3BodyType bodyType, float x = 1, float y = 1, float z = 1, bool sensor = false, float friction = 0);
	virtual void Init(b3World & world, const ImporterLibrary::CollisionBoxes & collisionBoxes, float friction = 0.1f);

	virtual void addCollisionBox(b3Vec3 pos, b3Vec3 size, b3Quaternion rotation, std::string type, bool sensor, b3World * world);

	virtual void setBaseBodyDef(b3BodyType bodyType = b3BodyType::e_dynamicBody);
	virtual void setBodyDef(BodyDefine bodyDefine);

	virtual void setBaseShapeDef(bool sensor = false, float friction = 1);
	virtual void CreateBox(float x = 1, float y = 1, float z = 1);

	virtual void CreateBodyAndShape(b3World & world);

	virtual void CreateShape(b3Vec3 position = b3Vec3(0,0,0), b3Vec3 size = b3Vec3(1, 1, 1), float density = 1.0f, float friction = 0.0f, std::string objectTag = "NULL", bool isSensor = false);

	virtual void setGravityScale(float gravity);

	virtual void setLiniearVelocity(float x = 0, float y = 0, float z = 0);

	virtual void addForceToCenter(float x = 0, float y = 0, float z = 0);

	virtual void Release(b3World & world);

	virtual b3Vec3 getLiniearVelocity();
	virtual void getLiniearVelocity(_Out_ float & x, _Out_ float & y, _Out_ float &z);

	virtual void setAwakeState(const bool & awa);

	virtual void setUserDataBody(void * self);
	virtual void setObjectTag(const char * type);

	

	virtual b3Body* getBody();
	const b3Hull * getHull();
};
