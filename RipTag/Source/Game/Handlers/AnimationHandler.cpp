#include "AnimationHandler.h"
#include <string>
#include <vector>
#include <iostream>
#include <experimental/filesystem>
#include "../../../Engine/EngineSource/3D Engine/Extern.h"
namespace fs = std::experimental::filesystem;
#pragma warning(disable : 4996)
AnimationHandler Manager::g_animationManager;

std::string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
}

AnimationHandler::AnimationHandler()
{
}


AnimationHandler::~AnimationHandler()
{
}

bool AnimationHandler::loadClipCollection(std::string prefix, std::string newCollectionName, std::string directory, SharedSkeleton skeleton)
{
	typedef std::pair<std::wstring, std::string> FileInfo;

	std::vector<FileInfo> vFileInfo;

	std::vector<std::wstring> files;
	for (auto & p : fs::directory_iterator(directory))
		vFileInfo.push_back(std::make_pair(p.path().c_str() , "" ));

	if (!vFileInfo.size())
		return false;

	std::vector<FileInfo> qualifiedFiles;
	for (auto & fileInfo : vFileInfo)
	{
		auto filenameStart = fileInfo.first.find_last_of(L"\\");
		fileInfo.second = std::string( fileInfo.first.begin() + filenameStart + 1, fileInfo.first.end());

		if (fileInfo.second.find("animation") < fileInfo.second.size())
			qualifiedFiles.push_back(fileInfo);
		else if (fileInfo.second.find("ANIMATION") < fileInfo.second.size())
			qualifiedFiles.push_back(fileInfo);
	}

	auto thisCollection = std::make_shared<ClipCollection>();
	for (auto & fileInfo : qualifiedFiles)
	{
		std::string thinString = ws2s(fileInfo.first);
		auto animation = Animation::LoadAndCreateAnimation(thinString, skeleton);
		

		std::string name = fileInfo.second;
		name.erase(name.end() - 3, name.end());

		thisCollection->insert(std::make_pair(name, animation));
	}
	m_clipCollectionMap.insert(std::make_pair(newCollectionName, thisCollection));
	return true;
}

bool AnimationHandler::loadClip(std::string file, std::string collection)
{
	std::shared_ptr<Animation::Skeleton> skeleton = nullptr;
	try
	{
		skeleton = m_clipCollectionMap.at(collection)->begin()->second->m_skeleton;
	}
	catch (std::out_of_range e)
	{
		return false;
	}

	if (!skeleton)
		return false;


	auto animation = Animation::LoadAndCreateAnimation(file, skeleton);
	auto filenameStart = file.find_last_of("/");
	std::string name(file.begin() + filenameStart + 1, file.end() - 3);
	m_clipCollectionMap.at(collection)->insert(std::make_pair(name, animation));

	return true;
}

bool AnimationHandler::loadSkeleton(std::string file, std::string key)
{
	auto skeleton = Animation::LoadAndCreateSkeleton(file);

	if (!skeleton)
		return false;

	m_skeletonMap.insert(std::make_pair(key, skeleton));
}

SharedSkeleton AnimationHandler::getSkeleton(std::string key)
{
	return m_skeletonMap.at(key);
}

SharedClipCollection AnimationHandler::getClipCollection(std::string collectionKey)
{
	return m_clipCollectionMap.at(collectionKey);
}

SharedAnimation AnimationHandler::getAnimation(std::string collectionKey, std::string animationKey)
{
	return m_clipCollectionMap.at(collectionKey)->at(animationKey);
}
