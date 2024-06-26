#pragma once
#include "pch.h"
#include "SocialMedia.h"
#include "MultiplatformSocialMedia.h"

namespace node_attributes {
    const std::string banned_threshold = "banned threshold";
    const std::string moderation_delay = "moderation delay";
    const std::string moderation_threshold = "moderation threshold";
    const std::string user_type = " user type";
    const std::string moderator = "moderator";
    const std::string view_replies_probability = " view reply probability";
    const std::string thread_browsing_probability = " thread browsing probability";
}

struct Reddit : public virtual Social_Media_no_followers {
    Reddit(const dynet::ParameterMap& parameters, Construct& _construct);

    const Nodeset& subreddits = ns_manager.get_nodeset(nodeset_names::subreddits);

    //graph name - "subreddit membership network"
    //agent x subreddit
    //specifies the subreddit subscriptions for each agent
    const Graph<bool>& subreddit_network = graph_manager.load_required(graph_names::subreddit_mem, agents, subreddits);

    //graph name - "banned user network"
    //agent x subreddit
    //edge weight reflects the number of times a user's content is banned/removed in a given subreddit
    Graph<unsigned int>& banned_agent_network = graph_manager.load_optional(graph_names::banned_user, 0u, agents, sparse, subreddits, sparse);

    Social_Media_Moderation* moderation_model = nullptr;

private:
    using Social_Media_no_followers::create_post;
public:

    media_event* create_post(unsigned int knowledge_index, unsigned int id, unsigned int subreddit) {
        media_event* ret = Social_Media_no_followers::create_post(knowledge_index, id);
        ret->indexes[InteractionItem::item_keys::subreddit] = subreddit;
        ret->indexes[InteractionItem::item_keys::upvotes] = 1;
        ret->indexes[InteractionItem::item_keys::downvotes] = 0;
        ret->indexes[InteractionItem::item_keys::prev_banned] = banned_agent_network.examine(id, subreddit);
        return ret;
    }

    media_event* create_response(unsigned int id, media_event* parent) override {
        media_event* ret = Social_Media_no_followers::create_response(id, parent);
        ret->type = media_event::event_type::reply;
        parent->replies.insert(ret);
        ret->indexes[InteractionItem::item_keys::upvotes] = 1;
        ret->indexes[InteractionItem::item_keys::downvotes] = 0;
        ret->indexes[InteractionItem::item_keys::subreddit] = parent->indexes[InteractionItem::item_keys::subreddit];
        ret->indexes[InteractionItem::item_keys::prev_banned] = banned_agent_network.examine(id, parent->indexes[InteractionItem::item_keys::subreddit]);
        return ret;
    }

    void initialize(void) override;

    int get_feed_priority(const media_event& _event, unsigned int user) override;

    void random_event_swapping(unsigned int) override {}

    void update_event_scores() override;

    //class that contains all settings for a user as well as functions that dictates how each user interacts
    struct default_media_user : public media_user
    {
        default_media_user(Social_Media_no_followers* media, const Node& node);

        Reddit& media() {
            Reddit* temp = dynamic_cast<Reddit*>(media_ptr);
            // if the media couldn't be up casted the desired class this assertion will be raised.
            // If you're confused why you probably have a diamond inheritence that makes casting non-trivial
            assert(temp);
            return *temp;
        }

        //this user's agent index
        unsigned int id;

        //probability density to read events (time in hours) pdread*dt=average number of read messages in a time period.
        float pdread;

        //probability to create a post to a subreddit
        float pp;

        //probability to reply when an event or reply is read
        float pr;

        //probability to add an upvote to an event
        float up;

        //probability to add a downvote to an event
        float dp;

        float expand;

        float reply_tree;

        //user makes a new post based on their "Reddit post probability"
        void generate_post_events(void);

        //user reads the media_event and potentially the comments associated with it
        //calls default_media_user::upvote(), default_media_user::downvote(), default_media_user::comment(), and default_media_user::update_ktrust() 
        //on the media_event to interact with the post/comment and decide whether to read follow on comments
        void parse(media_event* _event);

        //if the user is not banned from making posts/comments in the media_event's subreddit,
        //this adds a comment to the media_event with probability equal to default_media_user::pr
        void reply(media_event* _event);

        //this upvotes on the media_event with probability equal to default_media_user::up
        void upvote(media_event* _event);

        //this downvotes on the media_event with probability equal to default_media_user::dp
        void downvote(media_event* _event);

        virtual void enrich_event(media_event* _event);

        virtual unsigned int select_subreddit();

        virtual unsigned int select_knowledge();

        unsigned int get_read_count();

        std::set<media_event*> read(media_event* read_event);

        void repost(media_event* _event) {}
        void quote(media_event* _event) {}
    };

    //class that contains all settings for a moderator as well as functions that dictates how each moderator interacts
    struct reddit_moderator : public media_user
    {
        reddit_moderator(Social_Media_no_followers* media, const Node& node);

        Reddit& media() {
            Reddit* temp = dynamic_cast<Reddit*>(media_ptr);
            // if the media couldn't be up casted the desired class this assertion will be raised.
            // If you're confused why you probably have a diamond inheritence that makes casting non-trivial
            assert(temp);
            return *temp;
        }

        float delay;

        float threshold;

        //this user's agent index
        unsigned int id;

        //bans/removes media_events from the moderator's subreddits that contain a piece of knowledge that is labeled as 
        //misinformation, was posted moderation_delay timesteps prior, and has a knowledge trust value of >= moderation_threshold
        //also updates the banned_agent_network
        void generate_post_events(void);

        

        //for moderators, posting, replying, quoting, reposting, and reading is disabled 
        // upvoting and downvoting is also disabled.
        //currently not used, but could be implemented for moderators to post warnings or comments

        //currently not used, but could limit the number of events reviewed each time step
        unsigned int get_read_count(void) { return 0; };
        void parse(media_event* _event) {}
        void repost(media_event* _event) {}
        void reply(media_event* _event) {}
        void quote(media_event* _event) {}
        std::set<media_event*> read(media_event* read_event) { return std::set<media_event*>(); }
    };

    Social_Media_no_followers::media_user* get_default_media_user(const Node& node) override {
        if (node[media_name + node_attributes::user_type] == node_attributes::moderator)
            return new reddit_moderator(this, node);
        return new default_media_user(this, node);
    }

    std::vector<unsigned int> subreddit_banned_threshold = std::vector<unsigned int>(subreddits.size());;
};


