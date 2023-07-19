#ifndef REDDIT_HEADER_GUARD
#define REDDIT_HEADER_GUARD
#include "SocialMedia.h"

namespace model_names {
    const std::string REDDIT = "Reddit Interaction Model";					         // "Reddit Interaction Model"
}

namespace graph_names {
    const std::string subreddit_mem = "subreddit membership network";					     // "subreddit membership network"
    const std::string k_type = "knowledge type network";					             // "knowledge type network"
    const std::string active_times = "user active time network";					         // "user active time newtork"
    const std::string banned_user = "banned user network";					             // "banned user network"
}

namespace nodeset_names {
    const std::string subreddits = "subreddit";					                         // "subreddit"
    const std::string k_types = "knowledge_type";				    	                 // "knowledge types"
    const std::string moderators = "moderator";                                           // "moderators
}

struct Reddit :public Social_Media_no_followers
{
public:

    Reddit(dynet::ParameterMap parameters, Construct* construct);

    Graph<bool>& subreddit_network = graph_manager->load_required(graph_names::subreddit_mem, nodeset_names::agents, nodeset_names::subreddits);
    Graph<bool>& agent_active_network = graph_manager->load_optional(graph_names::active_times, nodeset_names::agents, nodeset_names::time);
    Graph<unsigned int>& banned_agent_network = graph_manager->load_optional(graph_names::banned_user, 0u, nodeset_names::agents, false, nodeset_names::subreddits, false);
    Graph<bool>& knowledge_type_network = graph_manager->load_required(graph_names::k_type, nodeset_names::knowledge, nodeset_names::k_types);

    void think(void);
    void update_feeds(void);
    void load_users(void);

private:
    std::vector<unsigned int> subreddit_sizes;
};

#endif
