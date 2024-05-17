#pragma once
#include "pch.h"

//This model is a variant on the Standard Interaction Model
#include "StandardInteraction.h"

class Beliefs : public StandardInteraction {
public:
	Beliefs(const dynet::ParameterMap& parameters, Construct& construct) : StandardInteraction(parameters, construct) {}
	//graph name - "belief network"
	//agent x belief
	Graph<float>& beliefs = graph_manager.load_required(graph_names::beliefs, nodeset_names::agents, nodeset_names::belief);
	//graph name - "belief knowledge weight network"
	//belief x knowledge
	const Graph<float>& belief_weights = graph_manager.load_optional(graph_names::b_k_wgt, 1.0f, nodeset_names::belief, dense, nodeset_names::knowledge, sparse);
	//graph name - "belief similarity weight network"
	//agent - timeperiod
	const Graph<float>& belief_sim_weight = graph_manager.load_optional(graph_names::b_sim_wgt, 1.0f, agents, sparse, time, sparse);

	// checks for mutually exlusive models
	void initialize(void) override;

	// updates everyone's beliefs
	void cleanup(void) override;

	// get_belief_sim * agent_i's belief_sim_weight
	float get_additions(unsigned int agent_i, unsigned int agent_j) override;

	// gets belief cosine similarity between two agents
	virtual float get_belief_sim(unsigned int agent_i, unsigned int agent_j);

	// agent_i's knowledge_net row * the belief index's belief_weight row
	virtual float get_belief_value(unsigned int agent_i, unsigned int belief);
};