#include "RipTagPCH.h"
#include "PhysicsComponent.h"

#include <iostream>

#include "EngineSource/3D Engine/RenderingManager.h"
#include "EngineSource/3D Engine/Components/Base/Transform.h"
#include "ImportLibrary/FormatHeader.h"

#include <AudioEngine.h>

#pragma warning (disable : 4312)

void PhysicsComponent::p_updatePhysics(Transform * transform)
{
	transform->setPosition(m_body->GetTransform().translation.x,
		m_body->GetTransform().translation.y,
		m_body->GetTransform().translation.z);

	// #todoREMOVE
	auto vel = m_body->GetLinearVelocity();
	transform->setVelocity(vel);

	b3Mat33 mat = m_body->GetTransform().rotation;
	transform->setPhysicsRotation(mat);
	
}

void PhysicsComponent::p_setPosition(const  float& x, const float& y, const float& z)
{
	m_body->SetTransform(b3Vec3(x, y, z), m_body->GetQuaternion());	
}

void PhysicsComponent::p_setPositionRot(const float & x, const float & y, const float & z, const float & pitch, const float & yaw, const float & roll)
{
	b3Quaternion qu = m_body->GetQuaternion();
	qu.SetIdentity();
	DirectX::XMVECTOR vec = DirectX::XMVectorSet(qu.a, qu.b, qu.c, qu.d);

	//add == mulitypyy

	DirectX::XMVECTOR t = DirectX::XMQuaternionRotationRollPitchYaw(pitch, 0, 0);

	t = DirectX::XMQuaternionMultiply(vec, t);



	float xx = DirectX::XMVectorGetX(t);
	float yy = DirectX::XMVectorGetY(t);
	float zz = DirectX::XMVectorGetZ(t);
	float ww = DirectX::XMVectorGetW(t);
	m_body->SetTransform(b3Vec3(x, y, z), b3Quaternion(xx, yy, zz, ww));

	///STPP
	qu = m_body->GetQuaternion();
	vec = DirectX::XMVectorSet(qu.a, qu.b, qu.c, qu.d);

	//add == mulitypyy

	t = DirectX::XMQuaternionRotationRollPitchYaw(0, yaw, 0);

	t = DirectX::XMQuaternionMultiply(vec, t);



	xx = DirectX::XMVectorGetX(t);
	yy = DirectX::XMVectorGetY(t);
	zz = DirectX::XMVectorGetZ(t);
	ww = DirectX::XMVectorGetW(t);
	m_body->SetTransform(b3Vec3(x, y, z), b3Quaternion(xx, yy, zz, ww));

	//STOP
	qu = m_body->GetQuaternion();
	vec = DirectX::XMVectorSet(qu.a, qu.b, qu.c, qu.d);

	//add == mulitypyy

	t = DirectX::XMQuaternionRotationRollPitchYaw(0, 0, roll);

	t = DirectX::XMQuaternionMultiply(vec, t);



	xx = DirectX::XMVectorGetX(t);
	yy = DirectX::XMVectorGetY(t);
	zz = DirectX::XMVectorGetZ(t);
	ww = DirectX::XMVectorGetW(t);
	m_body->SetTransform(b3Vec3(x, y, z), b3Quaternion(xx, yy, zz, ww));
	
}

void PhysicsComponent::p_setRotation(const float& pitch, const float& yaw, const float& roll)
{
	b3Quaternion qu = m_body->GetQuaternion();
	qu.SetIdentity();
	DirectX::XMVECTOR vec = DirectX::XMVectorSet(qu.a, qu.b, qu.c, qu.d);

	//add == mulitypyy

	DirectX::XMVECTOR t = DirectX::XMQuaternionRotationRollPitchYaw(pitch, 0, 0);

	t = DirectX::XMQuaternionMultiply(vec, t);



	float xx = DirectX::XMVectorGetX(t);
	float yy = DirectX::XMVectorGetY(t);
	float zz = DirectX::XMVectorGetZ(t);
	float ww = DirectX::XMVectorGetW(t);
	m_body->SetTransform(m_body->GetTransform().translation, b3Quaternion(xx, yy, zz, ww));

	///STPP
	qu = m_body->GetQuaternion();
	vec = DirectX::XMVectorSet(qu.a, qu.b, qu.c, qu.d);

	//add == mulitypyy

	t = DirectX::XMQuaternionRotationRollPitchYaw(0, yaw, 0);

	t = DirectX::XMQuaternionMultiply(vec, t);



	xx = DirectX::XMVectorGetX(t);
	yy = DirectX::XMVectorGetY(t);
	zz = DirectX::XMVectorGetZ(t);
	ww = DirectX::XMVectorGetW(t);
	m_body->SetTransform(m_body->GetTransform().translation, b3Quaternion(xx, yy, zz, ww));

	//STOP
	qu = m_body->GetQuaternion();
	vec = DirectX::XMVectorSet(qu.a, qu.b, qu.c, qu.d);

	//add == mulitypyy

	t = DirectX::XMQuaternionRotationRollPitchYaw(0, 0, roll);

	t = DirectX::XMQuaternionMultiply(vec, t);



	xx = DirectX::XMVectorGetX(t);
	yy = DirectX::XMVectorGetY(t);
	zz = DirectX::XMVectorGetZ(t);
	ww = DirectX::XMVectorGetW(t);
	m_body->SetTransform(m_body->GetTransform().translation, b3Quaternion(xx, yy, zz, ww));
	//m_body->SetTransform(m_body->GetTransform().translation, b3Quaternion(xx, yy, zz, ww));
}

void PhysicsComponent::p_addRotation(const float& pitch, const float& yaw, const float& roll)
{
	b3Quaternion qu = m_body->GetQuaternion();
	DirectX::XMVECTOR vec = DirectX::XMVectorSet(qu.a, qu.b, qu.c, qu.d);

	//add == mulitypyy

	DirectX::XMVECTOR t = DirectX::XMQuaternionRotationRollPitchYaw(pitch, 0, 0);

	t = DirectX::XMQuaternionMultiply(vec, t);



	float xx = DirectX::XMVectorGetX(t);
	float yy = DirectX::XMVectorGetY(t);
	float zz = DirectX::XMVectorGetZ(t);
	float ww = DirectX::XMVectorGetW(t);
	m_body->SetTransform(m_body->GetTransform().translation, b3Quaternion(xx, yy, zz, ww));

	qu = m_body->GetQuaternion();
	vec = DirectX::XMVectorSet(qu.a, qu.b, qu.c, qu.d);

	//add == mulitypyy

	t = DirectX::XMQuaternionRotationRollPitchYaw(0, yaw, 0);

	t = DirectX::XMQuaternionMultiply(vec, t);



	xx = DirectX::XMVectorGetX(t);
	yy = DirectX::XMVectorGetY(t);
	zz = DirectX::XMVectorGetZ(t);
	ww = DirectX::XMVectorGetW(t);
	m_body->SetTransform(m_body->GetTransform().translation, b3Quaternion(xx, yy, zz, ww));

	//STOP
	qu = m_body->GetQuaternion();
	vec = DirectX::XMVectorSet(qu.a, qu.b, qu.c, qu.d);

	//add == mulitypyy

	t = DirectX::XMQuaternionRotationRollPitchYaw(0, 0, roll);

	t = DirectX::XMQuaternionMultiply(vec, t);



	xx = DirectX::XMVectorGetX(t);
	yy = DirectX::XMVectorGetY(t);
	zz = DirectX::XMVectorGetZ(t);
	ww = DirectX::XMVectorGetW(t);
	m_body->SetTransform(m_body->GetTransform().translation, b3Quaternion(xx, yy, zz, ww));
	
}


PhysicsComponent::PhysicsComponent()
{

}

PhysicsComponent::~PhysicsComponent()
{
	
}

void PhysicsComponent::Init(b3World& world, b3BodyType bodyType, float x, float y, float z, bool sensor, float friction)
{
	setBaseBodyDef(bodyType);
	CreateBox(x, y, z);
	setBaseShapeDef(sensor,friction);
	CreateBodyAndShape(world);
}

void PhysicsComponent::Init(b3World & world, const ImporterLibrary::CollisionBoxes & collisionBoxes, float friction)
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
		s->friction = friction;
		
		s->userData = (void*)collisionBoxes.boxes[i].typeOfBox;
		m_shapeDefs.push_back(s);
		
	}

	for (unsigned int i = 0; i < collisionBoxes.nrOfBoxes; i++)
	{
		b3Body * b = world.CreateBody(*m_bodyDef);
		b->SetObjectTag("WORLD");
		b->SetTransform(b3Vec3(collisionBoxes.boxes[i].translation[0], collisionBoxes.boxes[i].translation[1], collisionBoxes.boxes[i].translation[2]),
			b3Quaternion(collisionBoxes.boxes[i].rotation[0], collisionBoxes.boxes[i].rotation[1], collisionBoxes.boxes[i].rotation[2], collisionBoxes.boxes[i].rotation[3]));
		
		if (collisionBoxes.boxes[i].typeOfBox == 1)
			b->SetObjectTag("BLINK_WALL");

		if (collisionBoxes.boxes[i].typeOfBox == 2)
			b->SetObjectTag("WIN_BOX");
		//b->SetUserData((void*)collisionBoxes.boxes[i].typeOfBox);
		m_bodys.push_back(b);
		m_shapes.push_back(b->CreateShape(*m_shapeDefs[i]));
	}
  }

void PhysicsComponent::addCollisionBox(b3Vec3 pos, b3Vec3 size, b3Quaternion rotation, std::string type, bool sensor, b3World * world)
{


	b3Hull * h;
	h = new b3Hull();
	h->SetAsBox(b3Vec3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f));
	m_hulls.push_back(h);


	b3Polyhedron * p;
	p = new b3Polyhedron();
	p->SetHull(h);
	m_polys.push_back(p);


	b3ShapeDef* s;
	s = new b3ShapeDef();
	s->shape = p;
	s->density = 1.0f;
	s->restitution = 0;
	s->friction = 0;
	s->sensor = sensor;
	m_shapeDefs.push_back(s);
	
	/*
	PLAYER
	LEVER
	ENEMY
	PRESSUREPLATE
	BLINK_WALL
	*/

	

	b3Body * b = world->CreateBody(*m_bodyDef);
	
	b->SetObjectTag(type);
	b->SetTransform(pos, rotation);
	m_bodys.push_back(b);
	m_shapes.push_back(b->CreateShape(*s));
}



void PhysicsComponent::setBaseBodyDef(b3BodyType bodyType)
{
	m_bodyDef = new b3BodyDef();
	m_bodyDef->position.Set(0, 0, 0);
	m_bodyDef->type = bodyType;
	m_bodyDef->gravityScale = 9.82f;
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

void PhysicsComponent::setBaseShapeDef(bool sensor, float friction)
{
	//Create a base shape definition
	m_bodyBoxDef = new b3ShapeDef();
	m_bodyBoxDef->shape = m_poly;
	m_bodyBoxDef->density = 1.0f;
	m_bodyBoxDef->restitution = 0;
	m_bodyBoxDef->sensor = sensor;


	m_bodyBoxDef->density = 1.0f;
	m_bodyBoxDef->friction = friction;
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

void PhysicsComponent::CreateShape(float x, float y, float z, float sizeX, float sizeY, float sizeZ, std::string objectTag)
{
	b3Hull* hull = DBG_NEW b3Hull();
	hull->SetAsBox(b3Vec3(sizeX, sizeY, sizeZ));
	b3Polyhedron* polyhedron = DBG_NEW b3Polyhedron();
	polyhedron->SetHull(hull);
	//polyhedron->SetTransform(b3Vec3(x, y, z), m_body->GetQuaternion());
	polyhedron->SetObjectTag(objectTag);
	//b3Shape* shape = polyhedron;//
	b3ShapeDef* s;
	s = DBG_NEW b3ShapeDef();
	s->shape = polyhedron;
	s->density = 1.0f;
	s->restitution = 0;
	s->friction = 1;
	s->sensor = false;
	b3Transform pos;
	pos.SetIdentity();
	pos.translation = b3Vec3(x, y, z);
	s->local = pos;
	m_body->CreateShape(*s);
	m_shapeDefs.push_back(s);
	m_hulls.push_back(hull);
	delete polyhedron;
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

