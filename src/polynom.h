#ifndef POLYNOM_H
#define POLYNOM_H

class Polynom
{
	public:
		double a,b,c,d;

		Polynom(void);
		Polynom(const Polynom &other);

		double evaluate0(double x);
		double evaluate1(double x);
		double evaluate2(double x);
};

#endif
