#ifndef KNOWLEDGE_PARSING_HEADER_GUARD
#define KNOWLEDGE_PARSING_HEADER_GUARD
#include "pch.h"

struct CONSTRUCT_LIB KnowledgeParsing : public Model
{
public:

	//graph name - "knowledge network"
	//agent x knowledge
	Graph<bool>& knowledge_net = graph_manager->load_required(graph_names::knowledge, nodeset_names::agents, nodeset_names::knowledge);

	KnowledgeParsing(Construct* _construct) : Model(_construct, model_names::KPARSE) {}

	void communicate(InteractionMessageQueue::iterator msg);
};
#endif



