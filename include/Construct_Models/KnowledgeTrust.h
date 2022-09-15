#ifndef TRUST_HEADER_GUARD
#define TRUST_HEADER_GUARD
#include "pch.h"

class CONSTRUCT_LIB KnowledgeTrust : public Model
{
	const std::string relax_rate = "relax rate";
public:

	float rr;

	//graph name - "knowledge network"
	//agent x knowledge
	const Graph<bool>& knowledge_net = graph_manager->load_required(graph_names::knowledge, nodeset_names::agents, nodeset_names::knowledge);

	//graph name - "knowledge trust network"
	//agent x knowledge
	Graph<float>& knowledge_trust_net = graph_manager->load_optional(graph_names::k_trust, 0.5f, 
		nodeset_names::agents, knowledge_net.row_dense, nodeset_names::knowledge, knowledge_net.col_dense);

	//graph name - "knowledge trust transactive memory network"
	//agent x agent x knowledge
	Graph<std::map<unsigned int, float> >& kttm = graph_manager->load_optional(graph_names::kttm, std::map<unsigned int, float>(),
		nodeset_names::agents, dense, nodeset_names::agents, sparse, nodeset_names::knowledge);

	

	KnowledgeTrust(const dynet::ParameterMap& parameters, Construct* construct);

	void initialize(void);

	void update(void);

	void communicate(InteractionMessageQueue::iterator msg);

	void cleanup(void);
};
#endif



