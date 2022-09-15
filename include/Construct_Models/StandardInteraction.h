#ifndef STANDARD_INTERACTION_HEADER_GUARD
#define STANDARD_INTERACTION_HEADER_GUARD
#include "pch.h"

class CONSTRUCT_LIB StandardInteraction : public virtual Model
{
public:

	StandardInteraction(const dynet::ParameterMap& parameters, Construct* construct);

	//adds the knowledge parsing model to the models list
	virtual void initialize(void);

	//sets interaction probabilities, creates messages, and adds them to construct's interaction queue
	virtual void think(void);

	//creates all the communication mediums based on the medium nodeset
	void initialize_communication_mediums();

	//sets all interaction probability network links
	virtual void set_probabilities(void);

	//forms interaction pairs based on the interaction probability network, mediums, and available knowledge.
	virtual void add_messages(void);


	//sets the corresponding element where agent_i corresponds to source index
	virtual void set_interaction_probability(unsigned int agent_i, unsigned int agent_j);

	// Grabs the send_msg_queue and decrements all of their time_to_live values
	// any msg that has gotten to zero gets added to the private message queue
	// this should be called before populating the send_msg_queue during interactions
	virtual void update_send_msg_queue();
	
	// generates a vector of paired values with first being the agent index
	// and second being the associated initiation/reception count
	virtual void initialize_interaction_lists(std::vector<std::pair<unsigned int, unsigned int> > &initiators, std::vector<std::pair<unsigned int,unsigned int> > &receivers);

	//gets the index in the initiators and receivers list of the interaction partners found
	std::pair<unsigned int, unsigned int> get_interaction_pair_index(std::vector<std::pair<unsigned int, unsigned int> >& initiators, std::vector<std::pair<unsigned int, unsigned int> >& receivers);

	// gets the communication medium based on the shared medium between sender and receiver
	const CommunicationMedium* get_comm_medium(unsigned int sender_index, unsigned int receiver_index);

	// uses the proximity networks and proximity weight networks to calculate a proximity
	// proximity is based on the perspective of the sender
	virtual float get_prox(unsigned int sender_index, unsigned int receiver_index);

	// compares the sender and receiver for how similar their knowledge is
	virtual float get_k_sim(unsigned int sender_index, unsigned int receiver_index);

	// finds how much knowledge the sender can send to the receiver
	virtual float get_k_exp(unsigned int sender_index, unsigned int receiver_index);

	// returns zero
	virtual float get_additions(unsigned int sender_index, unsigned int receiver_index) { return 0; }

	//given a sender, receiver, and medium, returns the vector of InteractionItems to be attached to an InteractionMessage
	virtual std::vector<InteractionItem> get_interaction_items(unsigned int sender, unsigned int receiver, const CommunicationMedium* comm);

	const Nodeset* agents = ns_manager->get_nodeset(nodeset_names::agents);
	const Nodeset* knowledge = ns_manager->get_nodeset(nodeset_names::knowledge);
	const Nodeset* comm = ns_manager->get_nodeset(nodeset_names::comm);
	const Nodeset* time = ns_manager->get_nodeset(nodeset_names::time);

	//The only required network

	//graph name - "knowledge network"
	//agent x knowledge
	Graph<bool>& knowledge_net = graph_manager->load_required(graph_names::knowledge, agents, knowledge);


	//These graphs are used to control which agents can interact with which other agents.

	//graph name - "sphere of influence network"
	//agent x agent
	Graph<bool>& soi = graph_manager->load_optional(graph_names::soi, true, agents, sparse, agents, sparse);
	//graph name - "agent active timeperiod network"
	//agent x timeperiod
	const Graph<bool>& agent_active = graph_manager->load_optional(graph_names::active, true, agents, sparse, time, sparse);
	//graph name - "agent initiation count network"
	//agent x timeperiod
	const Graph<unsigned int>& agent_initiation_count = graph_manager->load_optional(graph_names::init_count, 1u, agents, sparse, time, sparse);
	//graph name - "agent reception count network"
	//agent x timeperiod
	const Graph<unsigned int>& agent_reception_count = graph_manager->load_optional(graph_names::recep_count, 1u, agents, sparse, time, sparse);


	//these graphs are for internal use, not expected as input and are only used for output
	//if they are inputted, the networks get cleared at the beginning of each time step


	//graph name - "interaction probability network"
	//agent x agent
	Graph<float>& interaction_prob = graph_manager->load_optional(graph_names::interact_prob, 0.0f, agents, dense, agents, dense);
	//graph name - "interaction network"
	//agent x agent
	Graph<bool>& interactions = graph_manager->load_optional(graph_names::interact, false, agents, sparse, agents, sparse);

	//These graphs are used to determine how messages are constructed given an interaction pair



	//graph name - "knowledge message complexity network"
	//agent x timeperiod
	const Graph<unsigned int>& knowledge_message_complexity = graph_manager->load_optional(graph_names::k_msg_complex, 1u, agents, sparse, time, sparse);
	//graph name - "knowledge priority network"
	//agent x knowledge
	const Graph<float>& knowledge_priority_network = graph_manager->load_optional(graph_names::k_priority, 1.0f, agents, sparse, knowledge, sparse);
	//graph name - "learnable knowledge network"
	//agent x knowledge
	const Graph<bool>& learnable_knowledge_net = graph_manager->load_optional(graph_names::learnable_k, true, agents, sparse, knowledge, sparse);
	//graph name - "communication medium preferences network"
	//agent x medium
	const Graph<float>& comm_medium_pref = graph_manager->load_optional(graph_names::comm_pref, 1.0f, agents, sparse, comm, sparse);
	//graph name - "medium knowledge network"
	//medium x knowledge
	const Graph<bool>& medium_knowledge_access = graph_manager->load_optional(graph_names::medium_k_access, true, comm, sparse, knowledge, sparse);
	//graph name - "communication medium access network"
	//agent x CommunicationMedium
	const Graph<bool>& comm_access = graph_manager->load_optional(graph_names::comm_access, true, agents, sparse, comm, sparse);

	//These graphs determine proximity which is assumed to be static.



	//graph name - "physical proximity network"
	//agent x agent
	const Graph<float>& physical_prox = graph_manager->load_optional(graph_names::phys_prox, 1.0f, agents, sparse, agents, sparse);
	//graph name - "physical proximity weight network"
	//agent x timeperiod
	const Graph<float>& physical_prox_weight = graph_manager->load_optional(graph_names::phys_prox_wgt, 1.0f, agents, sparse, time, sparse);
	//graph name - "social proximity network"
	//agent x agent
	const Graph<float>& social_prox = graph_manager->load_optional(graph_names::soc_prox, 1.0f, agents, sparse, agents, sparse);
	//graph name - "social proximity weight network"
	//agent x timeperiod
	const Graph<float>& social_prox_weight = graph_manager->load_optional(graph_names::soc_prox_wgt, 1.0f, agents, sparse, time, sparse);
	//graph name - "sociodemographic proximity network"
	//agent x agent
	const Graph<float>& socdem_prox = graph_manager->load_optional(graph_names::dem_prox, 1.0f, agents, sparse, agents, sparse);
	//graph name - "sociodemographic proximity weight network"
	//agent x timeperiod
	const Graph<float>& socdem_prox_weight = graph_manager->load_optional(graph_names::dem_prox_wgt, 1.0f, agents, sparse, time, sparse);

	//These graphs indicate how important various pieces are for determining interaction probability



	//graph name - "interaction knowledge weight network"
	//agent x knowledge
	const Graph<float>& knowledge_importance = graph_manager->load_optional(graph_names::interact_k_wgt, 1.0f, agents, sparse, knowledge, sparse);
	//graph name - "knowledge similarity weight network"
	//agent x timeperiod
	const Graph<float>& knowledge_sim_weight = graph_manager->load_optional(graph_names::k_sim_wgt, 1.0f, agents, sparse, time, sparse);
	//graph name - "knowledge expertise weight network"
	//agent x timeperiod
	const Graph<float>& knowledge_exp_weight = graph_manager->load_optional(graph_names::k_exp_wgt, 1.0f, agents, sparse, time, sparse);


	//delayed messages waiting to be sent are here
	InteractionMessageQueue send_msg_queue; 

	
	//The communication mediums derived from the medium nodeset
	std::vector<CommunicationMedium> communication_mediums;
	//size of the agent nodeset
	unsigned int agent_count;
	//size of the knowledge nodeset
	unsigned int knowledge_count;
	//number of times a try for an interaction pair can happen before interaction pairing prematurely ends
	unsigned int interaction_rejection_limit;

	const unsigned int& t;

	//used to prevent multiple instances of rejection limit being printed
	static bool needs_printing;
};
#endif

