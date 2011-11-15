#include <polynom.h>

Polynom::Polynom(void)
{
	a = b = c = d = 0.0;
}

Polynom::Polynom(const Polynom &other)
{
	a = other.a;
	b = other.b;
	c = other.c;
	d = other.d;
}

double Polynom::evaluate0(double x)
{
	return a + b*x + c*x*x + d*x*x*x;
}

double Polynom::evaluate1(double x)
{
	return b + 2*c*x + 3*d*x*x;
}

double Polynom::evaluate2(double x)
{
	return 2*c + 6*d*x;
}
