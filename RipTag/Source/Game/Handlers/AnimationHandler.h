#pragma once
#include "../../../../Engine/Source/3D Engine/Model/Meshes/AnimatedModel.h"
#include <unordered_map>

typedef std::shared_ptr<Animation::AnimationClip> SharedAnimation;
typedef std::unordered_map<std::string, SharedAnimation> ClipCollection;
typedef std::shared_ptr<ClipCollection> SharedClipCollection;
typedef std::unordered_map<std::string, std::shared_ptr<ClipCollection>> ClipCollectionMap;

class AnimationHandler
{
public:
	AnimationHandler();
	~AnimationHandler();

	///Loads all animations with prefix and adds a collection with newCollectionName key
	bool loadClipCollection(std::string prefix, std::string newCollectionName);

	///Loads all animations with prefix and adds a collection with newCollectionName key under directory
	bool loadClipCollection(std::string prefix, std::string newCollectionName, std::string directory, std::string skeletonskeletonPath);

	///Loads filename as animation and adds to collection
	bool loadClip(std::string file, std::string collection);
	
	///Get collection under key
	SharedClipCollection getClipCollection(std::string collectionKey);

	///Get clip under animation key under collection key
	SharedAnimation getAnimation(std::string collectionKey, std::string animationKey);

private:
	ClipCollectionMap m_clipCollectionMap;
};

