#include "pch.h" 
#include "Supp_Library.h"

#include "Template.h"
#include "GrandInteraction.h"
#include "Location.h"
#include "Forget.h"
#include "KnowledgeLearningDifficulty.h"
#include "Mail.h"
#include "Subscription.h"
#include "KnowledgeTrust.h"

// comment out this definition to disable custom media users for Social_Media_no_followers
#define CUSTOM_MEDIA_USERS

// comment out this definition to disable custom media users for Social_Media_with_followers
#define CUSTOM_MEDIA_USERS_FOLLOWERS

// uncomment this definition to enable custom media users for SM_nf_emotions
//#define CUSTOM_MEDIA_USERS_EMOTIONS

// uncomment this definition to enable custom media users for SM_wf_emotions
//#define CUSTOM_MEDIA_USERS_FOLLOWERS_EMOTIONS


#include "SocialMedia.h"
#include "Emotions.h"
#include "Output.h"


//only edit the contents of each function and include any extra files needed

#include "Interventions.h"

//both functions should be a sequence of if/esle if statements
//once the pointer is passed through the this function the relevant managers will
//take ownership and deconstruct the classes at the appropriate time
	
//It is expected that models and output will take in a ParameterMap and a Construct reference, however it is not strictly required.
//Models will require a Construct reference in order to have access to the loaded nodes, networks/graphs, other models, and the interaction queue.

Model * dynet::create_model(const std::string & model_name, const ParameterMap& parameters, Construct& construct, const std::string& version) {

	assert(std::string(Construct::version) == version);

	if (model_name == "Template Model")
		return new Template(parameters, construct);

	//************************************
	//*    add your custom model here    *
	//************************************


	if (model_name == model_names::SIM)
		return new StandardInteraction(parameters, construct);

	else if (model_name == model_names::SUB)
		return new Subscription(construct);

	else if (model_name == model_names::KDIFF)
		return new KnowledgeLearningDifficulty(construct);

	else if (model_name == model_names::KTM)
		return new KnowledgeTransactiveMemory(parameters, construct);

	else if (model_name == model_names::FORGET)
		return new Forget(construct);

	else if (model_name == model_names::MAIL)
		return new Mail(construct);

	else if (model_name == model_names::TWIT_nf)
		return new Twitter_nf(parameters, construct);

	else if (model_name == model_names::FB_nf)
		return new Facebook_nf(parameters, construct);

	else if (model_name == model_names::TWIT_wf)
		return new Twitter_wf(parameters, construct);

	else if (model_name == model_names::FB_wf)
		return new Facebook_wf(parameters, construct);

	else if (model_name == model_names::EMOT)
		return new Emotions(construct);

	else if (model_name == model_names::TWIT_nf_emot)
		return new Twitter_nf_emotions(parameters, construct);

	else if (model_name == model_names::FB_nf_emot)
		return new Facebook_nf_emotions(parameters, construct);

	else if (model_name == model_names::TWIT_wf_emot)
		return new Twitter_wf_emotions(parameters, construct);

	else if (model_name == model_names::FB_wf_emot)
		return new Facebook_wf_emotions(parameters, construct);

	else if (model_name == model_names::BELIEF)
		return new Beliefs(parameters, construct);

	else if (model_name == model_names::TASK)
		return new Tasks(parameters, construct);

	else if (model_name == model_names::GRAND)
		return new GrandInteraction(parameters, construct);

	else if (model_name == model_names::TRUST)
		return new Trust(parameters, construct);

	else if (model_name == model_names::LOC)
		return new Location(construct);

	//*************************************
	//*    add your custom model here     *
	//*************************************

	return NULL;
}

Output* dynet::create_output(const std::string& output_name, const ParameterMap& parameters, Construct& construct, const std::string& version) {

	assert(std::string(Construct::version) == version);

	if (output_name == output_names::output_graph) {
		return new Output_Graph(parameters, construct);
	}
	else if (output_name == output_names::output_dynetml) {
		return new Output_dynetml(parameters, construct);
	}
	else if (output_name == output_names::output_messages)
		return new Output_Messages(parameters, construct);
	else if (output_name == output_names::output_media_events)
		return new Output_Events(parameters, construct);
	//*************************************
	//*    add your custom output here    *
	//*************************************

	return NULL;
}

// to disable custom media users for Social_Media_no_followers comment out the definition of "CUSTOM_MEDIA_USERS" at the top of this file
#ifdef CUSTOM_MEDIA_USERS
Social_Media_no_followers::media_user* Social_Media_no_followers::load_user(const Node& node) {
	if (node.attributes.contains("censored"))
		return new Intervention1_nf(this, node);

	else if (node.attributes.contains("one time compel"))
		return new Intervention2_nf(this, node);

	else if (node.attributes.contains("compel"))
		return new Intervention3_nf(this, node);

	return new Social_Media_no_followers::default_media_user(this, node);
}
#endif

// to disable custom media users for Social_Media_with_followers comment out the definition of "CUSTOM_MEDIA_USERS_FOLLOWERS" at the top of this file
#ifdef CUSTOM_MEDIA_USERS_FOLLOWERS
Social_Media_with_followers::media_user* Social_Media_with_followers::load_user(const Node& node) {
	if (node.attributes.contains("censored"))
		return new Intervention1(this, node);

	else if (node.attributes.contains("one time compel"))
		return new Intervention2(this, node);

	else if (node.attributes.contains("compel"))
		return new Intervention3(this, node);

	return new Social_Media_with_followers::default_media_user(this, node);
}
#endif

// to enable custom media users for SM_nf_emotions uncomment the definition of "CUSTOM_MEDIA_USERS_EMOTIONS" at the top of this file
#ifdef CUSTOM_MEDIA_USERS_EMOTIONS
SM_nf_emotions::media_user* SM_nf_emotions::load_user(const Node& node) {
	return new SM_nf_emotions::default_media_user(this, node);
}
#endif

// to enable custom media users for SM_nf_emotions uncomment the definition of "CUSTOM_MEDIA_USERS_FOLLOWERS_EMOTIONS" at the top of this file
#ifdef CUSTOM_MEDIA_USERS_FOLLOWERS_EMOTIONS
SM_wf_emotions::media_user* SM_wf_emotions::load_user(const Node& node) {
	return new SM_wf_emotions::default_media_user(this, node);
}
#endif