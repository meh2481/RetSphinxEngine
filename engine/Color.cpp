#include "Color.h"
#include <sstream>
#include "StringUtils.h"

Color::Color()
{
	clear();
}

Color::Color(int ir, int ig, int ib)
{
	from256(ir, ig, ib);
}

Color::Color(float fr, float fg, float fb, float fa)
{ 
	set(fr, fg, fb, fa);
}

void Color::from256(int ir, int ig, int ib, int ia)
{
	r = (float)ir / 255.0f;
	g = (float)ig / 255.0f;
	b = (float)ib / 255.0f;
	a = (float)ia / 255.0f;
}

void Color::fromString(std::string s)
{
	s = StringUtils::stripCommas(s);

	//Now, parse
	std::istringstream iss(s);
	int r, g, b, a;
	if(iss >> r >> g >> b)
	{
		if(!(iss >> a))
			from256(r, g, b);
		else
			from256(r, g, b, a);
	}
}

void Color::set(float fr, float fg, float fb, float fa)
{ 
	r = fr; 
	g = fg; 
	b = fb; 
	a = fa; 
}