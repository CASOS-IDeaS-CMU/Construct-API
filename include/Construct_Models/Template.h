#ifndef TEMPLATE_HEADER_GUARD
#define TEMPLATE_HEADER_GUARD
#include "pch.h"

class Template : public Model
{
public:

	Template(dynet::ParameterMap parameters, Construct* construct);
	void initialize(void);
	void think(void);
	void update(void);
	void communicate(const InteractionMessage& msg);
	void cleanup(void);
};
#endif



