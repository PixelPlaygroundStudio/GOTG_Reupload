#include <wipch.h>
#include "OzzAnimation.h"

namespace Wiwa {
	void OzzAnimation::SetSkeleton(ozz::animation::Skeleton* skeleton)
	{
        skeleton_ = skeleton;

		OnSkeletonSet();
	}
}