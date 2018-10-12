#include "PhysicsComponent.h"
#include <iostream>
#include "Source/3D Engine/RenderingManager.h"

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
	

	//double roll;
	//double pitch;
	//double yaw;

	//b3Mat33 mat = m_body->GetTransform().rotation;
	////b3Quaternion q = m_body->GetQuaternion();
	//mat = b3Transpose(mat);
	////mat = b3Inverse(mat);
	//pitch = atan2(mat.z.y, mat.z.z);
	//yaw = atan2(-mat.z.x, sqrt(pow(mat.z.y, 2) + pow(mat.z.z, 2)));
	//roll = atan2(mat.y.x, mat.x.x);

	/*roll = roll * -1;
	yaw = yaw * -1;
	pitch = pitch * -1;*/
	//x = x * DirectX::XM_PI;
	//if (x != 1.5708f)
	//{
	//	std::cout << "X : " << x << std::endl;
	//}
	

	//double sinr_cosp = +2.0 * (q.d * q.a + q.b * q.c);
	//double cosr_cosp = +1.0 - 2.0 * (q.a * q.a + q.b * q.b);
	//roll = atan2(sinr_cosp, cosr_cosp);

	//// pitch (y-axis rotation)
	//double sinp = +2.0 * (q.d * q.b - q.c * q.a);
	////if (fabs(sinp) >= 1)
	//	//pitch = copysign(DirectX::XM_PI / 2, sinp); // use 90 degrees if out of range
	///*else*/
	//pitch = asin(sinp);
	//pitch = pitch * DirectX::XM_PI;
	//// yaw (z-axis rotation)
	//double siny_cosp = +2.0 * (q.d * q.c + q.a * q.b);
	//double cosy_cosp = +1.0 - 2.0 * (q.b * q.b + q.c * q.c);
	//yaw = atan2(siny_cosp, cosy_cosp);

	//ImGui::Begin("box1");
	//ImGui::Text("Roll; %f", roll);
	//ImGui::Text("pitch; %f", pitch);
	//ImGui::Text("yaw; %f", yaw);
	///*std::cout << "Roll: " << roll << std::endl;
	//std::cout << "pitch: " << pitch << std::endl;
	//std::cout << "yaw: " << yaw << std::endl;*/
	//ImGui::End();

	//transform->setRotation(pitch, yaw, roll);
	//m_body->SetTransform()
	//transform->addRotation(pitch, yaw, roll);
}

//x = -1.5
//y = 2.1
//z = -2.1

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
	//Create a base shape definition
	m_bodyBoxDef = new b3ShapeDef();
	m_bodyBoxDef->shape = m_poly;
	m_bodyBoxDef->density = 1.0f;
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

b3Body* PhysicsComponent::getBody()
{
	return this->m_body;
}
