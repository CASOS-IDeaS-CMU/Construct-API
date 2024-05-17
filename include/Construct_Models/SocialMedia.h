#pragma once
#include "pch.h"
#include "json.hpp"

namespace model_parameters {
    //model parameter name who's value gets entered into Social_Media_with_followers::dt
    const std::string interval_time_duration = "interval time duration";
    //model parameter name who's value gets entered into Social_Media_with_followers::age
    const std::string maximum_post_inactivity = "maximum post inactivity";
    const std::string time_conversion = "time conversion to seconds";
    const std::string start_time = "start time";
    const std::string input_file = "input file";
    const std::string agent_mask = "agent mask";
    const std::string disable_follower_recommendations = "disable follower recommendations";
}

struct Social_Media_no_followers : public Model
{


    struct media_event : InteractionItem {

        //this goes through the entire chain of events recursively and updates the last_used
        //to the most recent time. Source call is only on the root event.
        void update_last_used(float time);

        enum class event_type : char
        {
            post,
            repost,
            quote,
            reply
        };

        ~media_event();

        media_event() noexcept;

        media_event& operator=(media_event&& other) noexcept;

        media_event(media_event&& temp) noexcept;

        media_event(const media_event& temp) {
            assert(false);
        }

        media_event& operator=(const media_event& temp) {
            assert(false);
	    return *this;
        }

        //gets the size of the tree of events with this event at its root (minimum size of 1)
        unsigned int child_size(void);

        //the pointer of the event this event is replying to.
        //if equal to this, the event has no parent and is a post.
        media_event* parent_event = this;

        //the pointer of the event that is at the root of the event tree that this event is apart of.
        //if equal to this, the event is the root event for the tree.
        media_event* root_event = this;

        //list of reposts that have shared this event
        std::set<media_event*> reposts;

        //list of replies to this event
        std::set<media_event*> replies;

        //list of quotes to this event
        std::set<media_event*> quotes;

        //list of users mentioned by this event
        std::set<unsigned int> mentions;

        //agent index of the user that posted the event
        unsigned int user = 0;

        //the time that this event was created
        float time_stamp = -1;

        //the last time any event was added to the event tree this event is apart of
        //all events in the same event tree have equal values for last_used
        float last_used = -1;

        //how trending an event is
        float score = 1;

        //how trending an event is
        //bool removed = false;

        bool operator== (const media_event& a) const { return score == a.score; }
        bool operator!= (const media_event& a) const { return score != a.score; }
        bool operator<= (const media_event& a) const { return score <= a.score; }
        bool operator>= (const media_event& a) const { return score >= a.score; }
        bool operator< (const media_event& a) const { return score < a.score; }
        bool operator> (const media_event& a) const { return score > a.score; }

        //what type of event this is i.e. "post","repost", "quote", or "reply"
        event_type type = event_type::post;

        void update_root(media_event* new_root);

        void check_consistency(void) const;
    };

    struct could_not_find_property_key : public dynet::construct_exception {
        could_not_find_property_key(const std::string& key) :
            construct_exception("Could not find property key \"" + key + "\" in JSON object") {}
        could_not_find_property_key(const std::string& key, const std::string& id) :
            construct_exception("Could not find property key \"" + key + "\" in JSON object with id:" + id) {}
    };

    void load_events(const std::string& fname, const dynet::datetime& start_time, float time_conversion, const std::map<std::string, unsigned int>& agent_mask = std::map<std::string, unsigned int>()) {
        std::ifstream f(fname);
        if (f.is_open()) {
            nlohmann::json data = nlohmann::json::parse(f);
            if (data.contains("data")) {
                auto& events = data.find("data").value();
                if (events.is_array()) {
                    std::map<std::string, media_event*> id_mapping;
                    {
                        std::list<std::pair<dynet::datetime, std::string>> time_id_list;
                        std::map<dynet::datetime, std::string> time_id_mapping;
                        for (auto& _event : events) {
                            if (!_event.contains("id"))
                                throw could_not_find_property_key("id");
                            std::string id = _event.find("id").value().get<std::string>();
                            if (!_event.contains("created_at")) {
                                dynet::cout << "event id: " << id << " was discarded because it does not have a \"created_at\" property." << dynet::endl;
                                continue;
                            }
                            if (!_event.contains("author_id")) {
                                dynet::cout << "event id: " << id << " was discarded because it does not have a \"author_id\" property." << dynet::endl;
                                continue;
                            }
                            std::string time_stamp = _event.find("created_at").value().get<std::string>();
                            time_id_list.emplace_back(std::pair(dynet::datetime(time_stamp), id));

                        }
                        time_id_list.sort([](const std::pair<dynet::datetime, std::string>& a, const std::pair<dynet::datetime, std::string>& b) {return a.first > b.first; });

                        for (auto& id : time_id_list) {
                            list_of_events.emplace_front();
                            id_mapping[id.second] = &list_of_events.front();
                        }
                    }

                    float lowest_time = 0;

                    for (auto& _event : events) {
                        std::string id = _event.find("id").value().get<std::string>();
                        auto map_it = id_mapping.find(id);
                        if (map_it == id_mapping.end()) continue;
                        media_event& post = *map_it->second;
                        std::string author = _event.find("author_id").value().get<std::string>();
                        std::string time = _event.find("created_at").value().get<std::string>();

                        auto it = agent_mask.find(author);
                        if (it == agent_mask.end())
                            post.user = agents[author].index;
                        else
                            post.user = it->second;
                        time_t time_val = start_time.time - dynet::datetime(time).time;
                        post.time_stamp = time_val / time_conversion;
                        if (post.time_stamp < lowest_time) lowest_time = post.time_stamp;
                        post.last_used = post.time_stamp;
                        if (_event.contains("referenced_tweets")) {
                            auto& response_info = _event.find("referenced_tweets").value();
                            if (response_info.is_array()) {
                                auto& nest_response_info = response_info.front();
                                if (nest_response_info.contains("type") && nest_response_info.contains("id")) {
                                    std::string type = nest_response_info.find("type").value().get<std::string>();
                                    std::string parent_name = nest_response_info.find("id").value().get<std::string>();
                                    std::string test = id_mapping.begin()->first;
                                    auto idit = id_mapping.find(parent_name);
                                    assert(idit != id_mapping.end());
                                    media_event& parent = *idit->second;
                                    post.parent_event = &parent;
                                    post.root_event = parent.root_event;
                                    post.root_event->update_last_used(post.time_stamp);
                                    if (type == "quoted") {
                                        post.type = media_event::event_type::quote;
                                        parent.quotes.insert(&post);
                                    }
                                    if (type == "replied_to") {
                                        post.type = media_event::event_type::reply;
                                        parent.replies.insert(&post);
                                    }
                                    if (type == "retweeted") {
                                        post.type = media_event::event_type::repost;
                                        parent.reposts.insert(&post);
                                    }
                                }
                            }
                        }
                        if (_event.contains("entities")) {
                            auto& entities = _event.find("entities").value();
                            if (entities.contains("mentions")) {
                                auto& mentions = entities.find("mentions").value();
                                if (mentions.is_array()) {
                                    for (auto& mention : mentions) {
                                        if (mention.contains("id")) {
                                            std::string node_name = mention.find("id").value().get<std::string>();
                                            auto it = agent_mask.find(node_name);
                                            if (it == agent_mask.end())
                                                post.mentions.insert(agents[node_name].index);
                                            else
                                                post.mentions.insert(it->second);
                                        }
                                    }
                                }
                            }
                            if (entities.contains("attributes")) {
                                auto attributes = entities.find("attributes").value();
                                if (attributes.is_array()) {
                                    for (auto& att : attributes) post.attributes.insert(InteractionItem::get_item_key(att));
                                }
                            }
                            if (entities.contains("indexes")) {
                                auto indexes = entities.find("indexes").value();
                                if (indexes.is_array()) {
                                    for (auto& index : indexes) {
                                        if (index.begin() != index.end()) {
                                            post.indexes[InteractionItem::get_item_key(index.begin().key())] = dynet::convert(index.begin().value().get<std::string>());
                                        }
                                    }
                                }
                            }
                            if (entities.contains("values")) {
                                auto values = entities.find("values").value();
                                if (values.is_array()) {
                                    for (auto& val : values) {
                                        if (val.begin() != val.end()) {
                                            post.values[InteractionItem::get_item_key(val.begin().key())] = dynet::convert(val.begin().value().get<std::string>());
                                        }
                                    }
                                }
                            }
                        }
                    }

                    update_feeds(lowest_time);
                }
            }
        }
        else {
            throw dynet::could_not_open_file(fname);
        }
    }

    //class that contains all settings for a user as well as functions that dictates how each user interacts
    struct media_user {
        Social_Media_no_followers* media_ptr;

        Random& random() { return media_ptr->random; }

        media_user(Social_Media_no_followers* media) : media_ptr(media) {}

        virtual ~media_user() { ; }

        // called before reply, quote, or repost, allows the user to parse an event before responding to it
        virtual void parse(media_event* read_event) = 0;

        // when called, allows the user to reply to the submitted event
        virtual void reply(media_event* read_event) = 0;

        // when called, allows the user to quote the submitted event
        virtual void quote(media_event* read_event) = 0;

        // when called, allows the user to repost the submitted event
        virtual void repost(media_event* read_event) = 0;

        //generate post events for the time step, called during the think function
        virtual void generate_post_events(void) = 0;

        //number of events read each time step
        virtual unsigned int get_read_count(void) = 0;

        virtual std::set<media_event*> read(media_event* read_event) = 0;
    };

    struct default_media_user : public media_user {

        default_media_user(Social_Media_no_followers* reddit, const Node& node);

        Social_Media_no_followers& media() {
            Social_Media_no_followers* temp = dynamic_cast<Social_Media_no_followers*>(media_ptr);
            // if the media couldn't be up casted the desired class this assertion will be raised.
            // If you're confused why you probably have a diamond inheritence that makes casting non-trivial
            assert(temp);
            return *temp;
        }

        //this user's agent index
        unsigned int id;

        //probability density to post pdtw * dt = average number of events in a time period
        float pdp;

        //probability to reply when an event or reply is read
        float pr;

        //probability to repost when an event is read
        float prp;

        //probability to quote when an event is read
        float pqu;

        //probability density to read events (time in hours) pdread*dt=average number of read messages in a time period.
        float pdread;

        //this reads the post given and performs any actions before the post is potentially responded to
        void parse(media_event* _event);

        //this adds a reply to the post with probability equal to media_user::pr
        //if an event is created default_media_user::add_mentions is called on that event
        void reply(media_event* _event);

        //this adds a quote to the post with probability equal to media_user::prp
        //if an event is created media_user::add_mentions is called on that event
        void quote(media_event* _event);

        //this adds a repost to the post with probability equal to default_media_user::pqu
        //if an event is created default_media_user::add_mentions is called on that event
        void repost(media_event* _event);

        //user adds a number of post events based on default_media_user::pdp
        void generate_post_events(void);

        //number of events read each time step
        unsigned int get_read_count(void);

        virtual void enrich_event(media_event* _event);

        // chooses which knowledge index to add to an event
        virtual unsigned int get_knowledge_selection(void);

        std::set<media_event*> read(media_event* read_event);
    };

    class event_container : public std::list<media_event> {
        using std::list<media_event>::insert;
        using std::list<media_event>::push_back;
        using std::list<media_event>::emplace_back;
        using std::list<media_event>::emplace;
        using std::list<media_event>::swap;
        using std::list<media_event>::sort;
        using std::list<media_event>::reverse;
        using std::list<media_event>::merge;
    public:

        // by default removed_events at the beginning of the container have larger values of timestamp
        // thus the root event of a reply tree should be at the highest index of any other event in the reply tree
        std::list<media_event> removed_events;

        iterator erase(iterator _Where) noexcept {
            removed_events.emplace_back(std::move(*_Where));
            removed_events.back().check_consistency();
            return list::erase(_Where);
        }

        iterator erase(iterator _First, iterator _Last) noexcept {
            for (auto temp = _First; temp != _Last; temp++) removed_events.emplace_back(std::move(*temp));
            return list::erase(_First, _Last);
        }

        void hard_erase(iterator _Where) {
            if (_Where->type == media_event::event_type::reply) _Where->parent_event->replies.erase(&*_Where);
            if (_Where->type == media_event::event_type::quote) _Where->parent_event->quotes.erase(&*_Where);
            if (_Where->type == media_event::event_type::repost) _Where->parent_event->reposts.erase(&*_Where);
            list::erase(_Where);
        }

        void clear() noexcept {
            for (auto& it : *this) {
                removed_events.emplace_back(std::move(it));
            }
            list::clear();
        }
    };

    // list of all current active events, all users can access this list
    // new events should be added to the front of this list
    event_container list_of_events;

    virtual media_event* create_post(unsigned int knowledge_index, unsigned int id) {
        list_of_events.emplace_front();
        media_event& new_post = list_of_events.front();
        new_post.type = media_event::event_type::post;
        new_post.user = id;
        new_post.time_stamp = time;
        new_post.last_used = new_post.time_stamp;
        new_post.set_knowledge_item(knowledge_index);
        return &new_post;
    }

    virtual media_event* create_response(unsigned int id, media_event* parent) {
        list_of_events.emplace_front();
        media_event& new_post = list_of_events.front();
        new_post.user = id;
        new_post.parent_event = parent;
        new_post.root_event = parent->root_event;
        new_post.time_stamp = time;
        new_post.last_used = new_post.time_stamp;
        new_post.root_event->update_last_used(new_post.time_stamp);
        new_post.set_knowledge_item(parent->get_knowledge());
        return &new_post;
    }

    virtual media_event* create_quote(unsigned int id, media_event* parent) {
        media_event* new_post = create_response(id, parent);
        new_post->type = media_event::event_type::quote;
        parent->quotes.insert(new_post);
        return new_post;
    }

    virtual media_event* create_reply(unsigned int id, media_event* parent) {
        media_event* new_post = create_response(id, parent);
        new_post->type = media_event::event_type::reply;
        parent->replies.insert(new_post);
        return new_post;
    }

    virtual media_event* create_repost(unsigned int id, media_event* parent) {
        media_event* new_post = create_response(id, parent);
        new_post->type = media_event::event_type::repost;
        parent->reposts.insert(new_post);
        return new_post;
    }

    //call this on events after you're done creating them.
    virtual void finalize_event(media_event* _event) {
        if (construct.intercept(*_event, _event->user, agents.size(), &medium))
            if (_event == &list_of_events.front())
                list_of_events.hard_erase(list_of_events.begin());
    }

    void check_list_order() const;

    const Nodeset& agents = ns_manager.get_nodeset(nodeset_names::agents);  
    const Nodeset& knowledge = ns_manager.get_nodeset(nodeset_names::knowledge);

    

    //this is the medium used for all messages created by this model
    //it is intended to have unlimited complexity and avoid models that interact based on medium
    const CommunicationMedium medium;

    //this key is added to messages created by this model for items that contain the feed index
    const InteractionItem::item_keys event_key;



    //contains each user's feed of events pseudo-sorted by priority, also contains user-centric event info like whether a event has been read
    std::vector<std::vector<media_event*> > users_feed;

    // how many posts each agent reads each time step
    std::vector<unsigned int> read_count;

    //the maximum time a post can exist without its tree being added to
    float age;

    //the time duration between time steps
    float dt;

    //current time period
    float time = 0.0f;

    //prefix for some of the node attributes names parsed by the media_user class
    std::string media_name;

    //graphs already used by other models in construct

    //graph name - "knowledge network"
    //agent x knowledge
    Graph<bool>& knowledge_net = graph_manager.load_required(graph_names::knowledge, agents, knowledge);

    //graph name - "agent active time network"
    //agent x timeperiod
    const Graph<bool>* active_agents = graph_manager.load_optional(graph_names::active, true, agents, sparse, ns_manager.get_nodeset(nodeset_names::time), sparse);

    //list of users
    std::vector<media_user*> users;

    Social_Media_no_followers::media_user& user(unsigned int index) {
        Social_Media_no_followers::media_user* temp = dynamic_cast<Social_Media_no_followers::media_user*>(users[index]);
        // if the media couldn't be up casted the desired class this assertion will be raised.
        // If you're confused why you probably have a diamond inheritence that makes casting non-trivial
        assert(temp);
        return *temp;
    }

    //Loads all nodesets and graphs for this model and checks to ensure all required node attributes are present
    //Loads the parameters "interval time duration" into dt and "maximum post inactivity" into age
    //Uses the API function create_social_media_user to populate Social_Media_with_followers::users
    Social_Media_no_followers(const std::string& _media_name, InteractionItem::item_keys event_key, const dynet::ParameterMap& parameters, Construct& _construct);

    //delete all pointers in stored in the Social_Media_with_followers::users data structure
    virtual ~Social_Media_no_followers();

    virtual media_user* get_default_media_user(const Node& node) { return new default_media_user(this, node); }

    //agents read events in their feeds starting with the first event
    //reading an event will create a message with all relavant knowledge and trust information in items along with the event's feed index
    //messages are sent from the read event's author to the reading user and uses a CommunicationMedium with maximum complexity
    void think(void) override;

    //adds the Knowledge Parsing Model
    void initialize(void) override;

    //only parses messages that have an attribute equal to Social_Media_no_followers::event_key for the feed position index corresponding to a media_event pointer
    //that pointer is then given to media_user::read and if the user already knows the knowledge the event is passed to media_user::(reply, quote, repost)
    void communicate(const InteractionMessage& msg) override;

    //feeds are updated, the social media will recommend users to follow, and users can decide to unfollow other users
    void cleanup(void) override;

    //appends the array of InteractionItems based on the submitted event and the intended receiver of the message
    virtual void append_message(media_event* _event, InteractionMessage& msg);
    
    //virtual std::set<media_event*> read_other_events(media_event* read_event, unsigned int reader_index);

    virtual InteractionItem convert_to_InteractionItem(media_event* _event, unsigned int sender_index, unsigned int receiver_index) const;

    virtual int get_feed_priority(const media_event& _event, unsigned int user);

    virtual void update_event_scores();

    virtual void random_event_swapping(unsigned int user_index);

    //updates each user's feeds with the new events created during the time step while also discarding read events from the feed
    //events are ordered by direct replies or mentions, events of followers, and all other events
    //within each category events are sorted based on media_event::score which is set to media_event::child_size * media_event::time_stamp
    //after the events have been organized stochastic shuffling is done on 10% of the feed to avoid a fully deterministic feed
    virtual void update_feeds(float new_events_timestamp);

    std::function<bool(media_event&)> current_timestep = [this](media_event& _event) { return _event.time_stamp > time - 0.5f * dt; };

    std::function<bool(media_event&)> previous_timestep = [this](media_event& _event) { return _event.time_stamp > time - 1.5f * dt; };

    std::function<bool(media_event&)> active = [this](media_event& _event) { return _event.last_used > time - age; };

    std::function<void(Social_Media_no_followers*,unsigned int)> feed_update_output;

    std::function<void(Social_Media_no_followers*)> cleanup_output;

    template<typename function>
    auto get_events(function filter) {
        return list_of_events | std::views::filter(filter);
    }
};

namespace dynet {
    template<typename SM_model, typename Callable>
    SM_model* load_users(SM_model* model, Callable load) {
        for (const Node& node : model->agents) {
            try {
                if (!model->users[node.index]) model->users[node.index] = load(model, node);
            }
            catch (const dynet::construct_exception& e) {
                throw dynet::construct_exception("Could not load user agent \"" + node.name + "\": " + e.string());
            }
        }
        return model;
    }
}

struct Social_Media_with_followers : virtual public Social_Media_no_followers
{
    //class that contains all settings for a user as well as functions that dictates how each user interacts
    struct media_user {

        //returns true if this user decides to follow an agent when called
        virtual bool follow_user(unsigned int alter_agent_index) = 0;

        //Returns true if this user decides to unfollow an agent when called
        virtual bool unfollow_user(unsigned int alter_agent_index) = 0;

        // An alter has decided to follow this user and can decide to reciprocate that following
        virtual bool respond_to_follow(unsigned int alter_agent_index) = 0;

        // How many alters should be considered each time step for recommendations
        virtual unsigned int consider_recommendations(void) = 0;

        // gets how charismatic a user is
        virtual float get_charisma() = 0;
    };

    struct default_media_user : virtual public Social_Media_no_followers::default_media_user, public media_user {

        default_media_user(Social_Media_no_followers* media, const Node& node);

        Social_Media_with_followers& media() {
            Social_Media_with_followers* temp = dynamic_cast<Social_Media_with_followers*>(media_ptr);
            // if the media couldn't be up casted the desired class this assertion will be raised.
            // If you're confused why you probably have a diamond inheritence that makes casting non-trivial
            assert(temp);
            return *temp;
        }

        //probability density to recommend followers (time in hours) pdaf * dt = average number of recommendations in a time period.
        float pdaf;

        //scale factor to determine number of removed followees
        float rf;

        //determines how likable someone's event is going to be.
        //people with more likable posts get more followers
        float charisma;

        //If true, this user, when added as a followee by another user, will automatically reciprocate followings
        bool auto_follow;
        
        void enrich_event(media_event* _event) override;

        //mentions are added to the event if the event is a post by randomly selecting a followee
        virtual void add_mentions(media_event* post);

        //returns true if this user decides to follow an agent when called
        bool follow_user(unsigned int alter_agent_index) override;

        //Returns true if this user decides to unfollow an agent when called
        bool unfollow_user(unsigned int alter_agent_index) override;

        // An alter has decided to follow this user and can decide to reciprocate that following
        bool respond_to_follow(unsigned int alter_agent_index) override;

        // How many alters should be considered each time step for recommendations
        unsigned int consider_recommendations(void) override;

        float get_charisma() override;
    };

    Social_Media_with_followers::media_user& user(unsigned int index) {
        Social_Media_with_followers::media_user* temp = dynamic_cast<Social_Media_with_followers::media_user*>(users[index]);
        // if the media couldn't be up casted the desired class this assertion will be raised.
        // If you're confused why you probably have a diamond inheritence that makes casting non-trivial
        assert(temp);
        return *temp;
    }

    Social_Media_no_followers::media_user* get_default_media_user(const Node& node) override { return new default_media_user(this, node); }

    std::vector < std::vector<unsigned int> > responses;

    //graph name - deteremined by the media
    //agent x agent
    // if (follower_net->examine(i,j)) // agent i is following agent j
    Graph<bool>* follower_net = nullptr;

    //graph name - "knowledge trust network"
    //agent x knowledge
    Graph<float>* ktrust_net = nullptr;

    //graph name - "knowledge trust transactive memory network"
    //agent x agent x knowledge
    Graph<std::map<unsigned int, float> >* kttm = nullptr;

    bool disable_follower_recommendations = false;

    //Loads all nodesets and graphs for this model and checks to ensure all required node attributes are present
    //Loads the parameters "interval time duration" into dt and "maximum post inactivity" into age
    Social_Media_with_followers(const std::string& _media_name, InteractionItem::item_keys event_key, const dynet::ParameterMap& parameters, Construct& _construct);

//#ifdef CUSTOM_MEDIA_USERS_FOLLOWERS
//    Social_Media_no_followers::media_user* load_user(const Node& node);
//#endif

    void communicate(const InteractionMessage& msg) override;

    //feeds are updated, the social media will recommend users to follow, and users can decide to unfollow other users
    void cleanup(void) override;

    //computes the Jaccard Similarity in the follower network between the two agent indexes
    float follower_jaccard_similarity(unsigned int agent_i, unsigned int agent_j) const;

    //function gives each user recommended users to follow
    virtual void add_followees(void);

    //each users decides whether to unfollow any other user
    virtual void remove_followees(void);

    virtual int get_feed_priority(const media_event& _event, unsigned int user) override;

    std::function<void(Social_Media_with_followers*, unsigned int)> add_followees_output;
};

struct Facebook_wf : public virtual Social_Media_with_followers {
    Facebook_wf(const dynet::ParameterMap& parameters, Construct& construct);

    void initialize() override {
        add_base_model_to_model_manager(model_names::FB_nf);
        Social_Media_with_followers::initialize();
    }
};


struct Twitter_wf : public virtual Social_Media_with_followers {
    Twitter_wf(const dynet::ParameterMap& parameters, Construct& construct);

    void initialize() override {
        add_base_model_to_model_manager(model_names::TWIT_nf);
        Social_Media_with_followers::initialize();
    }
};


struct Facebook_nf : public virtual Social_Media_no_followers {
    Facebook_nf(const dynet::ParameterMap& parameters, Construct& construct);
};


struct Twitter_nf : public virtual Social_Media_no_followers {
    Twitter_nf(const dynet::ParameterMap& parameters, Construct& construct);
};
