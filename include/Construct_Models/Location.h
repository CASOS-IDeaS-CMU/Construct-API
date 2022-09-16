#ifndef LOCATION_HEADER_GUARD
#define LOCATION_HEADER_GUARD
#include "pch.h"
#include "Tasks.h"

class CONSTRUCT_LIB Location : public Model
{
public:

	const Nodeset* agents = ns_manager->get_nodeset(nodeset_names::agents);
	const Nodeset* knowledge = ns_manager->get_nodeset(nodeset_names::knowledge);
	const Nodeset* locations = ns_manager->get_nodeset(nodeset_names::loc);

	// graph name - "location network"
	// location x location 
	const Graph<bool>& loc_adj = graph_manager->load_optional(graph_names::location_network, true, locations, sparse, locations, sparse);

	// graph name - "agent current location network"
	// agent x location
	Graph<bool>& current_loc = graph_manager->load_required(graph_names::current_loc, agents, locations);

	// graph name - "location knowledge network"
	// location x knowledge
	const Graph<bool>& location_knowledge_net = graph_manager->load_optional(graph_names::loc_knowledge, true, locations, sparse, knowledge, sparse);

	// graph name - "agent location learning rate network"
	// agent x location
	const Graph<float>& agent_location_learning_rate_net = graph_manager->load_optional(graph_names::loc_learning_rate, 1.0f, agents, sparse, locations, sparse);

	// graph name - "knowledge expertise weight network"
	// agent x timperiod
	const Graph<float>& knowledge_expertise = graph_manager->load_optional(graph_names::k_exp_wgt, 1.0f, agents, sparse, ns_manager->get_nodeset(nodeset_names::time), sparse);

	// graph name - "location preference network"
	// agent x location
	const Graph<float>& location_preference = graph_manager->load_optional(graph_names::loc_preference, 1.0f, agents, sparse, locations, sparse);

	// graph name - "location medium access network"
	// location x CommunicationMedium
	const Graph<bool>& location_medium_access = graph_manager->load_optional(graph_names::loc_medium_access, true, locations, sparse, ns_manager->get_nodeset(nodeset_names::comm), sparse);

	// graph name - "knowledge network"
	// agent x knowledge
	const Graph<bool>& knowledge_net = graph_manager->load_required(graph_names::knowledge, agents, knowledge);

	//graph name - "interaction knowledge weight network"
	//agent x knowledge
	const Graph<float>& knowledge_importance = graph_manager->load_optional(graph_names::interact_k_wgt, 1.0f, agents, sparse, knowledge, sparse);

	//graph name - "agent location learning limit network"
	//agent x location
	const Graph<unsigned int>& loc_limit = graph_manager->load_optional(graph_names::loc_learning_limit, 1u, agents, sparse, locations, sparse);

	//graph name - "agent active timperiod network"
	//agent x timeperiod
	const Graph<bool>& active = graph_manager->load_optional(graph_names::active, true, agents, sparse, ns_manager->get_nodeset(nodeset_names::time), sparse);

	//graph name - "knowledge priority network"
	//agent x knowledge
	const Graph<float>& knowledge_priority_network = graph_manager->load_optional(graph_names::k_priority, 1.0f, agents, sparse, knowledge, sparse);

	//graph name - "communication medium access network"
	//agent x CommunicationMedium
	const Graph<bool>& comm_access = graph_manager->load_optional(graph_names::comm_access, true, agents, sparse, ns_manager->get_nodeset(nodeset_names::comm), sparse);

	//graph name - "communication medium preferences network"
	//agent x CommunicationMedium
	const Graph<float>& comm_medium_prefs = graph_manager->load_optional(graph_names::comm_pref, 1.0f, agents, sparse, ns_manager->get_nodeset(nodeset_names::comm), sparse);

	//unsigned int agent_count;
	//unsigned int location_count;
	//unsigned int knowledge_count;
	std::vector<unsigned int> _current_agent_locations;
	std::vector<CommunicationMedium> communication_mediums;

	Tasks* tasks = 0;

	Location(Construct* construct);
	void initialize(void);
	void think(void);
	void cleanup(void);

	virtual float get_expertise(unsigned int agent, unsigned int location);
	virtual const CommunicationMedium* get_medium(unsigned int agent);
};
#endif


