
#include "pch.h" 
#include "Supp_Library.h"
#include "Template.h"
#include "Interventions.h"


//only edit the contents of each function and include any extra files needed

namespace dynet {

	//both functions should be a sequence of if/esle if statements
	//once the pointer is passed through the this function the relevant managers will
	//take ownership and deconstruct the classes at the appropriate time
	
	//It is expected that models and output will take in a ParameterMap and the Construct pointer, however it is not strictly required.
	//Models will require the Construct pointer in order to have access to the loaded nodes, networks/graphs, and the interaction queue.

	Model * create_custom_model(const std::string & model_name, const ParameterMap& parameters, Construct * construct) {

		if (model_name == model_names::TEMP) return new Template(parameters, construct);

		//add your custom model here

		return NULL;
	}

	Output* create_custom_output(const std::string& output_name, const ParameterMap& parameters, Construct* construct) {

		return NULL;
	}

	//It is expected that this function will be able to differential and return different types of users
	//derived from the media_user class using an attribute of the node
	//for example including an attribute with name "user type" and value "spammer".
	//Social_Media is the parent class for Twitter and Facebook. Social_Media::media_name is used to differential them

	Social_Media_no_followers::media_user* create_custom_media_user_no_followers(Social_Media_no_followers* _media, Nodeset::iterator node) {

		if (node->attributes->find("censored") != node->attributes->end())
			return new Intervention1_nf(_media, node);

		else if (node->attributes->find("one time compell") != node->attributes->end())
			return new Intervention2_nf(_media, node);

		else if (node->attributes->find("compell") != node->attributes->end())
			return new Intervention3_nf(_media, node);

		return new Social_Media_no_followers::default_media_user(_media, node);
	}

	Social_Media_with_followers::media_user* create_custom_media_user_with_followers(Social_Media_with_followers* _media, Nodeset::iterator node) {

		if (node->attributes->find("censored") != node->attributes->end())
			return new Intervention1(_media, node);

		else if (node->attributes->find("one time compell") != node->attributes->end())
			return new Intervention2(_media, node);

		else if (node->attributes->find("compell") != node->attributes->end())
			return new Intervention3(_media, node);

		return new Social_Media_with_followers::default_media_user(_media, node);
	}
}