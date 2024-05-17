#pragma once
#include "pch.h"

struct Subscription : public Model
{
	Subscription(Construct& construct) : Model(construct) {};

	void think() override;

	void communicate(const InteractionMessage& msg) override;

	void cleanup() override;

	//graph name - "public propensity network"
	//agent x CommunicationMedium
	const Graph<float>& public_propensity = graph_manager.load_optional(graph_names::propensity, 
		0.01f, nodeset_names::agents, sparse, nodeset_names::comm, sparse);
	//graph name - "subscription network"
	//agent x agent
	//if row,column element is true row is subscribed to column
	Graph<bool>& subscriptions = graph_manager.load_optional(graph_names::subs, 
		false, nodeset_names::agents, sparse, nodeset_names::agents, sparse);
	//graph name - "subscription probability network"
	//agent x agent
	const Graph<float>& sub_prob = graph_manager.load_optional(graph_names::sub_probability, 
		0.01f, nodeset_names::agents, sparse, nodeset_names::agents, sparse);

	std::list<InteractionMessage> public_queue;
};