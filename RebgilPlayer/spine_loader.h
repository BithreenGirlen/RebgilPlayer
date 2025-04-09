﻿#ifndef SPINE_LOADER_H_
#define SPINE_LOADER_H_

#include <string>
#include <memory>

#include <spine/spine.h>

namespace spine_loader
{
	std::shared_ptr<spine::SkeletonData> ReadTextSkeletonFromFile(const spine::String& filename, spine::Atlas* atlas, float scale);
	std::shared_ptr<spine::SkeletonData> ReadBinarySkeletonFromFile(const spine::String& filename, spine::Atlas* atlas, float scale);

	std::shared_ptr<spine::SkeletonData> ReadTextSkeletonFromMemory(const std::string& skeleton, spine::Atlas* atlas, float scale);
	std::shared_ptr<spine::SkeletonData> ReadBinarySkeletonFromMemory(const std::string& skeleton, spine::Atlas* atlas, float scale);
}

#endif
