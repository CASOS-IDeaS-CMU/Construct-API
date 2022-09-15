#ifndef SUBSCRIPTION_HEADER_GUARD
#define SUBSCRIPTION_HEADER_GUARD
#include "pch.h"

struct CONSTRUCT_LIB Subscription : public Model
{
	Subscription(Construct* _construct) : Model(_construct, model_names::SUB) {};

	void think();
	void communicate(InteractionMessageQueue::iterator msg);
	void cleanup();

	//graph name - "public propensity"
	//agent x CommunicationMedium
	const Graph<float>& public_propensity = graph_manager->load_optional(graph_names::propensity, 
		0.01f, nodeset_names::agents, sparse, nodeset_names::comm, sparse);
	//graph name - "subscription network"
	//agent x agent
	//if row,column element is true row is subscribed to column
	Graph<bool>& subscriptions = graph_manager->load_optional(graph_names::subs, 
		false, nodeset_names::agents, sparse, nodeset_names::agents, sparse);
	//graph name - "subscription probability"
	//agent x agent
	const Graph<float>& sub_prob = graph_manager->load_optional(graph_names::sub_probability, 
		0.01f, nodeset_names::agents, sparse, nodeset_names::agents, sparse);

	InteractionMessageQueue public_queue;
};
#endif

