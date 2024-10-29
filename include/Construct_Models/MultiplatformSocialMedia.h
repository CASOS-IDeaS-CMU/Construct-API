#pragma once
#include "SocialMedia.h"

namespace model_parameters {
    const std::string platforms = "platform model names"; //platform model names
    const std::string ban_thresholds = "ban thresholds"; //ban thresholds
    const std::string moderation_thresholds = "moderation thresholds"; //moderation thresholds
    const std::string platform_switching_preference = "platform switching preference"; //platform switching preference
    const std::string platform_switching_probability = "platform switching probability"; //platform switching probability
}

namespace node_attributes {
    const std::string moderated = "moderated"; //moderated

    const std::string moderation_threshold = "moderation threshold"; // moderation threshold
    const std::string ban_threshold = "ban threshold"; // ban threshold
}

struct Social_Media_Moderation : public Model {
    struct Platform {
        Social_Media_no_followers* platform;
        Graph<bool>* active_agent;
        unsigned int ban_threshold;
        unsigned int index;
    };

    const Nodeset& models = ns_manager.get_nodeset(nodeset_names::model);
    const Nodeset& agents = ns_manager.get_nodeset(nodeset_names::agents);

    //graph name: "media removal count network"
    //agent x model
    //how many times an agent has had their content removed from a social media
    Graph<unsigned int>& removal_count = graph_manager.load_optional(graph_names::media_removal, 0u, agents, sparse, models, dense);

    //each social media platform can be identified by their communication medium
    std::unordered_map<const CommunicationMedium*, Platform> moderated_platforms;

    //creates a mapping from a social media model's node index to its communication medium
    std::vector<const CommunicationMedium*> platform_medium = std::vector<const CommunicationMedium*>(models.size(), nullptr);

    const Nodeset& knowledge = ns_manager.get_nodeset(nodeset_names::knowledge);

    Graph<float>& moderation_threshold = graph_manager.load_optional(graph_names::moderation_threshold, 0.1f, knowledge, sparse, models, sparse);

    Social_Media_Moderation(Construct& construct) : Model(construct) {}

    void initialize() override;

    virtual void moderation_response(unsigned int user_index, const CommunicationMedium* medium) {}
    
    bool intercept(InteractionItem& item, unsigned int sender, unsigned int receiver, const CommunicationMedium* medium) override;

    virtual bool should_moderate_user(InteractionItem& item, unsigned int sender, const CommunicationMedium* medium, float threshold);
};

struct Multiplatform_Manager : public Social_Media_Moderation {
    //graph name: "platform switch probability network"
    //agent x model
    //when an agent is moderated, they will switch to being active on a different platform at each time step 
    //with a probabilty corresponding to the platform the agent is being moderated in
    Graph<float>& _platform_switch_probabilities = graph_manager.load_optional("platform switch probability network", 0.1f, agents, sparse, models, sparse);

    //graph name: "platform preference network"
    //agent x model
    //when an agent switches platform this weights which platform they will move to. Weights are normalized after removing the platform the agent is leaving
    Graph<float>& _platform_preference_weigths = graph_manager.load_optional("platform preference network", 1.0f, agents, sparse, models, sparse);
    

    Multiplatform_Manager(Construct& construct) : Social_Media_Moderation(construct) {}

    void initialize() override;

    void moderation_response(unsigned int user_index, const CommunicationMedium* medium) override;

};