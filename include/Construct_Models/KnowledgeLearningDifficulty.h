#ifndef KNOWLEDGE_LEARNING_DIFFICULTY_HEADER_GUARD
#define KNOWLEDGE_LEARNING_DIFFICULTY_HEADER_GUARD
#include "pch.h"

class CONSTRUCT_LIB KnowledgeLearningDifficulty : public Model
{
public:

	//sifts through interaction queue and removes items that are too difficult to learn
	//if a message loses all of items, it is removed from the queue
	void update(void);

	KnowledgeLearningDifficulty(Construct* _construct) : Model(_construct, model_names::KDIFF) {}

	//graph name - "knowledge learning difficulty network"
	//agent x knowledge
	const Graph<float>& k_leanrning_difficulty_net = graph_manager->load_required(graph_names::k_diff, nodeset_names::agents, nodeset_names::knowledge);

};

// KNOWLEDGE_LEARNING_DIFFICULTY_HH_HH_H
#endif


