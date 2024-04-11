#pragma once
#include "pch.h"

class KnowledgeLearningDifficulty : public Model
{
public:

	//sifts through interaction queue and removes items that are too difficult to learn
	//if a message loses all of items, it is removed from the queue
	//void update(void) override;

	/*<summary> 
	Returns true with probability from the receiver's corresponding element in k_learning_difficulty_net.
	<para>
	Only applies to items with receiver index less than the agent nodeset size, a non-null pointer medium, 
	and if the item contains InteractionItem::item_keys::knowledge.
	</para>
	<summary>*/
	bool intercept(InteractionItem& item, unsigned int sender, unsigned int receiver, const CommunicationMedium* medium) override;

	KnowledgeLearningDifficulty(Construct& construct) : Model(construct) {}
	//graph name - "knowledge learning difficulty network"
	//agent x knowledge
	const Graph<float>& k_leanrning_difficulty_net = graph_manager.load_required(graph_names::k_diff, nodeset_names::agents, nodeset_names::knowledge);

};