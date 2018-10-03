#pragma once

#include "../../Physics/Bounce.h"
#include "../Engine/Source/3D Engine/Components/Base/Transform.h"


class PhysicsComponent
{
private:

	b3Body*			 m_body;
	b3Shape *		 m_shape;

	b3BodyDef *		 m_bodyDef;
					 
	b3Hull *		 m_bodyBox;
	b3Polyhedron *	 m_poly;
					 
	b3ShapeDef*		 m_bodyBoxDef;

protected:
	virtual void p_updatePhysics(Transform * transform);
	virtual void p_setPosition(const float & x, const float & y, const float & z);

public:
	PhysicsComponent();
	virtual ~PhysicsComponent();

	virtual void Init(b3World & world, b3BodyType bodyType, float x = 1, float y = 1, float z = 1);

	virtual void setBaseBodyDef(b3BodyType bodyType = b3BodyType::e_dynamicBody);

	virtual void setBaseShapeDef();
	virtual void CreateBox(float x = 1, float y = 1, float z = 1);

	virtual void CreateBodyAndShape(b3World & world);


	virtual void setLiniearVelocity(float x = 0, float y = 0, float z = 0);

	virtual void addForceToCenter(float x = 0, float y = 0, float z = 0);

	virtual void Release(b3World & world);
};
