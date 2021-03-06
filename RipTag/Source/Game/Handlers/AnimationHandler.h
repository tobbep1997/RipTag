#pragma once
#include <unordered_map>

namespace Animation
{
	struct AnimationClip;
	struct Skeleton;
};

typedef std::shared_ptr<Animation::AnimationClip> SharedAnimation;
typedef std::shared_ptr<Animation::Skeleton> SharedSkeleton;
typedef std::unordered_map<std::string, SharedSkeleton> SkeletonMap;
typedef std::unordered_map<std::string, SharedAnimation> ClipCollection;
typedef std::shared_ptr<ClipCollection> SharedClipCollection;
typedef std::unordered_map<std::string, std::shared_ptr<ClipCollection>> ClipCollectionMap;

class AnimationHandler
{
public:
	AnimationHandler();
	~AnimationHandler();

	///Loads all animations with prefix and adds a collection with newCollectionName key under directory
	bool loadClipCollection(std::string prefix, std::string newCollectionName, std::string directory, SharedSkeleton skeleton);

	///Loads filename as animation and adds to collection
	bool loadClip(std::string file, std::string collection);
	
	///Loads filename as skeleton
	bool loadSkeleton(std::string file, std::string key);

	///Get skeleton under key
	SharedSkeleton getSkeleton(std::string key);

	///Get collection under key
	SharedClipCollection getClipCollection(std::string collectionKey);

	///Get clip under animation key under collection key
	SharedAnimation getAnimation(std::string collectionKey, std::string animationKey);

private:
	ClipCollectionMap m_clipCollectionMap;
	SkeletonMap m_skeletonMap;
};

