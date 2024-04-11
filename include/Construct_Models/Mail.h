#pragma once
#include "pch.h"

class Mail : public Model
{
public:
	//graph name - "agent mail usage by medium"
	//agent x CommunicationMedium
	const Graph<float>& mail_pref = graph_manager.load_optional(graph_names::mail_usage, 1.0f, nodeset_names::agents, sparse, nodeset_names::comm, sparse);
	//graph name - "mail check probability"
	//agent x timeperiod
	const Graph<float>& mail_check_prob = graph_manager.load_optional(graph_names::mail_check_prob, 0.5f, nodeset_names::agents, sparse, nodeset_names::time, sparse);
	std::vector<std::list<InteractionMessage> > mailboxes = std::vector<std::list<InteractionMessage> >(mail_pref.source_nodeset->size());
	Mail(Construct& construct) : Model(construct) {};

	void update(void) override;
};