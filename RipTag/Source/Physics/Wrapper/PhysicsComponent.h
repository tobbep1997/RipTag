#pragma once

#include "../../Physics/Bounce.h"
#include "../Engine/Source/3D Engine/Components/Base/Transform.h"
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

class PhysicsComponent
{
private:

	b3Body*			 m_body = nullptr;
	b3Shape *		 m_shape = nullptr;

	b3BodyDef *		 m_bodyDef = nullptr;
					 
	b3Hull *		 m_bodyBox = nullptr;
	b3Polyhedron *	 m_poly = nullptr;
					 
	b3ShapeDef*		 m_bodyBoxDef = nullptr;

protected:
	virtual void p_updatePhysics(Transform * transform);
	virtual void p_setPosition(const float & x, const float & y, const float & z);

public:
	PhysicsComponent();
	virtual ~PhysicsComponent();

	virtual void Init(b3World & world, b3BodyType bodyType, float x = 1, float y = 1, float z = 1);

	virtual void setBaseBodyDef(b3BodyType bodyType = b3BodyType::e_dynamicBody);
	virtual void setBodyDef(BodyDefine bodyDefine);

	virtual void setBaseShapeDef();
	virtual void CreateBox(float x = 1, float y = 1, float z = 1);

	virtual void CreateBodyAndShape(b3World & world);

	virtual void setGravityScale(float gravity);

	virtual void setLiniearVelocity(float x = 0, float y = 0, float z = 0);

	virtual void addForceToCenter(float x = 0, float y = 0, float z = 0);

	virtual void Release(b3World & world);

	virtual b3Vec3 getLiniearVelocity();
	virtual void getLiniearVelocity(_Out_ float & x, _Out_ float & y, _Out_ float &z);

	virtual void setAwakeState(const bool & awa);
};
