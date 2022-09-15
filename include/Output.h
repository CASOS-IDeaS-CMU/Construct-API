#ifndef OUTPUT_HEADER_GUARD
#define OUTPUT_HEADER_GUARD

#include "pch.h"
#include "SocialMedia.h"


namespace dynet {
	const std::string WHITESPACE = " \n\r\t\f\v";

	std::string ltrim(const std::string& s)
	{
		size_t start = s.find_first_not_of(WHITESPACE);
		return (start == std::string::npos) ? "" : s.substr(start);
	}

	std::string rtrim(const std::string& s)
	{
		size_t end = s.find_last_not_of(WHITESPACE);
		return (end == std::string::npos) ? "" : s.substr(0, end + 1);
	}

	std::string trim(const std::string& s) {
		return rtrim(ltrim(s));
	}

	
}

struct Output_Graph : public Output {

	const Typeless_Graph* _graph;
	std::ofstream _output_file;
	std::vector<unsigned int> output_times;
	std::vector<unsigned int>::const_iterator _next_output_time;

	Output_Graph(const dynet::ParameterMap& params, Construct* construct) {

		//names of required parameteres
		const char* timeperiods = "timeperiods";
		const char* network_name = "network name";
		const char* output_file = "output file";


		_graph = construct->graph_manager.get_network(params.get_parameter(network_name));

		std::string file = params.get_parameter(output_file);
		if (file.size() <= 4 || ".csv" != file.substr(file.size() - 4, 4)) {
			throw dynet::wrong_file_extension(output_file, ".csv");
		}

		if (construct->working_directory != "") file = construct->working_directory + dynet::seperator() + file;

		_output_file.open(file);
		if (!_output_file.is_open()) throw dynet::could_not_open_file(file);


		auto str = params.get_parameter(timeperiods);

		if (str == "last") output_times.push_back(construct->time_count - 1);
		else if (str == "all") for (unsigned int i = 0; i < construct->time_count; i++) output_times.push_back(i);
		else if (str != "initial") throw dynet::unknown_value(timeperiods, str);

		output_times.push_back(construct->time_count);

		for (Nodeset::iterator node = _graph->target_nodeset->begin(); node != _graph->target_nodeset->end(); ++node) {
			_output_file << "," << node->name;
		}
		_output_file << std::endl;
		_output_file << "initialization" << std::endl;
		_graph->get_data_state(_output_file);
	}

	~Output_Graph(void) { _output_file.close(); }

	void process(unsigned int t) {

		//output_time should be sorted so the iterator should always be less than or equal to t
		//the last element in output_time should be the time count
		assert(t >= *_next_output_time);

		if (t == *_next_output_time) {
			_output_file << "timeperiod " << t << std::endl;
			_graph->get_data_state(_output_file);
			++_next_output_time;
		}
	}
};

struct New_Output_Graph : public Output_Graph {

	New_Output_Graph(const dynet::ParameterMap& params, Construct* construct) : Output_Graph(params, construct) {
		//timeperiods will be replaced by this functionality at the next Construct revision
		const char* time_indexes = "time indexes";

		std::vector<std::string> times = dynet::split(params.get_parameter(time_indexes), ",");
		output_times.clear();
		for (auto& val : times) {
			output_times.push_back((unsigned int)dynet::convert(val));
		}
		output_times.push_back(construct->time_count);
		_next_output_time = output_times.begin();
	}

};

struct Output_dynetml : public Output {

	std::vector<const Typeless_Graph*> _graphs;
	std::unordered_set<const Nodeset*> _nodesets;
	std::unordered_map<const Nodeset*, const std::vector<dynet::ParameterMap>* > nodeset_attributes;
	std::ofstream _output_file;
	std::vector<unsigned int> output_times;
	std::vector<unsigned int>::const_iterator _next_output_time;

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

				_output_file << "<link source=\"" << g->source_nodeset->get_node_by_index(*rows)->name <<
					"\" target=\"" << g->target_nodeset->get_node_by_index(row.col())->name << "\"/>";

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
					_output_file << "<link source=\"" << g->source_nodeset->get_node_by_index(*rows)->name <<
						"\" target=\"" << g->target_nodeset->get_node_by_index(row.col())->name <<
						"\" value=\"" << *row << "\"/>";
				}
			}
		}

		_output_file << "</network>";
	}

	void process(unsigned int t) {

		//output_time should be sorted so the iterator should always be less than or equal to t
		//the last element in output_time should be the time count
		assert(t >= *_next_output_time);

		if (t == *_next_output_time) {
			_output_file << "<MetaNetwork id=\"timeperiod_" << t << "\" timePeriod=\"" << t << "\">";
			process();
			++_next_output_time;
		}
	}

	Output_dynetml(const dynet::ParameterMap& params, Construct* construct) {

		//names of required parameteres
		const char* network_name = "network names";
		const char* output_file = "output file";
		const char* timeperiods = "timeperiods";

		std::vector<std::string> network_names = dynet::split(params.get_parameter(network_name), ",");
		for (unsigned int i = 0; i < network_names.size(); i++) {
			const Typeless_Graph* g = construct->graph_manager.get_network(dynet::trim(network_names[i]));
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


		/*for (auto n = nodes.begin(); n != nodes.end(); ++n) {
			nodeset_out += "<nodeset type=\"" + (*n)->name + "\" id=\"" + (*n)->name + "\">";
			for (auto nit = (*n)->begin(); nit != (*n)->end(); ++nit) {
				nodeset_out += "<node id=\"" + nit->name + "\"/>";
			}
			nodeset_out += "</nodeset>";
		}
		nodeset_out += "</nodes>";*/

		auto str = params.get_parameter(timeperiods);
		if (str == "last") output_times.push_back(construct->time_count - 1);
		else if (str == "all") for (unsigned int i = 0; i < construct->time_count; i++) output_times.push_back(i);
		else if (str != "initial") throw dynet::unknown_value(timeperiods, str);

		_next_output_time = output_times.begin();

		std::string file = params.get_parameter(output_file);

		if (file.size() <= 4 || ".xml" != file.substr(file.size() - 4, 4)) {
			throw dynet::wrong_file_extension(output_file, ".xml");
		}
		
		if (construct->working_directory != "") file = construct->working_directory + dynet::seperator() + file;

		_output_file.open(file);

		if (!_output_file.is_open()) {
			throw dynet::could_not_open_file(file);
		}
		_output_file << "<DynamicMetaNetwork id=\"Construct\">";

		

		_output_file << "<MetaNetwork id=\"intitial\" timePeriod=\"-1\">";
		process();
	}
};

struct Output_Messages : public Output {

	InteractionMessageQueue* queue;
	std::ofstream _output_file;

	~Output_Messages(void) {
		_output_file << "}" << std::endl;
		_output_file.close();
	}

	void item_out(InteractionMessage::iterator item) {

		static constexpr const char* tabs = "\t\t\t\t\t";

		_output_file << tabs << "\"attributes\" : [";
		if (item->attributes.size()) {
			InteractionItem::attribute_iterator it = item->attributes.begin();
			_output_file << "\"" << InteractionItem::get_item_name(*it) << "\"";
			++it;
			for (; it != item->attributes.end(); ++it) {
				_output_file << "," << "\"" << InteractionItem::get_item_name(*it) << "\"";
			}
		}
		_output_file << "]," << std::endl;
		_output_file << tabs << "\"indexes\" : {" << std::endl;
		if (item->indexes.size()) {
			InteractionItem::index_iterator it = item->indexes.begin();
			_output_file << tabs << "\t\"" << InteractionItem::get_item_name(it->first) << "\" : " << it->second;
			++it;
			for (; it != item->indexes.end(); ++it) {
				_output_file << "," << std::endl;
				_output_file << tabs << "\t\"" << InteractionItem::get_item_name(it->first) << "\" : " << it->second;
			}
			_output_file << std::endl;

		}
		_output_file << tabs << "}," << std::endl;
		_output_file << tabs << "\"values\" : {" << std::endl;
		if (item->values.size()) {
			InteractionItem::value_iterator it = item->values.begin();
			_output_file << tabs << "\t\"" << InteractionItem::get_item_name(it->first) << "\" : " << it->second;
			++it;
			for (; it != item->values.end(); ++it) {
				_output_file << "," << std::endl;
				_output_file << tabs << "\t\"" << InteractionItem::get_item_name(it->first) << "\" : " << it->second;
			}
			_output_file << std::endl;
		}
		_output_file << tabs << "}" << std::endl;
	}

	void msg_out(InteractionMessageQueue::iterator msg) {

		static constexpr const char* tabs = "\t\t\t";

		_output_file << tabs << "\"sender\" : " << msg->sender << "," << std::endl;
		_output_file << tabs << "\"receiver\" : " << msg->receiver << "," << std::endl;
		_output_file << tabs << "\"medium name\" : \"" << msg->medium->name << "\"," << std::endl;
		_output_file << tabs << "\"Items\" : [" << std::endl;

		InteractionMessage::iterator item = msg->begin();
		_output_file << tabs << "\t{" << std::endl;
		if (msg->size()) {
			item_out(item);
			++item;
		}

		for (; item != msg->end(); ++item) {
			_output_file << tabs << "\t}," << std::endl;
			_output_file << tabs << "\t{" << std::endl;
			item_out(item);
		}
		_output_file << tabs << "\t}" << std::endl;
		_output_file << tabs << "]" << std::endl;
	}

	void process(unsigned int t) {
		if (t) _output_file << "}," << std::endl;;
		_output_file << "{" << std::endl;
		_output_file << "\t\"timeperiod\" : " << t;
		_output_file << "," << std::endl;
		_output_file << "\t\"messages\" : [" << std::endl;



		if (queue->begin() != queue->end()) {
			InteractionMessageQueue::iterator msg = queue->begin();
			_output_file << "\t\t{" << std::endl;

			if (msg->size()) {
				msg_out(msg);
				++msg;
			}

			for (; msg != queue->end(); ++msg) {
				_output_file << "\t\t}," << std::endl;
				_output_file << "\t\t{" << std::endl;
				msg_out(msg);
			}
			_output_file << "\t\t}" << std::endl;
		}
		_output_file << "\t]" << std::endl;
	}


	Output_Messages(const dynet::ParameterMap& params, Construct* construct) {

		//name of required parameter
		const char* output_file = "output file";

		queue = &construct->interaction_message_queue;

		std::string file = params.get_parameter(output_file);
		if (file.size() <= 5 || ".json" != file.substr(file.size() - 5, 5)) {
			throw dynet::wrong_file_extension(output_file, ".json");
		}

		if (construct->working_directory != "") file = construct->working_directory + dynet::seperator() + file;

		_output_file.open(file);

		if (!_output_file.is_open()) {
			throw dynet::could_not_open_file(file);
		}
	}
};

#include <locale>
#include <iomanip>

namespace dynet {
	struct datetime {
		time_t time;
		static constexpr const char* dateTimeFormat = "%Y-%m-%dT%H:%M:%S.000Z";

		datetime(const std::string& input_time) {
			std::istringstream ss(input_time);
			std::tm t;

			ss >> std::get_time(&t, dateTimeFormat);

			if (ss.fail()) {
				throw dynet::construct_exception("Input datetime format \"" + input_time + "\" does not match the format \"" + dateTimeFormat + "\"");
			}

			time = std::mktime(&t);
		}

		datetime() : time(0) {}
	};
}

std::ostream& operator<<(std::ostream& os, const dynet::datetime& val) {

	char buffer[25];
	std::strftime(buffer, 25, dynet::datetime::dateTimeFormat, std::localtime(&val.time));
	os << buffer;

	return os;
}

struct Output_Events : public Output {
	std::ofstream _output_file;
	const std::list<Social_Media_no_followers::media_event>* revents;
	float tconvert;
	dynet::datetime time_zero;
	unsigned int event_count = 0;
	bool first_event_printed = false;

	Output_Events(const dynet::ParameterMap& params, Construct* construct) {

		//names of required parameteres
		const char* model_name = "model name";
		const char* output_file = "output file";
		const char* start_time = "start time";
		const char* time_conversion = "time conversion to seconds";

		auto media_ptr = dynamic_cast<Social_Media_no_followers*>(construct->model_manager.get_model_by_name(params.get_parameter(model_name)));
		assert(media_ptr);

		revents = &media_ptr->list_of_events.removed_events;

		std::string file = params.get_parameter(output_file);
		if (file.size() <= 5 || ".json" != file.substr(file.size() - 5, 5)) {
			throw dynet::wrong_file_extension(output_file, ".json");
		}

		if (construct->working_directory != "") file = construct->working_directory + dynet::seperator() + file;

		_output_file.open(file);

		if (!_output_file.is_open()) {
			throw dynet::could_not_open_file(file);
		}

		_output_file << "{\"data\":[";

		time_zero = dynet::datetime(params.get_parameter(start_time));

		tconvert = dynet::convert(params.get_parameter(time_conversion));
	}

	void process(unsigned int t) {
		std::map<const Social_Media_no_followers::media_event*, unsigned int> event_mapping;
		for (auto tweet = revents->rbegin(); tweet != revents->rend(); ++tweet) {
			event_mapping[&(*tweet)] = event_count;
			dynet::datetime time_stamp = time_zero;
			time_stamp.time += (time_t)(tweet->time_stamp * tconvert);

			if (first_event_printed) {
				_output_file << ",";
			}
			else {
				first_event_printed = true;
			}
			_output_file << "{\"id\":\"" << event_count << "\",";
			_output_file << "\"autor_id\":\"" << tweet->user << "\",";
			_output_file << "\"created_at\":\"" << time_stamp << "\",";

			_output_file << "\"public_metrics\":{";
			_output_file << "\"retweet_count\":" << tweet->reposts.size() << ",";
			_output_file << "\"quote_count\":" << tweet->quotes.size() << ",";
			_output_file << "\"reply_count\":" << tweet->replies.size() << "},";

			if (tweet->type == Social_Media_no_followers::media_event::event_type::quote) {
				_output_file << "\"referenced_tweets\":[{\"type\":\"quoted\",\"id\":\"" << event_mapping[tweet->parent_event] << "\"}],";
			}
			if (tweet->type == Social_Media_no_followers::media_event::event_type::repost) {
				_output_file << "\"referenced_tweets\":[{\"type\":\"retweet\",\"id\":\"" << event_mapping[tweet->parent_event] << "\"}],";
			}
			if (tweet->type == Social_Media_no_followers::media_event::event_type::reply) {
				_output_file << "\"referenced_tweets\":[{\"type\":\"reply\",\"id\":\"" << event_mapping[tweet->parent_event] << "\"}],";
			}

			if (tweet->mentions.size()) {
				auto it = tweet->mentions.begin();
				_output_file << "\"entities\":{\"mentions\":[{\"id\":" << *it << "\"}";
				while (it != tweet->mentions.end()) {
					_output_file << ",{\"id\":\"" << *it << "\"}";
					++it;
				}
				_output_file << "]}";
			}

			if (tweet->indexes.size()) {
				auto it = tweet->indexes.begin();
				_output_file << ",\"indexes\":{\"" << InteractionItem::get_item_name(it->first) << "\":" << it->second;
				while (it != tweet->indexes.end()) {
					_output_file << ",\"" << InteractionItem::get_item_name(it->first) << "\":" << it->second;
					++it;
				}
				_output_file << "}";
			}
			if (tweet->values.size()) {
				auto it = tweet->values.begin();
				_output_file << ",\"values\":{\"" << InteractionItem::get_item_name(it->first) << "\":" << it->second;
				while (it != tweet->values.end()) {
					_output_file << ",\"" << InteractionItem::get_item_name(it->first) << "\":" << it->second;
					++it;
				}
				_output_file << "}";
			}
			_output_file << "}";

			++event_count;
		}
	}

	~Output_Events() {
		_output_file << "]}";
		_output_file.close();
	}
};

#endif