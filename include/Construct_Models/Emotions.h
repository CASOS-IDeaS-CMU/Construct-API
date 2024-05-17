#pragma once
#include "pch.h"

struct Emotions : public Model
{
	Emotions(Construct& construct);
	
	~Emotions(void) {
		for (auto& node : emotions)
			InteractionItem::item_names.erase((InteractionItem::item_keys)(emot_itemkey_block + node.index));
	}

	// defines the start of indexes reserved for emotion InteractionItem::item_keys
	static constexpr char emot_itemkey_block = 100;
	const Nodeset& agents = ns_manager.get_nodeset(nodeset_names::agents);
	const Nodeset& emotions = ns_manager.get_nodeset(nodeset_names::emotions);

	//graph name: "emotion network"
	//agent x emotion
	//contains each agents current emotional state
	Graph<float>& emotion_net = graph_manager.load_required(graph_names::emotion_net, agents, emotions);

	//graph name: "emotion broadcast bias network"
	//agent x emotion
	//the base probability that an agent attaches an emotion independent of their currrent emotional state
	const Graph<float>& emot_broadcast_bias = graph_manager.load_optional(graph_names::emot_broad_bias, 1.0f, agents, false, emotions, false);

	//graph name: "emotion broadcast first order network"
	//emotion x emotion
	//the first order dependence for the probability that an agent attaches an emotion
	//the source dimension correspondes to the emotion being attached 
	//the target dimension corresponds to the emotional state of an agent
	const Graph<float>& emot_broadcast_first = graph_manager.load_optional(graph_names::emot_broad_first, 0.0f, emotions, false, emotions, false);

	//graph name: "emotion broadcast second order network"
	//emotion x emotion x emotion
	//the second order dependence for the probability that an agent attaches an emotion
	//the source dimension corresponds to the emotion being attached
	//the target and slice dimension corresponds to combinations of emotional states of an agent
	const Graph<std::map<unsigned int, float> >& emot_broadcast_second = 
		graph_manager.load_optional(graph_names::emot_broad_second, std::map<unsigned int, float>(), emotions, false, emotions, false, emotions);

	//graph name: "emotion reading first order network"
	//emotion x emotion
	//the first order dependence for the emotional deflection of reading a message with emotions
	//the source dimension corresponds to the emotion being deflected
	//the target dimension corresponds to the emotions attached in a read message
	const Graph<float>& emot_reading_first = graph_manager.load_required(graph_names::emot_read_first, emotions, emotions);

	//graph name: "emotion reading second order network"
	//emotion x emotion x emotion
	//the second order dependence for the emotional deflection of reading a message with emotions
	//the source dimension corresponds to the emotion being deflected
	//the target and slice dimension correspond to combinations of emotional states of an agent
	const Graph<std::map<unsigned int, float> >& emot_reading_second =
		graph_manager.load_optional(graph_names::emot_read_second, std::map<unsigned int, float>(), emotions, false, emotions, false, emotions);

	//graph name: "emotion regulation bias network"
	//agent x emotion
	//the base value that an agent's emotions will tend toward independent of the agent's current emotional state
	const Graph<float>& emot_regulation_bias = graph_manager.load_required(graph_names::emot_reg_bias, agents, emotions);

	//graph name: "emotion regulation first order network"
	//emotion x emotion
	//the first order dependence for the self-regulation of an emotion
	//the source dimension corresponds to the emotion being regulated
	//the target dimension corresponds to the emotional state of an agent
	const Graph<float>& emot_regulation_first = graph_manager.load_required(graph_names::emot_reg_first, emotions, emotions);

	//graph name: "emotion regulation second order network"
	//emotion x emotion x emotion
	//the second order dependence for the self-regulation of an emotion
	//the source dimension corresponds to the emotion being regulated
	//the target and slice dimension corresponds to combinations of emotional states of an agent
	const Graph<std::map<unsigned int, float> >& emot_regulation_second =
		graph_manager.load_optional(graph_names::emot_reg_second, std::map<unsigned int, float>(), emotions, false, emotions, false, emotions);

	//emotions are selected probabilistically to be attached to a message
	//map pairs corresponds to an emotional index and that emotion's value for the agent_index
	//probability calculated using the emotion broadcast networks
	virtual std::map<unsigned int, float> get_emotions(unsigned int agent_index);

	// takes the result from Emotions::get_emotions and adds them to an InteractionItem::values object.
	// The value of the InteractionItem::item_keys key is equal to the map entry index plus Emotions::emot_itemkey_block
	virtual void attach_emotions(unsigned int sender_index, InteractionItem* item);

	// reverses the operation of Emotions::attach_emotions and converts an InteractionItem::values object to a map
	virtual std::map<unsigned int, float> parse_emotions(const InteractionItem& values);

	//allows the agent to parse the emotions attached in a message
	//the agent's emotions are updated using the deflection from the emotion reading networks
	virtual void read_emotions(const std::map<unsigned int, float>& attached_emotions, unsigned int agent_index);

	// adds the names of the emotion nodeset nodes into InteractionItem::item_names so that output routines know what to call an item
	// size of emotion nodeset is limited to 100 to save space for other InteractionItem::item_keys and prevent overflow when converting
	void update_item_names(void) {
		
		//emotion nodeset can't exceed 100 to ensure no overflow on emot_itemkey_block
		assert(emot_itemkey_block + emotions.size() <= 200);
		for (auto& node : emotions) {
			auto insertion = InteractionItem::item_names.insert(std::pair((InteractionItem::item_keys)(emot_itemkey_block + node.index), node.name));
			//no item names with this index should be present in item_names
			assert(insertion.second);
		}
	}

	//attaches emotions to a message if no emotions are attached
	//void update(void) override;

	bool intercept(InteractionItem& item, unsigned int sender, unsigned int receiver, const CommunicationMedium* medium) override;

	//parses the message for emotions and updates the receiver's emotions
	void communicate(const InteractionMessage& msg) override;

	//each agent self-regulates their emotions using the emotion regulation networks
	void cleanup(void) override;
};

#include "SocialMedia.h"
struct SM_nf_emotions : virtual public Social_Media_no_followers {
	SM_nf_emotions(const std::string& _media_name, InteractionItem::item_keys event_key, const dynet::ParameterMap& params, Construct& construct) : 
		Social_Media_no_followers(_media_name, event_key, params, construct) {}
	const Nodeset* emotions = ns_manager.get_nodeset(nodeset_names::emotions);

	// implements a user whose actions are dependent on their emotional state
	struct default_media_user : virtual public Social_Media_no_followers::default_media_user {
		default_media_user(Social_Media_no_followers* media, const Node& node) :
			Social_Media_no_followers::default_media_user(media, node) {}

		SM_nf_emotions& media() {
			SM_nf_emotions* temp = dynamic_cast<SM_nf_emotions*>(media_ptr);
			// if the media couldn't be up casted the desired class this assertion will be raised.
			// If you're confused why you probably have a diamond inheritence that makes casting non-trivial
			assert(temp);
			return *temp;
		}

		void enrich_event(media_event* _event) override;

		void reply(media_event* _event) override;

		void quote(media_event* _event) override;

		void repost(media_event* _event) override;

		void generate_post_events(void) override;

		unsigned int get_read_count(void) override;

		//virtual unsigned int get_knowledge_selection(void);
	};

	Social_Media_no_followers::media_user* get_default_media_user(const Node& node) override { return new default_media_user(this, node); }

	//Social_Media_no_followers::media_user& user(unsigned int index) {
	//	Social_Media_no_followers::media_user* temp = dynamic_cast<Social_Media_no_followers::media_user*>(users[index]);
	//	// if the media couldn't be up casted the desired class this assertion will be raised.
	//	// If you're confused why you probably have a diamond inheritence that makes casting non-trivial
	//	assert(temp);
	//	return *temp;
	//}
	// reimplements Social_Media_no_followers to include an Emotions::attach_emotions call on the returned item
	InteractionItem convert_to_InteractionItem(media_event* _event, unsigned int sender_index, unsigned int receiver_index) const override;
	//graph name: "emotion network"
	//agent x emotion
	//holds the emotional state of each agent
	const Graph<float>& emotion_net = graph_manager.load_required(graph_names::emotion_net, agents, emotions);

	//graph name: "emotion broadcast bias network"
	//agent x emotion
	//the base probability that an agent attaches an emotion independent of their currrent emotional state
	const Graph<float>& emot_broadcast_bias = graph_manager.load_optional(graph_names::emot_broad_bias, 1.0f, agents, false, emotions, false);

	//graph name: "first order emotion broadcast network"
	//emotion x emotion
	//the first order dependence for the probability that an agent attaches an emotion
	//the source dimension correspondes to the emotion being attached 
	//the target dimension corresponds to the emotional state of an agent
	const Graph<float>& emot_broadcast_first = graph_manager.load_optional(graph_names::emot_broad_first, 0.0f, emotions, false, emotions, false);

	//graph name: "second order emotion broadcast network"
	//emotion x emotion x emotion
	//the second order dependence for the probability that an agent attaches an emotion
	//the source dimension corresponds to the emotion being attached
	//the target and slice dimension corresponds to combinations of emotional states of an agent
	const Graph<std::map<unsigned int, float> >& emot_broadcast_second =
		graph_manager.load_optional(graph_names::emot_broad_second, std::map<unsigned int, float>(), emotions, false, emotions, false, emotions);
	//graph name: "first order post density emotion network"
	//agent x emotion
	//the first order emotional dependence for the probability density for how many posts an agent will make
	//the source dimension corresponds to the agent making the posts
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& pd_emotion_first = graph_manager.load_optional("first order post density emotion network", 0.0f, agents, false, emotions, false);

	//graph name: "second order post density emotion network"
	//emotion x emotion
	//the second order emotional dependence for the probability density for how many posts an agent will make
	//the source and target dimensions corresponds to the combinations of emotional states of the agent
	const Graph<float>& pd_emotion_second = graph_manager.load_optional("second order post density emotion network", 0.0f, emotions, false, emotions, false);

	//graph name: "first order reply probability emotion network"
	//agent x emotion
	//the first order emotional dependence for the probability an agent will reply to an event when reading it
	//the source dimension corresponds to the agent reading the event
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& pr_emotion_first = graph_manager.load_optional("first order reply probability emotion network", 0.0f, agents, false, emotions, false);

	//graph name: "second order reply probability emotion network"
	//emotion x emotion
	//the second order emotional dependence for the probability an agent will reply to an event when reading it
	//the source and target dimensions corresponds to the combinations of emotional states of the agent
	const Graph<float>& pr_emotion_second = graph_manager.load_optional("second order reply probability emotion network", 0.0f, emotions, false, emotions, false);

	//graph name: "first order repost probability emotion network"
	//agent x emotion
	//the first order emotional dependence for the probability an agent will repost an event when reading it
	//the source dimension corresponds to the agent reading the event
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& prp_emotion_first = graph_manager.load_optional("first order repost probability emotion network", 0.0f, agents, false, emotions, false);

	//graph name: "second order repost probability emotion network"
	//emotion x emotion
	//the second order emotional dependence for the probability an agent will repost an event when reading it
	//the source and target dimensions corresponds to the combinations of emotional states of the agent
	const Graph<float>& prp_emotion_second = graph_manager.load_optional("second order repost probability emotion network", 0.0f, emotions, false, emotions, false);

	//graph name: "first order quote probability emotion network"
	//agent x emotion
	//the first order emotional dependence for the probability an agent will quote an event when reading it
	//the source dimension corresponds to the agent reading the event
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& pq_emotion_first = graph_manager.load_optional("first order quote probability emotion network", 0.0f, agents, false, emotions, false);

	//graph name: "second order repost probability emotion network"
	//emotion x emotion
	//the second order emotional dependence for the probability an agent will repost an event when reading it
	//the source and target dimensions corresponds to the combinations of emotional states of the agent
	const Graph<float>& pq_emotion_second = graph_manager.load_optional("second order quote probability emotion network", 0.0f, emotions, false, emotions, false);

	//graph name: "first order read density emotion network"
	//agent x emotion
	//the first order emotional dependence for the probability density for how many events the agent reads each time step
	//the source dimension corresponds to the agent reading the event
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& pread_emotion_first = graph_manager.load_optional("first order read density emotion network", 0.0f, agents, false, emotions, false);

	//graph name: "second order read density emotion network"
	//emotion x emotion
	//the second order emotional dependence for the probability density for how many events the agent reads each time step
	//the source and target dimensions corresponds to the combinations of emotional states of the agent
	const Graph<float>& pread_emotion_second = graph_manager.load_optional("second order read density emotion network", 0.0f, emotions, false, emotions, false);

	//graph name: "knowledge select emotion network"
	//knowledge x emotion
	//the emotional dependence of the probability that a knowledge bit is used to create a post
	//the source dimension corresponds to knowledge bits the agent knows
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& kselect_emotion = graph_manager.load_optional("knowledge select emotion network", 0.0f, knowledge, false, emotions, false);

	//graph name: "knowledge select trust network"
	//agent x knowledge
	//the knowledge trust dependence of the probability that a knowledge bit is used to create a post
	//the source dimension corresponds to the agent creating the post
	//the target dimension corresponds to the knowledge trust the agent has
	const Graph<float>& kselect_trust = graph_manager.load_optional("knowledge select trust network", 0.0f, agents, false, knowledge, false);

	//graph name: "knowledge select bias network"
	//agent x knowledge
	//the base proability that an agent will select a knowledge bit for creating a post
	const Graph<float>& kselect = graph_manager.load_optional("knowledge select bias network", 1.0f, agents, false, knowledge, false);
};



struct SM_wf_emotions : public Social_Media_with_followers, public SM_nf_emotions {
	void initialize(void) override { SM_nf_emotions::initialize(); }
	void communicate(const InteractionMessage& msg) override { Social_Media_with_followers::communicate(msg); }
	void cleanup() override { Social_Media_with_followers::cleanup(); }
	InteractionItem convert_to_InteractionItem(media_event* _event, unsigned int sender_index, unsigned int receiver_index) const override
		{ return SM_nf_emotions::convert_to_InteractionItem(_event, sender_index, receiver_index); }
	int get_feed_priority(const media_event& _event, unsigned int user) override { return Social_Media_with_followers::get_feed_priority(_event, user); }

	SM_wf_emotions(const std::string& _media_name, InteractionItem::item_keys event_key, const dynet::ParameterMap& params, Construct& construct) :
		Social_Media_no_followers(_media_name, event_key, params, construct),
		Social_Media_with_followers(_media_name, event_key, params, construct),
		SM_nf_emotions(_media_name, event_key, params, construct) {}

	struct default_media_user : public Social_Media_with_followers::default_media_user, public SM_nf_emotions::default_media_user {
	private:
		typedef Social_Media_with_followers::default_media_user sm_follow;
		typedef SM_nf_emotions::default_media_user sm_e;
	public:
		void parse(media_event* _event) override { sm_follow::parse(_event); }
		void reply(media_event* _event) override { sm_e::reply(_event); }
		void quote(media_event* _event) override { sm_e::quote(_event); }
		void repost(media_event* _event) override { sm_e::repost(_event); }
		void enrich_event(media_event* _event) override;
		void generate_post_events() override { sm_e::generate_post_events(); }
		unsigned int get_read_count() override { return sm_e::get_read_count(); }
		unsigned int get_knowledge_selection() override { return sm_e::get_knowledge_selection(); }
		void add_mentions(media_event* _event) override { sm_follow::add_mentions(_event); }
		bool follow_user(unsigned int alter_agent_index) override { return sm_follow::follow_user(alter_agent_index); }
		bool unfollow_user(unsigned int alter_agent_index) override { return sm_follow::unfollow_user(alter_agent_index); }
		bool respond_to_follow(unsigned int alter_agent_index) override { return sm_follow::respond_to_follow(alter_agent_index); }
		unsigned int consider_recommendations() override { return sm_follow::consider_recommendations(); }
		float get_charisma() override { return sm_follow::get_charisma(); }

		default_media_user(Social_Media_no_followers* media, const Node& node) :
			Social_Media_no_followers::default_media_user(media, node),
			Social_Media_with_followers::default_media_user(media, node), 
			SM_nf_emotions::default_media_user(media, node) {}
	};

	Social_Media_no_followers::media_user* get_default_media_user(const Node& node) override { return new default_media_user(this, node); }
};




struct Facebook_nf_emotions : public SM_nf_emotions {
	Facebook_nf_emotions(const dynet::ParameterMap& parameters, Construct& construct);

	void initialize() override {
		add_base_model_to_model_manager(model_names::FB_nf);
		SM_nf_emotions::initialize();
	}
};


struct Twitter_nf_emotions : public SM_nf_emotions {
	Twitter_nf_emotions(const dynet::ParameterMap& parameters, Construct& construct);

	void initialize() override {
		add_base_model_to_model_manager(model_names::TWIT_nf);
		SM_nf_emotions::initialize();
	}
};

struct Facebook_wf_emotions : public SM_wf_emotions {
	Facebook_wf_emotions(const dynet::ParameterMap& parameters, Construct& construct);

	void initialize() override {
		Social_Media_with_followers::add_base_model_to_model_manager(model_names::FB_nf);
		Social_Media_with_followers::add_base_model_to_model_manager(model_names::FB_wf);
		Social_Media_with_followers::add_base_model_to_model_manager(model_names::FB_nf_emot);
		SM_wf_emotions::initialize();
	}
};

struct Twitter_wf_emotions : public SM_wf_emotions {
	Twitter_wf_emotions(const dynet::ParameterMap& parameters, Construct& construct);

	void initialize() override {
		Social_Media_with_followers::add_base_model_to_model_manager(model_names::TWIT_nf);
		Social_Media_with_followers::add_base_model_to_model_manager(model_names::TWIT_wf);
		Social_Media_with_followers::add_base_model_to_model_manager(model_names::TWIT_nf_emot);
		SM_wf_emotions::initialize();
	}
};
