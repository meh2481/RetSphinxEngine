#pragma once

#include "glmx.h"
#include <set>
#include "RenderState.h"
#include "ObjSegment.h"

//TODO: Use SceneryLayer rather than ObjSegment
class SceneryManager
{
	class DepthComparator
	{
	public:
		bool operator()(const ObjSegment* s1, const ObjSegment* s2)
		{
			return s1->depth < s2->depth;
		}
	};

	std::multiset<ObjSegment*, DepthComparator> m_lSceneryFg;
	std::multiset<ObjSegment*, DepthComparator> m_lSceneryBg;

public:
	~SceneryManager();

	void update(float dt);
	void renderForeground(const RenderState& renderState);
	void renderBackground(const RenderState& renderState);
	void add(ObjSegment* seg);
	void cleanup();
};









