#ifndef MAILING_HEADER_GUARD
#define MAILING_HEADER_GUARD
#include "pch.h"

class CONSTRUCT_LIB Mail : public Model
{
public:

	//graph name - "agent mail usage by medium"
	//agent x CommunicationMedium
	Graph<float>& mail_pref = graph_manager->load_optional(graph_names::mail_usage, 1.0f, nodeset_names::agents, sparse, nodeset_names::comm, sparse);

	//graph name - "mail check probability"
	//agent x timeperiod
	Graph<float>& mail_check_prob = graph_manager->load_optional(graph_names::mail_check_prob, 0.5f, nodeset_names::agents, sparse, nodeset_names::time, sparse);

	std::vector<InteractionMessageQueue> mailboxes = std::vector<InteractionMessageQueue>(mail_pref.source_nodeset->size());

	Mail(Construct* _construct) : Model(_construct, model_names::MAIL) {};
	void update(void);
};
#endif



