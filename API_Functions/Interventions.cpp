#include "pch.h"
#include "Interventions.h"

Intervention1_nf::Intervention1_nf(Social_Media_no_followers* _media, Nodeset::iterator _node) : default_media_user(_media, _node) {
	const Nodeset* knowledge_ns = media->ns_manager->get_nodeset(nodeset_names::knowledge);
	const Nodeset* time_ns = media->ns_manager->get_nodeset(nodeset_names::time);
	
	
	const Node* k_node = knowledge_ns->get_node_by_name(_node->get_attribute("knowledge"));
	assert(k_node);
	knowledge = k_node->index;
	const Node* t_node = time_ns->get_node_by_name(_node->get_attribute("time"));
	assert(t_node);
	time = t_node->index;
}

unsigned int Intervention1_nf::get_knowledge_selection(void) {
	std::vector<unsigned int> known_knowledge;
	for (auto k = media->knowledge_net.sparse_row_begin(id, 0); k != media->knowledge_net.row_end(id); ++k) {
		// if we've passed the specified time, we want to skip the specified knowledge as a possible selection.
		if (k.col() == knowledge && media->time >= time) 
			continue;
		known_knowledge.push_back(k.col());
	}
	return known_knowledge[media->random->integer((unsigned int)known_knowledge.size())];
}

Intervention2_nf::Intervention2_nf(Social_Media_no_followers* _media, Nodeset::iterator _node) : default_media_user(_media, _node) {
	const Nodeset* knowledge_ns = media->ns_manager->get_nodeset(nodeset_names::knowledge);
	const Nodeset* time_ns = media->ns_manager->get_nodeset(nodeset_names::time);


	const Node* k_node = knowledge_ns->get_node_by_name(_node->get_attribute("knowledge"));
	assert(k_node);
	knowledge = k_node->index;
	const Node* t_node = time_ns->get_node_by_name(_node->get_attribute("time"));
	assert(t_node);
	time = t_node->index;
}

void Intervention2_nf::generate_post_events(void) {
	default_media_user::generate_post_events();
	// if we're at the specified time, create one extra post with a trust of zero for the specified knowledge
	if (media->time == time) {
		media->list_of_events.emplace_front(id, knowledge, 0.0f, media->time * media->dt);
	}
}

Intervention3_nf::Intervention3_nf(Social_Media_no_followers* _media, Nodeset::iterator _node) : default_media_user(_media, _node) {
	const Nodeset* knowledge_ns = media->ns_manager->get_nodeset(nodeset_names::knowledge);
	const Nodeset* time_ns = media->ns_manager->get_nodeset(nodeset_names::time);


	const Node* k_node = knowledge_ns->get_node_by_name(_node->get_attribute("knowledge"));
	assert(k_node);
	knowledge = k_node->index;
	const Node* t_node = time_ns->get_node_by_name(_node->get_attribute("time"));
	assert(t_node);
	time = t_node->index;
}

float Intervention3_nf::get_trust(unsigned int knowledge_index) {
	if (!media->TRUST) return -1;

	// after the specified time, the user always returns trust 0 for the specified knowledge
	if (time >= media->time && knowledge_index == knowledge) 
		return 0;

	return default_media_user::get_trust(knowledge_index);
}