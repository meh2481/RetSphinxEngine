#ifndef DRAWABLE_H
#define DRAWABLE_H


class Drawable
{
public:
	Drawable() {depth=0;};
	~Drawable() {};
	
	float depth;
	
	virtual void draw() = 0;

};

#endif