#include "Color.h"
#include <sstream>
#include "Parse.h"

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
	r = (float)ir / 255.0;
	g = (float)ig / 255.0;
	b = (float)ib / 255.0;
	a = (float)ia / 255.0;
}

void Color::fromString(string s)
{
	s = stripCommas(s);

	//Now, parse
	istringstream iss(s);
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