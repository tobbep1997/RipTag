#include "PhysicsComponent.h"
#include <iostream>

void PhysicsComponent::p_updatePhysics(Transform * transform)
{
	transform->setPosition(m_body->GetTransform().translation.x,
		m_body->GetTransform().translation.y,
		m_body->GetTransform().translation.z);
	/*transform->setRotation(m_body->GetTransform().rotation.x,
		m_body->GetTransform().rotation.y,
		m_body->GetTransform().rotation.z);*/
	//transform->setRotation(m_body->GetTransform().rotation);
	//m_shape->GetTransform().rotation;
	b3Mat33 mat = m_body->GetTransform().rotation;
	float x = atan2(mat.z.z, mat.z.y);
	//x = x * DirectX::XM_PI;
	if (x != 1.5708f)
	{
		std::cout << "X : " << x << std::endl;
	}
	
}

void PhysicsComponent::p_setPosition(const  float& x, const float& y, const float& z)
{
	m_body->SetTransform(b3Vec3(x, y, z), b3Vec3(0,0,0), 0);
	//m_shape->ComputeAabb()
	
}

PhysicsComponent::PhysicsComponent()
{
}

PhysicsComponent::~PhysicsComponent()
{
	
}

void PhysicsComponent::Init(b3World& world, b3BodyType bodyType, float x, float y, float z)
{
	setBaseBodyDef(bodyType);
	CreateBox(x, y, z);
	setBaseShapeDef();
	CreateBodyAndShape(world);
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

void PhysicsComponent::setBaseShapeDef()
{
	m_bodyBoxDef = new b3ShapeDef();
	m_bodyBoxDef->shape = m_poly;
	m_bodyBoxDef->density = 1.0f;
	m_bodyBoxDef->restitution = 0;
	//m_bodyBoxDef->friction = -1;
}

void PhysicsComponent::CreateBox(float x, float y, float z)
{
	m_bodyBox = new b3Hull();
	m_bodyBox->SetAsBox(b3Vec3(x, y, z));

	m_poly = new b3Polyhedron();
	m_poly->SetHull(m_bodyBox);
}

void PhysicsComponent::CreateBodyAndShape(b3World& world)
{
	m_body = world.CreateBody(*m_bodyDef);
	m_shape = m_body->CreateShape(*m_bodyBoxDef);
}

void PhysicsComponent::setLiniearVelocity(float x, float y, float z)
{
	m_body->SetLinearVelocity(b3Vec3(x,y,z));
}

void PhysicsComponent::addForceToCenter(float x, float y, float z)
{
	m_body->ApplyForceToCenter(b3Vec3(x, y, z), true);
}

void PhysicsComponent::Release(b3World& world)
{
	m_body->DestroyShape(m_shape);
	delete m_poly;
	delete m_bodyBox;
	delete m_bodyDef;
	delete m_bodyBoxDef;
	world.DestroyBody(m_body);
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
