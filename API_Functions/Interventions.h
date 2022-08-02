#pragma once
#include "pch.h"

//this user after a specified time can no longer talk about a specific piece of knowledge
struct Intervention1_nf : public Social_Media_no_followers::default_media_user
{
	// time that an agent stops broadcasting a piece of knowledge
	// obtained from the agent attribute "time"
	unsigned int time;

	// knowledge that the agent can't broadcast anymore
	// obtained from the agent attribute "knowledge"
	unsigned int knowledge;
	Intervention1_nf(Social_Media_no_followers* _media, Nodeset::iterator _node);

	unsigned int get_knowledge_selection(void);
};


struct Intervention1 : public Intervention1_nf, public virtual Social_Media_with_followers::default_media_user {
	Intervention1(Social_Media_with_followers* _media, Nodeset::iterator _node) : 
		Intervention1_nf(_media, _node), Social_Media_with_followers::default_media_user(_media, _node) {}
};

// this user at the specified time must create a post with trust value of 0 for the specified knowledge
struct Intervention2_nf : public Social_Media_no_followers::default_media_user {

	// time the user creates the post
	// obtained from the agent attribute "time"
	unsigned int time;

	// knowledge the post is about
	// obtained from the agent attribute "knowledge"
	unsigned int knowledge;

	Intervention2_nf(Social_Media_no_followers* _media, Nodeset::iterator _node);

	void generate_post_events(void);

};

struct Intervention2 : public Intervention2_nf, virtual public Social_Media_with_followers::default_media_user {
	Intervention2(Social_Media_with_followers* _media, Nodeset::iterator _node) : 
		Intervention2_nf(_media, _node), Social_Media_with_followers::default_media_user(_media, _node) {}

};

// this user on and after the specified time must always attach a trust value of 0 for the specified knowledge
// underlying element in the knowledge trust network for the corresponding agent-knowledge link is unchanged
struct Intervention3_nf : public Social_Media_no_followers::default_media_user {

	// time the user changes trust
	// obtained from the agent attribute "time"
	unsigned int time;

	// knowledge the user changes trust about
	// obtained from the agent attribute "knowledge"
	unsigned int knowledge;

	Intervention3_nf(Social_Media_no_followers* _media, Nodeset::iterator _node);

	float get_trust(unsigned int knowledge_index);

};


struct Intervention3 : public Intervention3_nf, virtual public Social_Media_with_followers::default_media_user {
	Intervention3(Social_Media_with_followers* _media, Nodeset::iterator _node) :
		Intervention3_nf(_media, _node), Social_Media_with_followers::default_media_user(_media, _node) {}
};
