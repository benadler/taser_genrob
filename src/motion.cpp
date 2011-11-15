#include "motion.h"

Motion::Motion(void)
{
	logger = Logger::instance();
	logger->Motion("Motion::Motion()");

	config = Configuration::instance();
	finished = false;
}

Motion::~Motion(void)
{
}

bool Motion::isFinished(void) const
{
	return finished;
}

void Motion::setFinished(const bool finished)
{
	this->finished = finished;
}
