#include "PhysicsComponent.h"
#include <iostream>
#include "EngineSource/3D Engine/RenderingManager.h"
#pragma warning (disable : 4312)

void PhysicsComponent::p_updatePhysics(Transform * transform)
{
	transform->setPosition(m_body->GetTransform().translation.x,
		m_body->GetTransform().translation.y,
		m_body->GetTransform().translation.z);

	b3Mat33 mat = m_body->GetTransform().rotation;
	transform->setPhysicsRotation(mat);
	
}

void PhysicsComponent::p_setPosition(const  float& x, const float& y, const float& z)
{
	m_body->SetTransform(b3Vec3(x, y, z), m_body->GetQuaternion());	
}

void PhysicsComponent::p_setPositionRot(const float & x, const float & y, const float & z, const float & pitch, const float & yaw, const float & roll)
{
	DirectX::XMVECTOR t = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

	float xx = DirectX::XMVectorGetX(t);
	float yy = DirectX::XMVectorGetY(t);
	float zz = DirectX::XMVectorGetZ(t);
	float ww = DirectX::XMVectorGetW(t);
	m_body->SetTransform(b3Vec3(x, y, z), b3Quaternion(xx, yy, zz, ww));
	
}

void PhysicsComponent::p_setRotation(const float& pitch, const float& yaw, const float& roll)
{
	DirectX::XMVECTOR t = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

	float xx = DirectX::XMVectorGetX(t);
	float yy = DirectX::XMVectorGetY(t);
	float zz = DirectX::XMVectorGetZ(t);
	float ww = DirectX::XMVectorGetW(t);
	m_body->SetTransform(m_body->GetTransform().translation, b3Quaternion(xx, yy, zz, ww));
}

PhysicsComponent::PhysicsComponent()
{
}

PhysicsComponent::~PhysicsComponent()
{
	
}

void PhysicsComponent::Init(b3World& world, b3BodyType bodyType, float x, float y, float z, bool sensor)
{
	setBaseBodyDef(bodyType);
	CreateBox(x, y, z);
	setBaseShapeDef(sensor);
	CreateBodyAndShape(world);
}

void PhysicsComponent::Init(b3World & world, const MyLibrary::CollisionBoxes & collisionBoxes)
{
	singelCollider = false;
	//setBaseBodyDef---------------------------------------
	m_bodyDef = new b3BodyDef();
	m_bodyDef->position.Set(0, 0, 0);
	m_bodyDef->type = e_staticBody;
	m_bodyDef->gravityScale = 1;
	m_bodyDef->linearVelocity = b3Vec3(0, 0, 0);
	//-----------------------------------------------------
		

	b3Hull * h;
	b3Polyhedron * p;
	b3ShapeDef* s;

	for (unsigned int i = 0; i < collisionBoxes.nrOfBoxes; i++)
	{
		h = new b3Hull();
		h->SetAsBox(b3Vec3(collisionBoxes.boxes[i].scale[0] / 2.0f, collisionBoxes.boxes[i].scale[1] / 2.0f, collisionBoxes.boxes[i].scale[2] / 2.0f));
		m_hulls.push_back(h);

		//-----------------------------------------------------

		p = new b3Polyhedron();
		p->SetHull(h);
		m_polys.push_back(p);
		

		//-----------------------------------------------------

		s = new b3ShapeDef();
		s->shape = p;
		s->density = 1.0f;
		s->restitution = 0;
		s->friction = 0;
		s->userData = (void*)collisionBoxes.boxes[i].typeOfBox;
		m_shapeDefs.push_back(s);
		
	}

	for (unsigned int i = 0; i < collisionBoxes.nrOfBoxes; i++)
	{
		b3Body * b = world.CreateBody(*m_bodyDef);
		
	
		b->SetTransform(b3Vec3(collisionBoxes.boxes[i].translation[0], collisionBoxes.boxes[i].translation[1], collisionBoxes.boxes[i].translation[2]),
			b3Quaternion(collisionBoxes.boxes[i].rotation[0], collisionBoxes.boxes[i].rotation[1], collisionBoxes.boxes[i].rotation[2], collisionBoxes.boxes[i].rotation[3]));
		
		b->SetUserData((void*)collisionBoxes.boxes[i].typeOfBox);
		m_bodys.push_back(b);
		m_shapes.push_back(b->CreateShape(*m_shapeDefs[i]));
	}
  }



void PhysicsComponent::setBaseBodyDef(b3BodyType bodyType)
{
	m_bodyDef = new b3BodyDef();
	m_bodyDef->position.Set(0, 0, 0);
	m_bodyDef->type = bodyType;
	m_bodyDef->gravityScale = 1;
	m_bodyDef->linearVelocity = b3Vec3(0, 0, 0);
	
}

void PhysicsComponent::setBodyDef(BodyDefine bodyDefine)
{
	m_bodyDef = new b3BodyDef();
	m_bodyDef->position.Set(bodyDefine.posX, bodyDefine.posY, bodyDefine.posZ);
	m_bodyDef->type = bodyDefine.bodyType;
	m_bodyDef->angularVelocity = bodyDefine.angularVelocity;
	m_bodyDef->linearVelocity = bodyDefine.linearVelocity;
	m_bodyDef->awake = bodyDefine.awake;
	m_bodyDef->gravityScale = bodyDefine.gravityScale;
}

void PhysicsComponent::setBaseShapeDef(bool sensor)
{
	//Create a base shape definition
	m_bodyBoxDef = new b3ShapeDef();
	m_bodyBoxDef->shape = m_poly;
	m_bodyBoxDef->density = 1.0f;
	m_bodyBoxDef->restitution = 0;
	m_bodyBoxDef->sensor = sensor;


	m_bodyBoxDef->density = 1.0f;
	m_bodyBoxDef->friction = 1.0f;
	m_bodyBoxDef->restitution = 0;


	//everything in this except the shape you can set using m_shape
}

void PhysicsComponent::CreateBox(float x, float y, float z)
{
	if (m_bodyBox == nullptr)
	{
		//Create the box
		m_bodyBox = new b3Hull();
		m_bodyBox->SetAsBox(b3Vec3(x, y, z));
		//Then the poly
		m_poly = new b3Polyhedron();
		m_poly->SetHull(m_bodyBox);
	}
	else
	{
		//Destory the exsiting shape
		//And delete the current body
		m_body->DestroyShape(m_shape);
		delete m_bodyBox;

		//Create a new Box
		m_bodyBox = new b3Hull();
		m_bodyBox->SetAsBox(b3Vec3(x, y, z));

		//Set the Polyhedron to the new box
		//Aka connect the box to the wrapper
		m_poly->SetHull(m_bodyBox);

		//Connect the bodyDef to the shape
		m_bodyBoxDef->shape = m_poly;

		//Create the shape
		m_shape = m_body->CreateShape(*m_bodyBoxDef);
	}
	
}

void PhysicsComponent::CreateBodyAndShape(b3World& world)
{
	m_body = world.CreateBody(*m_bodyDef);
	m_shape = m_body->CreateShape(*m_bodyBoxDef);
}

void PhysicsComponent::setGravityScale(float gravity)
{
	m_body->SetGravityScale(gravity);
}

void PhysicsComponent::setLiniearVelocity(float x, float y, float z)
{
	m_body->SetLinearVelocity(b3Vec3(x, y, z));
}

void PhysicsComponent::addForceToCenter(float x, float y, float z)
{
	m_body->ApplyForceToCenter(b3Vec3(x, y, z), true);
}

void PhysicsComponent::Release(b3World& world)
{
	delete m_bodyDef;
	if (singelCollider)
	{
		m_body->DestroyShape(m_shape);
		delete m_poly;
		delete m_bodyBox;
		delete m_bodyBoxDef;
		world.DestroyBody(m_body);
	}




	for (int i = 0; i < m_bodys.size(); i++) 
		m_bodys[i]->DestroyShape(m_shapes[i]);
	for (int i = 0; i < m_polys.size(); i++)
		delete m_polys[i];
	for (int i = 0; i < m_hulls.size(); i++)
		delete m_hulls[i];
	for (int i = 0; i < m_shapeDefs.size(); i++)
		delete m_shapeDefs[i];
	for (int i = 0; i < m_bodys.size(); i++)
		world.DestroyBody(m_bodys[i]);

	
	
}

b3Vec3 PhysicsComponent::getLiniearVelocity()
{
	return m_body->GetLinearVelocity();
}

void PhysicsComponent::getLiniearVelocity(float& x, float& y, float& z)
{
	b3Vec3 temp = m_body->GetLinearVelocity();
	x = temp.x;
	y = temp.y;
	z = temp.z;
}

b3Body* PhysicsComponent::getBody()
{
	return this->m_body;
}

void PhysicsComponent::setAwakeState(const bool& awa)
{
	m_body->SetAwake(awa);
}

void PhysicsComponent::setUserDataBody(void* self)
{
	this->m_body->SetUserData(self);
}

void PhysicsComponent::setObjectTag(const char * type)
{
	m_body->SetObjectTag(type);
}
