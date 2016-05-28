#pragma once

class Drawable
{
public:
	Drawable() {depth=0;};
	~Drawable() {};
	
	float depth;
	
	virtual void draw(bool bDebugInfo) = 0;

};
