#pragma once
#include "Construct.h"

class Template : public Model
{
public:

	Template(dynet::ParameterMap parameters, Construct& construct);
	void initialize(void) override;
	void think(void) override;
	void update(void) override;
	void communicate(const InteractionMessage& msg) override;
	void cleanup(void) override;
};