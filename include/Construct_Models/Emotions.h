#ifndef EMOTIONS_HEADER_GUARD
#define EMOTIONS_HEADER_GUARD
#include "pch.h"

struct CONSTRUCT_LIB Emotions : public Model
{

	const Nodeset* agents = ns_manager->get_nodeset(nodeset_names::agents);
	const Nodeset* emotions = ns_manager->get_nodeset(nodeset_names::emotions);
	const Nodeset* knowledge = ns_manager->get_nodeset(nodeset_names::knowledge);

	//graph name: "emotion network"
	//agent x emotion
	//contains each agents current emotional state
	Graph<float>& emotion_net = graph_manager->load_required(graph_names::emotion_net, agents, emotions);

	//graph name: "emotion broadcast bias network"
	//agent x emotion
	//the base probability that an agent attaches an emotion independent of their currrent emotional state
	const Graph<float>& emot_broadcast_bias = graph_manager->load_optional("emotion broadcast bias network", 1.0f, agents, false, emotions, false);

	//graph name: "first order emotion broadcast network"
	//emotion x emotion
	//the first order dependence for the probability that an agent attaches an emotion
	//the source dimension correspondes to the emotion being attached 
	//the target dimension corresponds to the emotional state of an agent
	const Graph<float>& emot_broadcast_first = graph_manager->load_optional("first order emotion broadcast network", 0.0f, emotions, false, emotions, false);

	//graph name: "second order emotion broadcast network"
	//emotion x emotion x emotion
	//the second order dependence for the probability that an agent attaches an emotion
	//the source dimension corresponds to the emotion being attached
	//the target and slice dimension corresponds to combinations of emotional states of an agent
	const Graph<std::map<unsigned int, float> >& emot_broadcast_second = 
		graph_manager->load_optional("second order emotion broadcast network", std::map<unsigned int, float>(), emotions, false, emotions, false, emotions);

	//graph name: "first order emotion reading network"
	//emotion x emotion
	//the first order dependence for the emotional deflection of reading a message with emotions
	//the source dimension corresponds to the emotion being deflected
	//the target dimension corresponds to the emotions attached in a read message
	const Graph<float>& emot_reading_first = graph_manager->load_required("first order emotion reading network", emotions, emotions);

	//graph name: "second order emotion reading network"
	//emotion x emotion x emotion
	//the second order dependence for the emotional deflection of reading a message with emotions
	//the source dimension corresponds to the emotion being deflected
	//the target and slice dimension correspond to combinations of emotional states of an agent
	const Graph<std::map<unsigned int, float> >& emot_reading_second =
		graph_manager->load_optional("second order emotion reading network", std::map<unsigned int, float>(), emotions, false, emotions, false, emotions);

	//graph name: "emotion regulation bias network"
	//agent x emotion
	//the base value that an agent's emotions will tend toward independent of the agent's current emotional state
	const Graph<float>& emot_regulation_bias = graph_manager->load_required("emotion regulation bias network", agents, emotions);

	//graph name: "first order emotion regulation network"
	//emotion x emotion
	//the first order dependence for the self-regulation of an emotion
	//the source dimension corresponds to the emotion being regulated
	//the target dimension corresponds to the emotional state of an agent
	const Graph<float>& emot_regulation_first = graph_manager->load_required("first order emotion regulation network", emotions, emotions);

	//graph name: "second order emotion regulation network"
	//emotion x emotion x emotion
	//the second order dependence for the self-regulation of an emotion
	//the source dimension corresponds to the emotion being regulated
	//the target and slice dimension corresponds to combinations of emotional states of an agent
	const Graph<std::map<unsigned int, float> >& emot_regulation_second =
		graph_manager->load_optional("second order emotion regulation network", std::map<unsigned int, float>(), emotions, false, emotions, false, emotions);

	//emotions are selected probabilistically to be attached to a message
	//map pairs corresponds to an emotional index and that emotion's value for the agent_index
	//probability calculated using the emotion broadcast networks
	virtual std::map<unsigned int, float> get_attached_emotions(unsigned int agent_index);

	//allows the agent to parse the emotions attached in a message
	//the agent's emotions are updated using the deflection from the emotion reading networks
	virtual void read_emotions(const std::map<unsigned int, float>& attached_emotions, unsigned int agent_index);

	Emotions(Construct* _construct) : Model(_construct, model_names::EMOT) {}

	//attaches emotions to a message if no emotions are attached
	void update(void);

	//parses the message for emotions and updates the receiver's emotions
	void communicate(InteractionMessageQueue::iterator msg);

	//each agent self-regulates their emotions using the emotion regulation networks
	void cleanup(void);
};

#include "SocialMedia.h"

struct SM_nf_emotions;

struct SM_wf_emotions;

//these functions are defined in Supp_Library.cpp
namespace dynet {
	void load_users(SM_nf_emotions* media);
	void load_users(SM_wf_emotions* media);
}

struct CONSTRUCT_LIB SM_nf_emotions : public virtual Social_Media_no_followers {
	SM_nf_emotions(const std::string& _media_name, const dynet::ParameterMap& params, Construct* _construct) : 
		Social_Media_no_followers(_media_name, params, _construct), Model(_construct,"") {}

	const Nodeset* emotions = ns_manager->get_nodeset(nodeset_names::emotions);

	virtual void load_users() {
		dynet::load_users(this);
	}

	struct CONSTRUCT_LIB default_media_user : public Social_Media_no_followers::default_media_user {

		default_media_user(SM_nf_emotions* _media, Nodeset::iterator node) : 
			Social_Media_no_followers::default_media_user(_media, node), media(_media) {}
		SM_nf_emotions* media;

		//void read(media_event* _event);

		void reply(media_event* _event);

		void quote(media_event* _event);

		void repost(media_event* _event);

		void generate_post_events(void);

		unsigned int get_read_count(void);

		virtual unsigned int get_knowledge_selection(void);

		virtual void add_emotions(media_event* _event);
	};

	virtual void append_message(media_event* _event, InteractionMessage& msg);

	//holds all emotions attached to an event as they can not be held in the event themselves
	std::map<media_event*, std::vector<float> > attached_emotions;

	//graph name: "emotion network"
	//agent x emotion
	//holds the emotional state of each agent
	const Graph<float>& emotion_net = graph_manager->load_required("emotion network", agents, emotions);

	//graph name: "emotion broadcast bias network"
	//agent x emotion
	//the base probability that an agent attaches an emotion independent of their currrent emotional state
	const Graph<float>& emot_broadcast_bias = graph_manager->load_optional("emotion broadcast bias network", 1.0f, agents, false, emotions, false);

	//graph name: "first order emotion broadcast network"
	//emotion x emotion
	//the first order dependence for the probability that an agent attaches an emotion
	//the source dimension correspondes to the emotion being attached 
	//the target dimension corresponds to the emotional state of an agent
	const Graph<float>& emot_broadcast_first = graph_manager->load_optional("first order emotion broadcast network", 0.0f, emotions, false, emotions, false);

	//graph name: "second order emotion broadcast network"
	//emotion x emotion x emotion
	//the second order dependence for the probability that an agent attaches an emotion
	//the source dimension corresponds to the emotion being attached
	//the target and slice dimension corresponds to combinations of emotional states of an agent
	const Graph<std::map<unsigned int, float> >& emot_broadcast_second =
		graph_manager->load_optional("second order emotion broadcast network", std::map<unsigned int, float>(), emotions, false, emotions, false, emotions);
	//graph name: "first order post density emotion network"
	//agent x emotion
	//the first order emotional dependence for the probability density for how many posts an agent will make
	//the source dimension corresponds to the agent making the posts
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& pd_emotion_first = graph_manager->load_optional("first order post density emotion network", 0.0f, agents, false, emotions, false);

	//graph name: "second order post density emotion network"
	//emotion x emotion
	//the second order emotional dependence for the probability density for how many posts an agent will make
	//the source and target dimensions corresponds to the combinations of emotional states of the agent
	const Graph<float>& pd_emotion_second = graph_manager->load_optional("second order post density emotion network", 0.0f, emotions, false, emotions, false);

	//graph name: "first order reply probability emotion network"
	//agent x emotion
	//the first order emotional dependence for the probability an agent will reply to an event when reading it
	//the source dimension corresponds to the agent reading the event
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& pr_emotion_first = graph_manager->load_optional("first order reply probability emotion network", 0.0f, agents, false, emotions, false);

	//graph name: "second order reply probability emotion network"
	//emotion x emotion
	//the second order emotional dependence for the probability an agent will reply to an event when reading it
	//the source and target dimensions corresponds to the combinations of emotional states of the agent
	const Graph<float>& pr_emotion_second = graph_manager->load_optional("second order reply probability emotion network", 0.0f, emotions, false, emotions, false);

	//graph name: "first order repost probability emotion network"
	//agent x emotion
	//the first order emotional dependence for the probability an agent will repost an event when reading it
	//the source dimension corresponds to the agent reading the event
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& prp_emotion_first = graph_manager->load_optional("first order repost probability emotion network", 0.0f, agents, false, emotions, false);

	//graph name: "second order repost probability emotion network"
	//emotion x emotion
	//the second order emotional dependence for the probability an agent will repost an event when reading it
	//the source and target dimensions corresponds to the combinations of emotional states of the agent
	const Graph<float>& prp_emotion_second = graph_manager->load_optional("second order repost probability emotion network", 0.0f, emotions, false, emotions, false);

	//graph name: "first order quote probability emotion network"
	//agent x emotion
	//the first order emotional dependence for the probability an agent will quote an event when reading it
	//the source dimension corresponds to the agent reading the event
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& pq_emotion_first = graph_manager->load_optional("first order quote probability emotion network", 0.0f, agents, false, emotions, false);

	//graph name: "second order repost probability emotion network"
	//emotion x emotion
	//the second order emotional dependence for the probability an agent will repost an event when reading it
	//the source and target dimensions corresponds to the combinations of emotional states of the agent
	const Graph<float>& pq_emotion_second = graph_manager->load_optional("second order quote probability emotion network", 0.0f, emotions, false, emotions, false);

	//graph name: "first order read density emotion network"
	//agent x emotion
	//the first order emotional dependence for the probability density for how many events the agent reads each time step
	//the source dimension corresponds to the agent reading the event
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& pread_emotion_first = graph_manager->load_optional("first order read density emotion network", 0.0f, agents, false, emotions, false);

	//graph name: "second order read density emotion network"
	//emotion x emotion
	//the second order emotional dependence for the probability density for how many events the agent reads each time step
	//the source and target dimensions corresponds to the combinations of emotional states of the agent
	const Graph<float>& pread_emotion_second = graph_manager->load_optional("second order read density emotion network", 0.0f, emotions, false, emotions, false);

	//graph name: "knowledge select emotion network"
	//knowledge x emotion
	//the emotional dependence of the probability that a knowledge bit is used to create a post
	//the source dimension corresponds to knowledge bits the agent knows
	//the target dimension corresponds to the emotional state of the agent
	const Graph<float>& kselect_emotion = graph_manager->load_optional("knowledge select emotion network", 0.0f, knowledge, false, emotions, false);

	//graph name: "knowledge select trust network"
	//agent x knowledge
	//the knowledge trust dependence of the probability that a knowledge bit is used to create a post
	//the source dimension corresponds to the agent creating the post
	//the target dimension corresponds to the knowledge trust the agent has
	const Graph<float>& kselect_trust = graph_manager->load_optional("knowledge select trust network", 0.0f, agents, false, knowledge, false);

	//graph name: "knowledge select bias network"
	//agent x knowledge
	//the base proability that an agent will select a knowledge bit for creating a post
	const Graph<float>& kselect = graph_manager->load_optional("knowledge select bias network", 1.0f, agents, false, knowledge, false);
};

struct SM_wf_emotions : public Social_Media_with_followers, public virtual SM_nf_emotions {
	SM_wf_emotions(const std::string& _media_name, const dynet::ParameterMap& params, Construct* _construct) :
		Social_Media_with_followers(_media_name, params, _construct),
		Social_Media_with_followers::Social_Media_no_followers(_media_name, params, _construct),
		SM_nf_emotions(_media_name, params, _construct),
		Model(_construct, "") {}

	struct default_media_user : public Social_Media_with_followers::default_media_user, virtual public SM_nf_emotions::default_media_user {

		default_media_user(SM_wf_emotions* _media, Nodeset::iterator node) :
			Social_Media_with_followers::default_media_user(_media, node), 
			SM_nf_emotions::default_media_user(_media, node) {}
	
		void read(media_event* _event) { Social_Media_with_followers::default_media_user::read(_event); }
	
		void reply(media_event* _event) { SM_nf_emotions::default_media_user::reply(_event); }

		void quote(media_event* _event) { SM_nf_emotions::default_media_user::quote(_event); }

		void repost(media_event* _event) { SM_nf_emotions::default_media_user::repost(_event); }

		void generate_post_events(void) { SM_nf_emotions::default_media_user::generate_post_events(); }

		unsigned int get_read_count(void) { return SM_nf_emotions::default_media_user::get_read_count(); }

		virtual unsigned int get_knowledge_selection(void) { return SM_nf_emotions::default_media_user::get_knowledge_selection(); }

        virtual void add_mentions(media_event* post) { Social_Media_with_followers::default_media_user::add_mentions(post); }

        bool follow_user(unsigned int alter_agent_index) { return Social_Media_with_followers::default_media_user::follow_user(alter_agent_index); }

        bool unfollow_user(unsigned int alter_agent_index) { return Social_Media_with_followers::default_media_user::unfollow_user(alter_agent_index); }

        bool respond_to_follow(unsigned int alter_agent_index) { return Social_Media_with_followers::default_media_user::respond_to_follow(alter_agent_index); }

        unsigned int consider_recommendations(void) { return Social_Media_with_followers::default_media_user::consider_recommendations(); }

        float get_charisma() { return Social_Media_with_followers::default_media_user::get_charisma(); }

	};
	virtual void load_users() {
		dynet::load_users(this);
	}
};

struct Facebook_nf_emotions : public virtual SM_nf_emotions {
	Facebook_nf_emotions(const dynet::ParameterMap& parameters, Construct* _construct) :
		Model(_construct, model_names::FB_nf_emot),
		Social_Media_no_followers("Facebook", parameters, _construct),
		SM_nf_emotions("Facebook", parameters, _construct)
	{
		event_key = InteractionItem::item_keys::facebook_event;
	}
};


struct Twitter_nf_emotions : public virtual SM_nf_emotions {
	Twitter_nf_emotions(const dynet::ParameterMap& parameters, Construct* _construct) :
		Model(_construct, model_names::TWIT_nf_emot),
		Social_Media_no_followers("Twitter", parameters, _construct),
		SM_nf_emotions("Twitter", parameters, _construct)
	{
		event_key = InteractionItem::item_keys::twitter_event;
	}
};

struct Facebook_wf_emotions : public virtual SM_wf_emotions {
	Facebook_wf_emotions(const dynet::ParameterMap& parameters, Construct* _construct) :
		Model(_construct, model_names::FB_wf_emot),
		SM_wf_emotions("Facebook", parameters, _construct),
		SM_nf_emotions("Facebook", parameters, _construct),
		Social_Media_with_followers::Social_Media_no_followers("Facebook", parameters, _construct)
	{
		follower_net = graph_manager->load_required(graph_names::fb_friend, Social_Media_no_followers::agents, Social_Media_no_followers::agents);
		Social_Media_no_followers::event_key = InteractionItem::item_keys::facebook_event;
	}
};

struct Twitter_wf_emotions : public virtual SM_wf_emotions {
	Twitter_wf_emotions(const dynet::ParameterMap& parameters, Construct* _construct) :
		Model(_construct, model_names::TWIT_wf_emot),
		SM_wf_emotions("Twitter", parameters, _construct),
		SM_nf_emotions("Twitter", parameters, _construct),
		Social_Media_with_followers::Social_Media_no_followers("Twitter", parameters, _construct)
	{
		follower_net = graph_manager->load_required(graph_names::twit_follow, Social_Media_no_followers::agents, Social_Media_no_followers::agents);
		Social_Media_no_followers::event_key = InteractionItem::item_keys::twitter_event;
	}
};
#endif



