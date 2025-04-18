﻿

#include "spine_loader.h"

std::shared_ptr<spine::SkeletonData> spine_loader::ReadTextSkeletonFromFile(const spine::String& filename, spine::Atlas* atlas, float scale)
{
	spine::SkeletonJson json(atlas);
	json.setScale(scale);
	auto skeletonData = json.readSkeletonDataFile(filename);
	if (!skeletonData)
	{
		return nullptr;
	}
	return std::shared_ptr<spine::SkeletonData>(skeletonData);
}

std::shared_ptr<spine::SkeletonData> spine_loader::ReadBinarySkeletonFromFile(const spine::String& filename, spine::Atlas* atlas, float scale)
{
	spine::SkeletonBinary binary(atlas);
	binary.setScale(scale);
	auto skeletonData = binary.readSkeletonDataFile(filename);
	if (!skeletonData)
	{
		return nullptr;
	}
	return std::shared_ptr<spine::SkeletonData>(skeletonData);
}

std::shared_ptr<spine::SkeletonData> spine_loader::ReadTextSkeletonFromMemory(const std::string& skeleton, spine::Atlas* atlas, float scale)
{
	spine::SkeletonJson json(atlas);
	json.setScale(scale);
	auto skeletonData = json.readSkeletonData(skeleton.c_str());
	if (!skeletonData)
	{
		return nullptr;
	}
	return std::shared_ptr<spine::SkeletonData>(skeletonData);
}

std::shared_ptr<spine::SkeletonData> spine_loader::ReadBinarySkeletonFromMemory(const std::string& skeleton, spine::Atlas* atlas, float scale)
{
	spine::SkeletonBinary binary(atlas);
	binary.setScale(scale);
	auto skeletonData = binary.readSkeletonData(reinterpret_cast<const unsigned char*>(skeleton.c_str()), static_cast<int>(skeleton.size()));
	if (!skeletonData)
	{
		return nullptr;
	}
	return std::shared_ptr<spine::SkeletonData>(skeletonData);
}
