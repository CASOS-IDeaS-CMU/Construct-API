#pragma once
#include "Construct.h"

namespace dynet {
	inline float biased_sigmoid(float bias, float arg) noexcept {
		return 1 / (1 + (1 / bias - 1) * exp(-1 * arg));
	}
}

struct Emotions : public Model {
	Emotions(const dynet::ParameterMap& params, Construct& construct);
	
	~Emotions(void) {
		for (auto& node : emotions)
			InteractionItem::item_names.erase((item_keys)(emot_itemkey_block + node.index));
	}

	Graph<float>& get_emotional_response_graph(const dynet::ParameterMap& params);
	Graph<float>& get_emotional_attractor_graph();
	Graph<float>& get_emotional_decay_graph();
	Graph<float>& get_emotional_graph();

	// defines the start of indexes reserved for emotion item_keys
	static constexpr char emot_itemkey_block = 100;
	const Nodeset& agents = ns_manager.get_nodeset(nodeset_names::agents);
	const Nodeset& emotions = ns_manager.get_nodeset(nodeset_names::emotions);
	
	//functions that update their respective activity rates based on the submitted agent index.
	std::map<const Node*, std::function<float(unsigned int)> > emotional_activity_dependence;

	//graph name: "agent activity rates network"
	//agent x activity
	//becomes set to a Graph only if the activity nodeset is present
	//each element dictates how active agents should be at various activities
	Graph<float>* activity_rates = nullptr;

	//graph name: "emotion regulation network"
	//agent x emotion
	//how quickly an agent's emotions return to their respective attractors during the clean_up function
	const Graph<float>& emotion_regulation = get_emotional_decay_graph();
		//graph_manager.load_optional(graph_names::emot_reg, 0.1f, agents, sparse, emotions, sparse);

	//graph name: "emotion attractors network
	//agent x emotion
	//the emotional valence regulation stable point
	const Graph<float>& emotion_attractors = get_emotional_attractor_graph();
		//graph_manager.load_optional(graph_names::emotion_att, 0.0f, agents, sparse, emotions, sparse);

	//graph name: "emotion network"
	//agent x emotion
	//contains each agents current emotional state
	Graph<float>& emotion_net = get_emotional_graph();

	//graph name: "emotion reading first order network"
	//emotion x emotion
	//the first order dependence for the emotional deflection of reading a message with emotions
	//the source dimension corresponds to the emotion being deflected
	//the target dimension corresponds to the emotions attached in a read message
	const Graph<float>& emot_reading_first;

	//graph name: "emotion broadcast bias network"
	//agent x emotion
	//the base probability that an agent attaches an emotion independent of their currrent emotional state
	const Graph<float>& base_broadcast_prob = graph_manager.load_optional(graph_names::base_broad_prob, 0.1f, agents, sparse, emotions, sparse);

	//graph name: "emotion broadcast first order network"
	//emotion x emotion
	//the first order dependence for the probability that an agent attaches an emotion
	//the source dimension correspondes to the emotion being attached 
	//the target dimension corresponds to the emotional state of an agent
	const Graph<float>& emot_broadcast_first = graph_manager.load_optional(graph_names::emot_broad_first, 1.0f, emotions, sparse, emotions, sparse);

	//graph name: "emotion broadcast second order network"
	//emotion x emotion x emotion
	//the second order dependence for the probability that an agent attaches an emotion
	//the source dimension corresponds to the emotion being attached
	//the target and slice dimension corresponds to combinations of emotional states of an agent
	const Graph<std::map<unsigned int, float> >& emot_broadcast_second = 
		graph_manager.load_optional(graph_names::emot_broad_second, std::map<unsigned int, float>(), emotions, sparse, emotions, sparse, emotions);



	//graph name: "emotion reading second order network"
	//emotion x emotion x emotion
	//the second order dependence for the emotional deflection of reading a message with emotions
	//the source dimension corresponds to the emotion being deflected
	//the target and slice dimension correspond to combinations of emotional states of an agent
	const Graph<std::map<unsigned int, float> >& emot_reading_second =
		graph_manager.load_optional(graph_names::emot_read_second, std::map<unsigned int, float>(), emotions, sparse, emotions, sparse, emotions);



	//emotions are selected probabilistically to be attached to a message
	//map pairs corresponds to an emotional index and that emotion's value for the agent_index
	//probability calculated using the emotion broadcast networks
	virtual std::map<unsigned int, float> get_emotions(unsigned int agent_index);

	// takes the result from Emotions::get_emotions and adds them to an InteractionItem::values object.
	// The value of the item_keys key is equal to the map entry index plus Emotions::emot_itemkey_block
	virtual void attach_emotions(unsigned int sender_index, InteractionItem* item);

	// reverses the operation of Emotions::attach_emotions and converts an InteractionItem::values object to a map
	virtual std::map<unsigned int, float> parse_emotions(const InteractionItem& values);

	//allows the agent to parse the emotions attached in a message
	//the agent's emotions are updated using the deflection from the emotion reading networks
	virtual void read_emotions(std::map<unsigned int, float>&& attached_emotions, unsigned int agent_index);

	// adds the names of the emotion nodeset nodes into InteractionItem::item_names so that output routines know what to call an item
	// size of emotion nodeset is limited to 100 to save space for other item_keys and prevent overflow when converting
	void update_item_names(void) {
		
		//emotion nodeset can't exceed the alloacted limit to ensure no overflow on emot_itemkey_block
		assert(emot_itemkey_block + emotions.size() <= 255);
		for (auto& node : emotions) {
			auto insertion = InteractionItem::item_names.insert(std::pair((item_keys)(emot_itemkey_block + node.index), node.name));
			//no item names with this index should be present in item_names
			assert(insertion.second);
		}
	}

	//attaches emotions to a message if no emotions are attached
	//void update(void) override;

	bool intercept(InteractionItem& item, unsigned int sender, unsigned int receiver, const CommunicationMedium* medium) override;

	//each agent self-regulates their emotions using the emotion regulation networks
	void cleanup(void) override;
};

struct emotion_parser : public InteractionMessageParser {
	Emotions& emot;

	emotion_parser(Emotions& emot) : InteractionMessageParser(nodeset_names::emotions), emot(emot) {}

	void parse(const InteractionMessage& msg) override;
};
