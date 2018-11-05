#pragma once
#include <vector>

struct StaticVertex;

namespace ImporterLibrary
{
	struct CollisionBoxes;
};

class StaticMesh
{
private:
	std::string m_meshName;
	std::vector<StaticVertex> m_staticVertex;
	ImporterLibrary::CollisionBoxes * m_collisionBox;
public:
	StaticMesh();
	~StaticMesh();

	const StaticVertex * getRawVertice() const;
	const std::vector<StaticVertex> & getVertice() const;
	void setVertices(std::vector<StaticVertex>& input);

	const ImporterLibrary::CollisionBoxes & getCollisionBoxes() const;

	void SET_DEFAULT();

	void setName(const std::string & name);
	const std::string & getName() const;

	void LoadMesh(const std::string & path);
	void LoadCollision(const std::string & path);
};

