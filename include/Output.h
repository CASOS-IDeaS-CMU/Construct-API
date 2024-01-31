#ifndef OUTPUT_HEADER_GUARD
#define OUTPUT_HEADER_GUARD

#include "pch.h"
#include "SocialMedia.h"
#include "json.hpp"


namespace dynet {
	const std::string WHITESPACE = " \n\r\t\f\v";

	inline std::string ltrim(const std::string& s)
	{
		size_t start = s.find_first_not_of(WHITESPACE);
		return (start == std::string::npos) ? "" : s.substr(start);
	}

	inline std::string rtrim(const std::string& s)
	{
		size_t end = s.find_last_not_of(WHITESPACE);
		return (end == std::string::npos) ? "" : s.substr(0, end + 1);
	}

	inline std::string trim(const std::string& s) {
		return rtrim(ltrim(s));
	}

	
}

struct Output_Graph : public Output {

	const Typeless_Graph* _graph;
	std::ofstream _output_file;

	Output_Graph(const dynet::ParameterMap& params, Construct& construct) : Output(params, construct) {

		if (!params.contains("timeperiods")) throw dynet::could_not_find_parameter("timeperiods");

		//names of required parameters
		const char* network_name = "network name";
		const char* output_file = "output file";


		_graph = construct.graph_manager.get_network(params.get_parameter(network_name));

		std::string file = params.get_parameter(output_file);
		if (file.size() <= 4 || ".csv" != file.substr(file.size() - 4, 4)) {
			throw dynet::wrong_file_extension(output_file, ".csv");
		}

		if (construct.working_directory != "") file = construct.working_directory + dynet::seperator() + file;

		_output_file.open(file);
		if (!_output_file.is_open()) throw dynet::could_not_open_file(file);

		for (Nodeset::iterator node = _graph->target_nodeset->begin(); node != _graph->target_nodeset->end(); ++node) {
			_output_file << "," << node->name;
		}
		_output_file << std::endl;
		_output_file << "initialization" << std::endl;

		if (should_process(-1)) _graph->get_data_state(_output_file);\
			
	}

	~Output_Graph(void) { _output_file.close(); }

	void process(unsigned int t) override {
		if (should_process(t)) {
			_output_file << "timeperiod " << t << std::endl;
			_graph->get_data_state(_output_file);
		}
	}
};

struct Output_dynetml : public Output {
	std::vector<const Typeless_Graph*> _graphs;
	std::unordered_set<const Nodeset*> _nodesets;
	std::unordered_map<const Nodeset*, const std::vector<dynet::ParameterMap>* > nodeset_attributes;
	std::ofstream _output_file;

	//std::vector<std::string> _column_names;
	//std::string nodeset_out = "<nodes>";

	~Output_dynetml(void) {
		_output_file << "</DynamicMetaNetwork>";
		_output_file.close();
	}
	void process_nodesets() {
		_output_file << "<nodes>";
		for (auto n = _nodesets.cbegin(); n != _nodesets.cend(); ++n) {
			const Nodeset& ns = **n;
			_output_file << "<nodeset type=\"" << ns.name << "\" id=\"" << ns.name << "\">";
			auto it = nodeset_attributes.find(&ns);
			if (it != nodeset_attributes.end()) {
				_output_file << "<propertyIdentities>";
				for (auto atts = it->second->at(0).cbegin(); atts != it->second->at(0).cend(); ++atts) {
					_output_file << "<propertyIdentity id=\"" << atts->first << "\" singleValued=\"true\" type=\"number\"/>";
				}
				_output_file << "</propertyIdentities>";
			}
			for (auto nit = ns.begin(); nit != ns.end(); ++nit) {
				_output_file << "<node id=\"" + nit->name + "\"";
				if (it != nodeset_attributes.end()) {
					_output_file << ">";
					const dynet::ParameterMap& attributes = it->second->at(nit->index);
					for (auto& atts : attributes) {
						_output_file << "<property id=\"" << atts.first << "\" value=\"" << atts.second << "\"/>";
					}
					_output_file << "</node>";
				}
				_output_file << "/>";
			}
			_output_file << "</nodeset>";
		}
		_output_file << "</nodes>";
	}
	void process(void) {

		process_nodesets();
		_output_file << "<networks>";
		for (unsigned int i = 0; i < _graphs.size(); i++) {
			switch (_graphs[i]->edge_type)
			{
			case(Typeless_Graph::edge_types::dbool):
				add_output(dynamic_cast<const Graph<bool>*>(_graphs[i]));
				break;
			case(Typeless_Graph::edge_types::dint):
				add_output(dynamic_cast<const Graph<int>*>(_graphs[i]));
				break;
			case(Typeless_Graph::edge_types::duint):
				add_output(dynamic_cast<const Graph<unsigned int>*>(_graphs[i]));
				break;
			case(Typeless_Graph::edge_types::dfloat):
				add_output(dynamic_cast<const Graph<float>*>(_graphs[i]));
				break;
			default:
				//any other type of graph can not be used 
				//this is mainly a limitation of ORA as it doesn't support string links or 3d networks
				assert(false);
			}
		}
		_output_file << "</networks></MetaNetwork>";
	}
	inline void add_output(const Graph<bool>* g) {
		_output_file << "<network ";
		_output_file << "sourceType=\"" << g->source_nodeset->name << "\" ";
		_output_file << "source=\"" << g->source_nodeset->name << "\" ";
		_output_file << "targetType=\"" << g->target_nodeset->name << "\" ";
		_output_file << "target=\"" << g->target_nodeset->name << "\" ";
		_output_file << "id=\"" << g->name << "\" ";
		_output_file << "isDirected=\"true\" ";
		_output_file << "allowSelfLoops=\"true\" ";
		_output_file << "isBinary=\"true\" ";
		_output_file << "dynetmlUndirectedCompressed=\"false\">";

		for (auto rows = g->begin_rows(); rows != g->end_rows(); ++rows) {
			for (auto row = rows.sparse_begin(false); row != rows.end(); ++row) {

				_output_file << "<link source=\"" << (*g->source_nodeset)[*rows].name <<
					"\" target=\"" << (*g->target_nodeset)[row.col()].name << "\"/>";

			}
		}
		
		_output_file << "</network>";
	}
	template<typename T>
	void add_output(const Graph<T>* g) {

		_output_file << "<network ";
		_output_file << "sourceType=\"" << g->source_nodeset->name << "\" ";
		_output_file << "source=\"" << g->source_nodeset->name << "\" ";
		_output_file << "targetType=\"" << g->target_nodeset->name << "\" ";
		_output_file << "target=\"" << g->target_nodeset->name << "\" ";
		_output_file << "id=\"" << g->name << "\" ";
		_output_file << "isDirected=\"true\" ";
		_output_file << "allowSelfLoops=\"true\" ";
		_output_file << "isBinary=\"false\" ";
		_output_file << "dynetmlUndirectedCompressed=\"false\">";

		for (auto rows = g->begin_rows(); rows != g->end_rows(); ++rows) {
			for (auto row = rows.full_begin(); row != rows.end(); ++row) {
				bool one = dynet::convert(*row);
				if (one) {
					_output_file << "<link source=\"" << (*g->source_nodeset)[*rows].name <<
						"\" target=\"" << (*g->target_nodeset)[row.col()].name <<
						"\" value=\"" << *row << "\"/>";
				}
			}
		}

		_output_file << "</network>";
	}
	void process(unsigned int t) override {

		if (should_process(t)) {
			_output_file << "<MetaNetwork id=\"timeperiod_" << t << "\" timePeriod=\"" << t << "\">";
			process();
		}
	}

	Output_dynetml(const dynet::ParameterMap& params, Construct& construct) : Output(params, construct) {

		//names of required parameters
		const char* network_name = "network names";
		const char* output_file = "output file";

		std::vector<std::string> network_names = dynet::split(params.get_parameter(network_name), ",");
		for (unsigned int i = 0; i < network_names.size(); i++) {
			const Typeless_Graph* g = construct.graph_manager.get_network(dynet::trim(network_names[i]));
			if (!g) {
				throw dynet::could_not_find_network(network_names[i]);
			}
			if (g->slice_nodeset) {
				throw dynet::construct_exception("Network \"" + network_names[i] + "\" can't be a 3d network");
			}
			_graphs.push_back(g);
		}


		std::unordered_set<const Nodeset*> nodes;
		for (unsigned int i = 0; i < _graphs.size(); i++) {
			_nodesets.insert(_graphs[i]->source_nodeset);
			_nodesets.insert(_graphs[i]->target_nodeset);
		}

		std::string file = params.get_parameter(output_file);

		if (file.size() <= 4 || ".xml" != file.substr(file.size() - 4, 4)) {
			throw dynet::wrong_file_extension(output_file, ".xml");
		}
		
		if (construct.working_directory != "") file = construct.working_directory + dynet::seperator() + file;

		_output_file.open(file);

		if (!_output_file.is_open()) {
			throw dynet::could_not_open_file(file);
		}
		_output_file << "<DynamicMetaNetwork id=\"Construct\">";
		if (should_process(-1)) {
			_output_file << "<MetaNetwork id=\"intitial\" timePeriod=\"-1\">";
			process();
		}
		
	}
};

struct Output_Messages : public Output {
	InteractionMessageQueue* queue;
	std::ofstream _output_file;

	~Output_Messages(void) {
		_output_file << "]";
		_output_file.close();
	}
	void process(unsigned int t) override {
		
		nlohmann::json timeperiod;
		timeperiod["timeperiod"] = t;
		nlohmann::json messages = nlohmann::json::array();
		for (auto& msg : *queue) {
			nlohmann::json json_msg;
			json_msg["sender"] = msg.sender;
			json_msg["receiver"] = msg.receiver;
			json_msg["medium name"] = msg.medium->name;
			nlohmann::json items = nlohmann::json::array();

			for (auto& item : msg) {
				nlohmann::json attributes = nlohmann::json::array();
				for (auto att : item.attributes) attributes.push_back(InteractionItem::get_item_name(att));

				nlohmann::json indexes;
				for (auto& index : item.indexes) indexes[InteractionItem::get_item_name(index.first)] = index.second;

				nlohmann::json values;
				for (auto& value : item.values) values[InteractionItem::get_item_name(value.first)] = value.second;

				nlohmann::json json_item;
				json_item["attributes"] = attributes;
				json_item["indexes"] = indexes;
				json_item["values"] = values;
				items.push_back(json_item);
			}
			json_msg["items"] = items;

			messages.push_back(json_msg);
		}
		timeperiod["messages"] = messages;
		if (t == output_times[0])
			_output_file << "[";
		_output_file << timeperiod;
		if (t == output_times.back())
			_output_file << ",";

	}


	Output_Messages(const dynet::ParameterMap& params, Construct& construct) {

		//name of required parameter
		const char* output_file = "output file";

		queue = &construct.interaction_message_queue;

		std::string file = params.get_parameter(output_file);
		if (file.size() <= 5 || ".json" != file.substr(file.size() - 5, 5)) {
			throw dynet::wrong_file_extension(output_file, ".json");
		}

		if (construct.working_directory != "") file = construct.working_directory + dynet::seperator() + file;

		_output_file.open(file);

		if (!_output_file.is_open()) {
			throw dynet::could_not_open_file(file);
		}
	}
};



struct Output_Events : public Output {
	std::ofstream _output_file;
	const Social_Media_no_followers::event_container* media_events;
	const Nodeset* agents;
	float tconvert;
	dynet::datetime time_zero;
	unsigned int event_count = 0;
	unsigned int max_time;
	bool first_event_printed = false;

	virtual const Social_Media_no_followers::event_container* get_event_list(const dynet::ParameterMap& params, Construct& construct) {
		const char* model_name = "model name";

		auto media_ptr = dynamic_cast<Social_Media_no_followers*>(construct.model_manager.get_model(params.get_parameter(model_name)));\

		return &media_ptr->list_of_events;
	}

	Output_Events(const dynet::ParameterMap& params, Construct& construct) {

		//names of required parameters
		
		const char* output_file = "output file";
		const char* start_time = "start time";
		const char* time_conversion = "time conversion to seconds";

		media_events = get_event_list(params, construct);
		agents = construct.ns_manager.get_nodeset(nodeset_names::agents);
		max_time = construct.time_count;

		std::string file = params.get_parameter(output_file);
		if (file.size() <= 5 || ".json" != file.substr(file.size() - 5, 5)) {
			throw dynet::wrong_file_extension(output_file, ".json");
		}

		if (construct.working_directory != "") file = construct.working_directory + dynet::seperator() + file;

		_output_file.open(file);

		if (!_output_file.is_open()) {
			throw dynet::could_not_open_file(file);
		}

		_output_file << "{\"data\":[";

		time_zero = dynet::datetime(params.get_parameter(start_time));

		tconvert = dynet::convert(params.get_parameter(time_conversion));
	}
	// adds indexes, mentions, and values for the media_event into the json node
	virtual void add_entities(nlohmann::json& entities, const Social_Media_no_followers::media_event& _event) {
		if (_event.mentions.size()) {
			nlohmann::json mentions = nlohmann::json::array();
			for (auto id : _event.mentions) {
				nlohmann::json mention;
				mention["id"] = (*agents)[id].name;
				mentions.push_back(mention);
			}
			entities["mentions"] = mentions;
		}

		if (_event.indexes.size()) {
			nlohmann::json indexes = nlohmann::json::array();
			for (auto& it : _event.indexes) {
				nlohmann::json index;
				index[InteractionItem::get_item_name(it.first)] = it.second;
				indexes.push_back(index);
			}
			entities["indexes"] = indexes;
		}

		if (_event.values.size()) {
			nlohmann::json values = nlohmann::json::array();
			for (auto& it : _event.values) {
				nlohmann::json value;
				value[InteractionItem::get_item_name(it.first)] = it.second;
				values.push_back(value);
			}
			entities["values"] = values;
		}
	}

	// process a specific list, usually this is Social_Media_no_followers::event_container::removed
	// however at the last time step its instead Social_Media_no_followers::list_of_events
	void process(unsigned int t, const std::list<Social_Media_no_followers::media_event>* container) {
	
		std::map<const Social_Media_no_followers::media_event*, unsigned int> event_mapping;
		for (auto tweet = container->rbegin(); tweet != container->rend(); ++tweet) {
			event_mapping[&(*tweet)] = event_count;
			dynet::datetime time_stamp = time_zero;
			time_stamp.time += (time_t)(tweet->time_stamp * tconvert);

			if (first_event_printed) {
				_output_file << ",";
			}
			else {
				first_event_printed = true;
			}

			nlohmann::json json_event;
			json_event["id"] = event_count;
			json_event["author_id"] = (*agents)[tweet->user].name;
			json_event["created_at"] = dynet::convert_datetime(time_stamp);

			nlohmann::json entities;
			add_entities(entities, *tweet);
			if (entities.size()) {
				json_event["entities"] = entities;
			}

			if (tweet->type == Social_Media_no_followers::media_event::event_type::quote) {
				nlohmann::json quote;
				quote["type"] = "quoted";
				quote["id"] = event_mapping[tweet->parent_event];
				nlohmann::json referenced_tweets = nlohmann::json::array();
				referenced_tweets.push_back(quote);
				json_event["referenced_tweets"] = referenced_tweets;
			}
			if (tweet->type == Social_Media_no_followers::media_event::event_type::repost) {
				nlohmann::json retweet;
				retweet["type"] = "retweeted";
				retweet["id"] = event_mapping[tweet->parent_event];
				nlohmann::json referenced_tweets = nlohmann::json::array();
				referenced_tweets.push_back(retweet);
				json_event["referenced_tweets"] = referenced_tweets;
			}
			if (tweet->type == Social_Media_no_followers::media_event::event_type::reply) {
				nlohmann::json reply;
				reply["type"] = "replied_to";
				reply["id"] = event_mapping[tweet->parent_event];
				nlohmann::json referenced_tweets = nlohmann::json::array();
				referenced_tweets.push_back(reply);
				json_event["referenced_tweets"] = referenced_tweets;
			}

			nlohmann::json public_metrics;
			public_metrics["retweet_count"] = tweet->reposts.size();
			public_metrics["quote_count"] = tweet->quotes.size();
			public_metrics["reply_count"] = tweet->replies.size();

			json_event["public_metrics"] = public_metrics;

			_output_file << json_event;

			++event_count;
		}
	}
	void process(unsigned int t) override {
		process(t, &media_events->removed_events);
	}

	~Output_Events() {
		process(max_time, media_events);
		_output_file << "]}";
		_output_file.close();
	}
};

#include "Reddit.h"

struct Output_Reddit_Posts : public Output {
	std::ofstream _output_file;
	const Reddit::event_container* media_events;
	int output_frequency;

	virtual const Reddit::event_container* get_event_list(const dynet::ParameterMap& params, Construct& construct) {
		auto media_ptr = dynamic_cast<Reddit*>(construct.model_manager.get_model("Reddit Interaction Model")); \
			return &media_ptr->list_of_events;
	}

	~Output_Reddit_Posts(void) {
		_output_file.flush();
		_output_file.close();
	}

	void msg_out(const Social_Media_no_followers::media_event& msg, const unsigned int current_timestep) {

		static constexpr const char* tabs = "\t\t\t";
		_output_file << "\"current timestep\", " << current_timestep << ", ";
		_output_file << "\"timestep created\", " << msg.time_stamp << ", ";
		_output_file << "\"user\", \"" << msg.user << "\", ";
		_output_file << "\"last used\", " << msg.last_used << ", ";
		_output_file << "\"prev banned\", " << msg.indexes.find(InteractionItem::item_keys::prev_banned)->second << ", ";
		_output_file << "\"subreddit\", " << msg.indexes.find(InteractionItem::item_keys::subreddit)->second << ", ";
		_output_file << "\"knowledge\", " << msg.indexes.find(InteractionItem::item_keys::knowledge)->second << ", ";
		_output_file << "\"ktrust\", " << msg.values.find(InteractionItem::item_keys::ktrust)->second << ", ";
		_output_file << "\"upvotes\", " << msg.indexes.find(InteractionItem::item_keys::upvotes)->second << ", ";
		_output_file << "\"downvotes\", " << msg.indexes.find(InteractionItem::item_keys::downvotes)->second << ", ";
		_output_file << "\"banned\", " << msg.indexes.find(InteractionItem::item_keys::banned)->second << ", ";

		//_output_file << "\"type\" : " << (std::string)(msg->type) << ", ";
		//_output_file << "\"child_size\" : " << msg->child_size() << ", ";
		_output_file << "\"parent_event\", " << msg.parent_event << ", ";
		_output_file << "\"root_event\", " << msg.root_event << ", ";
		_output_file << "\"id\", " << &msg << std::endl;

	}

	void process(unsigned int t) {

		if ((t + 1) % output_frequency == 0) {
			for (auto& msg : *media_events) {
				msg_out(msg, t);
			}
		}
	}

	Output_Reddit_Posts(const dynet::ParameterMap& params, Construct& construct) {

		//name of required parameters
		const char* output_file = "output file";
		const char* frequency = "frequency";

		media_events = get_event_list(params, construct);

		std::string file = params.get_parameter(output_file);
		if (file.size() <= 4 || ".csv" != file.substr(file.size() - 4, 4)) {
			throw dynet::wrong_file_extension(output_file, ".csv");
		}

		if (construct.working_directory != "") file = construct.working_directory + dynet::seperator() + file;

		_output_file.open(file);

		if (!_output_file.is_open()) {
			throw dynet::could_not_open_file(file);
		}

		output_frequency = std::stoi(params.get_parameter(frequency));
	}
};

#endif