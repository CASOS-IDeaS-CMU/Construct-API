#include "Supp_Library.h"
#include "Template.h"
#include "GrandInteraction.h"
#include "Location.h"
#include "Forget.h"
#include "KnowledgeLearningDifficulty.h"
#include "Mail.h"
#include "Subscription.h"
#include "KnowledgeTrust.h"
#include "SocialMedia.h"
#include "Emotions.h"
#include "Reddit.h"
#include "Output.h"
#include "MultiplatformSocialMedia.h"

//only edit the contents of each function and include any extra files needed



// each function here corresponds to a specific social media platform type
// to add custom media users to a model edit the appropriate function
namespace dynet {
	
	Social_Media_no_followers::media_user* load_user(Social_Media_no_followers* media, const Node& node) {
		// add custom users here

		// example of adding a custom media user
		// return new my_custom_user(media, node)

		return media->get_default_media_user(node);
	}

	Social_Media_no_followers::media_user* load_user(Social_Media_with_followers* media, const Node& node) {
		// add custom users here

		return media->get_default_media_user(node);
	}

	Social_Media_no_followers::media_user* load_user(Reddit* media, const Node& node) {
		// add custom users here
		
		return media->get_default_media_user(node);
	}

}

//both functions should be a sequence of if/esle if statements
//once the pointer is passed through the this function the relevant managers will
//take ownership and deconstruct the classes at the appropriate time
	
//It is expected that models and output will take in a ParameterMap and a Construct reference, however it is not strictly required.
//Models will require a Construct reference in order to have access to the loaded nodes, networks/graphs, other models, and the interaction queue.

Model * dynet::create_model(const std::string & model_name, const ParameterMap& parameters, Construct& construct, const std::string& version) {

	assert(std::string(Construct::version) == version);

	if (model_name == "Template Model")
		return construct.create_model<Template>(parameters);

	//************************************
	//*    add your custom model here    *
	//************************************


	if (model_name == model_names::SIM)
		return construct.create_model<StandardInteraction>(parameters);

	else if (model_name == model_names::SUB)
		return construct.create_model<Subscription>();

	else if (model_name == model_names::KDIFF)
		return construct.create_model<KnowledgeLearningDifficulty>();

	else if (model_name == model_names::KTM)
		return construct.create_model<KnowledgeTransactiveMemory>(parameters);

	else if (model_name == model_names::FORGET)
		return construct.create_model<Forget>();

	else if (model_name == model_names::MAIL)
		return construct.create_model<Mail>();

	else if (model_name == model_names::TWIT_nf)
		return dynet::load_users(construct.create_model<Twitter_nf>(parameters),
			static_cast<Social_Media_no_followers::media_user * (*)(Social_Media_no_followers*, const Node&)>(&dynet::load_user));

	else if (model_name == model_names::FB_nf)
		return dynet::load_users(construct.create_model<Facebook_nf>(parameters),
			static_cast<Social_Media_no_followers::media_user * (*)(Social_Media_no_followers*, const Node&)>(&dynet::load_user));

	else if (model_name == model_names::TWIT_wf)
		return dynet::load_users(construct.create_model<Twitter_wf>(parameters),
			static_cast<Social_Media_no_followers::media_user * (*)(Social_Media_with_followers*, const Node&)>(&dynet::load_user));

	else if (model_name == model_names::FB_wf)
		return dynet::load_users(construct.create_model<Facebook_wf>(parameters),
			static_cast<Social_Media_no_followers::media_user* (*)(Social_Media_with_followers*, const Node&)>(&dynet::load_user));

	else if (model_name == model_names::EMOT)
		return construct.create_model<Emotions>();

	else if (model_name == model_names::BELIEF)
		return construct.create_model<Beliefs>(parameters);

	else if (model_name == model_names::TASK)
		return construct.create_model<Tasks>(parameters);

	else if (model_name == model_names::GRAND)
		return construct.create_model<GrandInteraction>(parameters);

	else if (model_name == model_names::TRUST)
		return construct.create_model<Trust>(parameters);

	else if (model_name == model_names::LOC)
		return construct.create_model<Location>();

	else if (model_name == model_names::REDDIT)
		return dynet::load_users(construct.create_model<Reddit>(parameters), 
			static_cast<Social_Media_no_followers::media_user* (*)(Reddit*,const Node&)>(&dynet::load_user));

	else if (model_name == model_names::MODERATION)
		return construct.create_model<Social_Media_Moderation>();

	else if (model_name == model_names::MULTI_MOD)
		return construct.create_model<Multiplatform_Manager>();

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