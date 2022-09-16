#ifndef BELIEFS_HEADER_GUARD
#define BELIEFS_HEADER_GUARD
#include "pch.h"

//This model is a variant on the Standard Interaction Model
#include "StandardInteraction.h"

class CONSTRUCT_LIB Beliefs : public StandardInteraction {
public:

	Beliefs(const dynet::ParameterMap& parameters, Construct* construct);

	//graph name - "belief network"
	//agent x belief
	Graph<float>& beliefs = graph_manager->load_required(graph_names::beliefs, nodeset_names::agents, nodeset_names::belief);

	//graph name - "belief knowledge weight network"
	//belief x knowledge
	const Graph<float>& belief_weights = graph_manager->load_optional(graph_names::b_k_wgt, 1.0f, nodeset_names::belief, dense, nodeset_names::knowledge, sparse);

	//graph name - "belief similarity weight network"
	//agent - timeperiod
	const Graph<float>& belief_sim_weight = graph_manager->load_optional(graph_names::b_sim_wgt, 1.0f, agents, sparse, time, sparse);

	void initialize(void);
	void cleanup(void);

	float get_additions(unsigned int agent_i, unsigned int agent_j);
	float get_belief_sim(unsigned int agent_i, unsigned int agent_j);

	float get_belief_value(unsigned int agent_i, unsigned int belief);
};

// BELIEFS_H_HH_CONSTRUCT_H
#endif


