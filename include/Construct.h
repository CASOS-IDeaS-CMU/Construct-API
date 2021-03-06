#pragma once
#ifdef DEBUG
	#undef NDEBUG
	#ifndef _DEBUG
		#define _DEBUG
	#endif
#endif // DEBUG

#ifdef _DEBUG
	#ifndef DEBUG
		#define DEBUG
	#endif
#endif

#ifdef NDEBUG
#undef DEBUG
#undef _DEBUG
#undef __DEBUG__
#endif


#include <cassert>
#include <iostream>



#include <string>
#include <sstream>
#include <algorithm>
#include <float.h>
#include <fstream>

//Loading all of c++ standard library containers

#include <array>
#include <vector>

#include <map>
#include <unordered_map>

#include <deque>

#include <forward_list>
#include <list>

#include <stack>

#include <set>
#include <unordered_set>


#if defined WIN32 || defined _WIN32 || defined __CYGWIN__ || defined __MINGW32__

#ifdef CONSTRUCTDLL_EXPORTS
	#define CONSTRUCT_LIB __declspec(dllimport)
#else
	#define CONSTRUCT_LIB __declspec(dllexport)
#endif

#else
	#define CONSTRUCT_LIB
#endif

namespace dynet
{


	struct CONSTRUCT_LIB ParameterMap : public std::unordered_map<std::string, std::string> {
		const std::string& get_parameter(const std::string& parameter_key) const;
	};


	struct construct_exception : public std::runtime_error {
		construct_exception(const std::string& message) : std::runtime_error(message) {}
		std::string string(void) const { return std::string(what()); }
	};


	struct could_not_convert :public construct_exception {
		std::string type;
		could_not_convert(const std::string& _type) : construct_exception("Could not convert value to " + _type), type(_type) {}
	};


	struct could_not_convert_value : public construct_exception {
		could_not_convert_value(const could_not_convert& e, const std::string& ending = "") :
			construct_exception("Could not convert value to " + e.type + ending) {}
	};


	struct could_not_convert_parameter : public construct_exception {
		could_not_convert_parameter(const could_not_convert& e, const std::string& param_name, const std::string& ending = "") :
			construct_exception("Could not convert parameter \"" + param_name + "\" to " + e.type + ending) {}
	};


	struct could_not_convert_attribute : public construct_exception {
		could_not_convert_attribute(const could_not_convert& e, const std::string& att_name, const std::string& ending = "") :
			construct_exception("Could not convert attribute \"" + att_name + "\" to " + e.type + ending) {}
	};


	struct could_not_find_parameter : public construct_exception {
		could_not_find_parameter(const std::string& param_name) :
			construct_exception("Parameter \"" + param_name + "\" not found") {}
	};


	struct missing_node_attribute : public construct_exception {
		missing_node_attribute(const std::string& node_name, const std::string& att_name) :
			construct_exception("Attribute \"" + att_name + "\" not found for node \"" + node_name + "\"") {}
	};


	struct already_exists : public construct_exception {
		already_exists(const std::string& type, const std::string& name) :
			construct_exception("A " + type + " with name \"" + name + "\" already exists") {}
	};


	struct could_not_find_node : public construct_exception {
		could_not_find_node(const std::string& node_name, const std::string& ns_name) :
			construct_exception("Node \"" + node_name + "\" not found in nodeset \"" + ns_name +"\"") {}
	};


	struct ns_index_out_of_range : public construct_exception {
		ns_index_out_of_range(unsigned int index, const std::string& ns_name) :
			construct_exception("Index \"" + std::to_string(index) + "\" is out of range for nodeset \"" + ns_name + "\"") {}
	};


	struct could_not_find_nodeset : public construct_exception {
		could_not_find_nodeset(const std::string& nodeset_name) :
			construct_exception("Nodeset \"" + nodeset_name + "\" not found") {}
	};


	struct could_not_find_network : public construct_exception {
		could_not_find_network(const std::string& network_name) :
			construct_exception("Network \"" + network_name + "\" not found") {}
	};


	struct out_of_range : public construct_exception {
		out_of_range(const std::string& val_name, const std::string& range, const std::string& object = "") :
			construct_exception("\"" + val_name + "\"" + object + " is out of range " + range) {}
	};


	struct could_not_open_file : public construct_exception {
		could_not_open_file(const std::string& fname) : 
			construct_exception("Could not open file \"" + fname + "\"") {}
	};


	struct csv_too_many_rows : public construct_exception {
		csv_too_many_rows(const std::string& fname) : 
			construct_exception("csv file \"" + fname + "\" has too many rows") {}
	};


	struct csv_too_many_cols : public construct_exception {
		csv_too_many_cols(const std::string& fname) :
			construct_exception("csv file \"" + fname + "\" has too many columns") {}
	};


	struct csv_too_many_slcs : public construct_exception {
		csv_too_many_slcs(const std::string& fname) :
			construct_exception("csv file \"" + fname + "\" has too many slices") {}
	};


	struct csv_missing_beginning_bracket : public construct_exception {
		csv_missing_beginning_bracket(const std::string& fname) :
			construct_exception("csv file \"" + fname + "\" must have slice elements that begin with \"{\" for 3d networks") {}
	};


	struct csv_missing_ending_bracket : public construct_exception {
		csv_missing_ending_bracket(const std::string& fname) :
			construct_exception("csv file \"" + fname + "\" must have slice elements that end with \"}\" for 3d networks") {}
	};


	struct unknown_value : public construct_exception {
		unknown_value(const std::string& param_name, const std::string& param_value) :
			construct_exception("Parameter \"" + param_name + "\" was given an unknown value of \"" + param_value + "\"") {}
	};


	struct wrong_file_extension : public construct_exception {
		wrong_file_extension(const std::string& param_name, const std::string& ext) : 
			construct_exception("Parameter \"" + param_name + "\" must have file extensions of " + ext) {}
	};


	struct model_multually_exclusive : public construct_exception {
		model_multually_exclusive(const std::string& model_name) :
			construct_exception("This model is mutually exclusive with the " + model_name) {}
	};


	constexpr inline char seperator() noexcept {
#ifdef _WIN32
		return '\\';
#else
		return '/';
#endif // _WIN32

	}

	template<typename T>
	T minimum(T v1, T v2) noexcept { return (v1 > v2) * v2 + (v1 <= v2) * v1; }

	template<typename T>
	T maximum(T v1, T v2) noexcept { return (v1 > v2) * v1 + (v1 <= v2) * v2; }

	CONSTRUCT_LIB std::vector<std::string> split(const std::string& s, const std::string& delimiter) noexcept;

	template<typename T>
	class CONSTRUCT_LIB Type_Interface {
		T _data;
	public:
		Type_Interface(T data) { _data = data; }
		template <typename S> operator S() const { return S(_data); }
	};

	template<>
	class CONSTRUCT_LIB Type_Interface<bool> {
		bool _data;
		static constexpr std::array<char, 2> boolean_outputs{ { '0', '1' } };

	public:
		Type_Interface(bool data) { _data = data; }
		operator bool() const noexcept { return _data; }
		operator int() const noexcept { return (int)_data; }
		operator unsigned() const noexcept { return (unsigned)_data; }
		operator float() const noexcept { return (float)_data; }
		operator std::string() const noexcept;
	};


	template<>
	class CONSTRUCT_LIB Type_Interface<int> {
		int _data;
	public:
		Type_Interface(int data) { _data = data; }
		operator bool() const noexcept { return (bool)_data; }
		operator int() const noexcept { return _data; }
		operator unsigned() const;
		operator float() const noexcept { return (float)_data; }
		operator std::string() const;
	};

	template<>
	class CONSTRUCT_LIB Type_Interface<unsigned int> {
		unsigned int _data;
	public:
		Type_Interface(unsigned int data) { _data = data; }
		operator bool() const noexcept { return (bool)_data; }
		operator int() const noexcept { return (int)_data; }
		operator unsigned() const noexcept { return _data; }
		operator float() const noexcept { return (float)_data; }
		operator std::string() const;
	};

	template<>
	class CONSTRUCT_LIB Type_Interface<std::string> {
		std::string _data;
	public:
		Type_Interface(std::string data) { _data = data; }
		operator bool() const;
		operator int() const;
		operator unsigned() const;
		operator float() const;
		operator std::string() const noexcept { return _data; };

	};

	template<>
	class CONSTRUCT_LIB Type_Interface<float> {
		float _data;
	public:
		Type_Interface(float data) { _data = data; }
		operator bool() const noexcept { return (bool)_data; }
		operator int() const noexcept { return (int)_data; }
		operator unsigned() const;
		operator float() const noexcept { return _data; }
		operator std::string() const;
	};


	template<typename T> 
	Type_Interface<T> convert(T data) { return Type_Interface<T>(data); }

	// sends output to std::cout
	struct console_output {
		virtual void out(const std::string& data) {
			std::cout << data << std::endl;
		}
		virtual ~console_output() {}
	};


	// sends strings to an output, typically the console
	// does no operations if QUIET is defined except the output_stream::error function
	struct output_stream {

		std::stringstream ss;
		
		void error(const std::string& data) {
			output_hook->out(data);
		}

		console_output* output_hook = nullptr;

		output_stream& operator<< (output_stream& (*pf)(output_stream&)) {
#ifndef QUIET
			pf(*this);
#endif
			return *this;
		}

		output_stream& operator<< (const char* data) {
#ifndef QUIET
			ss << data;
#endif
			return *this;
		}

		template<typename T>
		output_stream& operator<<(const T& data) {
#ifndef QUIET
			ss << data;
#endif
			return *this;
		}
	};

	//similar behaviour to std::endl
	inline output_stream& endl(output_stream& out) {
#ifndef QUIET
		out.output_hook->out(out.ss.str());
		out.ss.str(std::string());
#endif
		return out;
	}
	// similar behaviour to std::cout except defining QUIET will supress all text output.
	extern output_stream cout;
}
#include <random>


class CONSTRUCT_LIB Random
{
	friend class Construct;

	//Constructor is kept private so that only Construct has access to it.
	//This will prevent models from creating their own instance.
	Random() { ; }
	std::default_random_engine generator;
	~Random() { ; }

public:
	/*<summary> Sets the random number generator seed. </summary>*/
	void set_seed(unsigned int seed) noexcept;

	//creates a random number >= 0 and <1
	float uniform() noexcept;

	//creates a random number >= min and < max
	float uniform(float min, float max) noexcept;

	//creates a random integer >= 0 and < max
	unsigned int integer(unsigned int max);

	//samples from a poisson distribution with mean equal to lambda
	unsigned int poisson_number(float lambda);

	//returns true with probability equal to the parameter probability
	bool randombool(float probability = .5);

	//samples from a normal distribution
	float normal(float mean, float variance);

	//samples from an exoponential distribution
	float exponential(float mean);

	//each element in the vector will be moved to a random position
	template<typename T>
	void vector_shuffle(std::vector<T>& A) noexcept {
		for (unsigned int i = 0; i < A.size(); i++) {
			unsigned int index = i + integer((unsigned int)A.size() - i);
			T temp = A[index];
			A[index] = A[i];
			A[i] = temp;
		}
	}

	//chooses an index based on the submitted pdf
	unsigned int find_dist_index(std::vector<float>& pdf);

	//stochastically organizes in descending order the list of indexes based on their values in the pdf.
	std::vector<unsigned int> order_by_pdf(std::vector<float>& pdf);
	
};


//names of nodesets used in Construct
namespace nodeset_names {
	const std::string agents		= "agent";			//"agent"
	const std::string knowledge		= "knowledge";		//"knowledge"
	const std::string time			= "time";			//"time"
	const std::string belief		= "belief";			//"belief"
	const std::string comm			= "medium";			//"medium"
	const std::string loc			= "location";		//"location"
	const std::string task			= "task";			//"task"
	const std::string agent_group	= "agent group";	//"agent group"
}

//names of node attributes used in Construct
namespace node_attributes {
	
	const std::string send_k			= "can send knowledge";			//"can send knowledge"
	const std::string recv_k			= "can receive knowledge";		//"can receive knowledge"
	const std::string send_t			= "can send knowledge trust";	//"can send knowledge trust"
	const std::string recv_t			= "can receive knowledge trust";//"can receive knowledge trust"
	const std::string learning_rate		= "learning rate";				//"learning rate"
	const std::string send_beliefs		= "can send beliefs";			//"can send beliefs"
	const std::string receive_beliefs	= "can receive beliefs";		//"can send beliefs"
	const std::string send_beliefsTM	= "can send beliefTM";			//"can send beliefTM"
	const std::string receive_beliefsTM = "can receive beliefTM";		//"can receive beliefTM"
	const std::string influence			= "influence";					//"influence"
	const std::string susceptiblity		= "susceptiblity";				//"susceptiblity"

	//These node attributes have prefixes that precede their name
	//See the class media_user

	const std::string post_density	= " post density";					//" post density"
	const std::string reply_prob	= " reply probability";				//" reply probability"
	const std::string repost_prob	= " repost probability";			//" repost probability"
	const std::string quote_prob	= " quote probability";				//" quote probability"
	const std::string read_density	= " reading density";				//" reading density"
	const std::string add_follow	= " add follower density";			//" add follower density"
	const std::string remove_follow = " remove follower scale factor";	//" remove follower scale factor"
	const std::string auto_follow	= " auto follow";					//" auto follow"
	const std::string charisma		= " charisma";						//" charisma"
}

//nodes are the endpoints of a network link
//nodes also have constant attributes
struct CONSTRUCT_LIB Node {

	Node(const std::string& _name, unsigned int _index, const dynet::ParameterMap* atts) : attributes(atts), name(_name), index(_index) { ; }

	const dynet::ParameterMap* const attributes;

	const std::string name;

	const unsigned int index;

	const std::string& get_attribute(const std::string& attribute_name) const;
};


class CONSTRUCT_LIB Nodeset {
	std::vector<Node> _nodes;
	std::set<const dynet::ParameterMap*> _node_attributes;
public:
	Nodeset(const std::string& _name) : name(_name) { ; }

	~Nodeset();

	const std::string name;

	using iterator = std::vector<Node>::const_iterator;

	iterator begin(void) const;

	iterator end(void) const;

	unsigned int size(void) const;

	//can only be called if the nodeset hasn't been turned to constant
	//if true is returned the submitted attributes pointer needs to be deallocated
	bool add_node(const dynet::ParameterMap* attributes);

	//can only be called if the nodeset hasn't been turned to constant
	//if true is returned the submitted attributes pointer needs to be deallocated
	bool add_node(const std::string& node_name, const dynet::ParameterMap* attributes);

	const Node* get_node_by_index(unsigned int index) const;

	const Node* get_node_by_name(const std::string& name) const noexcept;
	
	//checks to make sure the attribute once converted is in range [min,max]
	template<typename T>
	void check_attributes(std::string attribute, T min, T max) const;

	//checks to make sure the attribute can be converted to the type specialiazation
	template<typename T>
	void check_attributes(std::string attribute) const;

	//sets a mutable nodeset to an immutable nodeset
	void turn_to_const() noexcept;

	void import_dynetml(const std::string& fname, const std::string& dynetml_ns_name);

private:
	bool malliable = true;

public:

	bool is_const() const noexcept { return !malliable; }
};


class CONSTRUCT_LIB NodesetManager
{
	NodesetManager(void) { ; }
	~NodesetManager(void);

	friend class Construct;

	std::unordered_map<std::string, const Nodeset*> _nodesets;
public:

	//Creates a mutable nodeset.
	//This nodeset can become immutable after calling turn_to_const
	//only immutable nodesets can be found by get_nodeset or does_nodeset_exist
	//the pointer returned is still owned by the ns_manager
	Nodeset* create_nodeset(const std::string& name);

	//can only find a nodeset if it has been turned to constant
	const Nodeset* get_nodeset(const std::string& name) const;

	//only checks nodesets that have been turned to constant
	bool does_nodeset_exist(const std::string& name) const noexcept;

	void import_nodeset(const Nodeset* nodeset);

	void export_nodeset(const Nodeset* nodeset) noexcept;
};


namespace comms_att {
	const std::string msg_complex = "maximum message complexity";		//"maximum message complexity"
	const std::string tts = "time to send";								//"time to send"
	const std::string percent_learnable = "maximum percent learnable";	//"maximum percent learnable"
}

struct CONSTRUCT_LIB CommunicationMedium
{
	//name of the medium
	const std::string name;

	//maximum value the knowledge strength network can be increased to from a message with this medium
	const float max_percent_learnable;

	//how many timeperiods must pass before a message with this medium is sent to the central message queue
	const unsigned int time_to_send;

	//the maximum number of InteractionItem that can be included in a message with this medium
	const unsigned int max_msg_complexity;

	//node index of the medium in the "medium" nodeset
	const unsigned int index;

	//messages with valid = false mediums can not be added to queues
	const bool valid;

	//Constructor that uses a Nodeset iterator that points to a node in the "medium" nodeset
	CommunicationMedium(Nodeset::iterator node);

	//Constructor that sets each member variable explicitly
	CommunicationMedium(const std::string& name, unsigned int medium_node_index,
		float max_percent_learnable, unsigned int max_msg_complexity, unsigned int time_to_send);
};





struct CONSTRUCT_LIB InteractionItem
{
	enum class item_keys : char {
		knowledge,
		alter,
		belief,
		ktm,
		btm,
		ktrust,
		twitter_event,
		facebook_event,
		feed_position
		//ordering of the above items shall not be modified
		//new items can be added after the above list
	};


	static std::unordered_map<InteractionItem::item_keys, std::string> item_names;

	static const std::string& get_item_name(InteractionItem::item_keys key);

	using attribute_iterator = std::unordered_set<item_keys>::iterator;
	using attribute_const_iterator = std::unordered_set<item_keys>::const_iterator;

	using index_iterator = std::unordered_map<item_keys, unsigned int>::iterator;
	using index_const_iterator = std::unordered_map<item_keys, unsigned int>::const_iterator;

	using value_iterator = std::unordered_map<item_keys, float>::iterator;
	using value_const_iterator = std::unordered_map<item_keys, float>::const_iterator;
	
	InteractionItem& set_knowledge_item(unsigned int knowledge_index) noexcept;

	InteractionItem& set_knowledgeTM_item(unsigned int knowledge_index, unsigned int alter_agent) noexcept;

	InteractionItem& set_belief_item(unsigned int belief_index, float belief_value) noexcept;

	InteractionItem& set_beliefTM_item(unsigned int belief_index, unsigned int alter_agent, float belief_value) noexcept;

	InteractionItem& set_knowledge_trust_item(unsigned int knowledge_index, float ktrust) noexcept;

	static InteractionItem create_knowledge_item(unsigned int knowledge_index) noexcept;

	static InteractionItem create_knowledgeTM_item(unsigned int knowledge_index, unsigned int alter_agent) noexcept;

	static InteractionItem create_belief_item(unsigned int belief_index, float belief_value) noexcept;

	static InteractionItem create_beliefTM_item(unsigned int belief_index, unsigned int alter_agent, float belief_value) noexcept;

	static InteractionItem create_knowledge_trust_item(unsigned int knowledge_index, float ktrust) noexcept;

	bool get_knowledge_item(unsigned int& knowledge_index);

	bool get_knowledgeTM_item(unsigned int& knowledge_index, unsigned int& alter_agent);

	bool get_belief_item(unsigned int& belief_index, float& belief_value);

	bool get_beliefTM_item(unsigned int& belief_index, unsigned int& alter_agent, float& belief_value);

	bool get_knowledge_trust_item(unsigned int& knowledge_index, float& ktrust);

	void clear(void) noexcept;

	//store any relevant indexes in this unordered map
	//Note: If any non-standard keys are used, custom parsing is required when reading messages
	std::unordered_map<item_keys, unsigned int> indexes;
	//store any relevant float values in this unordered map
	//Note: If any non-standard keys are used, custom parsing is required when reading messages
	std::unordered_map<item_keys, float> values;
	//store any relevant attributes in this unordered map
	//Note: If any non-standard keys are used, custom parsing is required when reading messages
	std::unordered_set<item_keys> attributes;

	
};


class CONSTRUCT_LIB InteractionMessage
{
	//this is private so the class can track whether the message is valid
	std::vector<InteractionItem> items;
public:

	// the sender and receiver of the message
	// the sender is the "owner" of the message, and therefore
	// has chosen the knowledge, beliefs, etc to send to the receiver
	// the receiver may or may not already know these facts

	const CommunicationMedium* medium;

	unsigned int sender;

	unsigned int receiver;

	unsigned int time_to_send;

	bool valid;

	using iterator = std::vector<InteractionItem>::iterator;
	using const_iterator = std::vector<InteractionItem>::const_iterator;

	//InteractionMessage(void);

	InteractionMessage(
		unsigned int senderAgentIndex,
		unsigned int receiverAgentIndex,
		const CommunicationMedium* _medium,
		const std::vector<InteractionItem>& interactionItems = std::vector<InteractionItem>());

	

	iterator begin(void) noexcept {return items.begin();}

	iterator end(void) noexcept {return items.end();}

	const_iterator begin(void) const noexcept {return items.begin();}
	
	const_iterator end(void) const noexcept {return items.end();}

	unsigned int size() noexcept {return (unsigned int)items.size();}

	iterator erase(iterator itr) noexcept;

	bool add_item(const InteractionItem& item) noexcept;

	bool add_knowledge_item(unsigned int knowledge_index) noexcept;

	bool add_knowledgeTM_item(unsigned int knowledge_index, unsigned int alter) noexcept;

	bool add_belief_item(unsigned int belief_index, float belief_value) noexcept;

	bool add_beliefTM_item(unsigned int belief_index, unsigned int alter, float belief_value) noexcept;

	bool add_knowledge_trust_item(unsigned int knowledge_index, float ktrust) noexcept;
};



class CONSTRUCT_LIB InteractionMessageQueue
{
	//order nor does random access matter for the queue
	//constant insert and erase are more important
	std::list<InteractionMessage> _queue;
public:

	using iterator = std::list<InteractionMessage>::iterator;
	using const_iterator = std::list<InteractionMessage>::const_iterator;
	using reverse_iterator = std::list<InteractionMessage>::reverse_iterator;
	using const_reverse_iterator = std::list<InteractionMessage>::const_reverse_iterator;

	iterator begin(void) noexcept { return _queue.begin(); }

	iterator end(void) noexcept { return _queue.end(); }

	const_iterator begin(void) const noexcept { return _queue.begin(); }

	const_iterator end(void) const noexcept { return _queue.end(); }

	reverse_iterator rbegin(void) noexcept { return _queue.rbegin(); }

	reverse_iterator rend(void) noexcept { return _queue.rend(); }

	const_reverse_iterator rbegin(void)const noexcept { return _queue.rbegin(); }

	const_reverse_iterator rend(void) const noexcept { return _queue.rend(); }

	void clear(void) noexcept { _queue.clear(); }

	void addMessage(const InteractionMessage& msg);

	iterator erase(iterator itr) noexcept;
};



#ifdef max
#undef max
#endif // max

namespace dynet {
	
}

class CONSTRUCT_LIB Typeless_Graph {
	friend class GraphManager;
public:
	enum class edge_types : char {
		dbool,
		dint,
		duint,
		dfloat,
		dstring,
		vbool,
		vint,
		vuint,
		vfloat,
		vstring,
		mbool,
		mint,
		muint,
		mfloat,
		mstring
	};


protected:
	Typeless_Graph(const Nodeset* src, const Nodeset* trg, const Nodeset* slc, const std::string& network_name, edge_types edge);
public:



	virtual ~Typeless_Graph() { ; }

	virtual void push_deltas(void) noexcept = 0;

	virtual void get_data_state(std::ostream& out) const = 0;

	const Nodeset* const source_nodeset;

	const Nodeset* const target_nodeset;

	const Nodeset* const slice_nodeset;

	const std::string name;

	const unsigned int row_size;

	const unsigned int col_size;

	const edge_types edge_type;
};


//constexpr const char* get_type_name(Typeless_Graph::edge_types edge_type) noexcept {
//	switch (edge_type)
//	{
//	case Typeless_Graph::edge_types::dbool:
//		return "bool";
//	case Typeless_Graph::edge_types::dint:
//		return "int";
//	case Typeless_Graph::edge_types::duint:
//		return "unsigned int";
//	case Typeless_Graph::edge_types::dfloat:
//		return "float";
//	case Typeless_Graph::edge_types::dstring:
//		return "string";
//	case Typeless_Graph::edge_types::vbool:
//		return "vector<bool>";
//	case Typeless_Graph::edge_types::vint:
//		return "vector<int>";
//	case Typeless_Graph::edge_types::vuint:
//		return "vector<unsigned int>";
//	case Typeless_Graph::edge_types::vfloat:
//		return "vector<float>";
//	case Typeless_Graph::edge_types::vstring:
//		return "vector<string>";
//	case Typeless_Graph::edge_types::mbool:
//		return "map<bool>";
//	case Typeless_Graph::edge_types::mint:
//		return "map<int>";
//	case Typeless_Graph::edge_types::muint:
//		return "map<unsigned int>";
//	case Typeless_Graph::edge_types::mfloat:
//		return "map<float>";
//	case Typeless_Graph::edge_types::mstring:
//		return "map<string>";
//	default:
//		return "unknown";
//	}
//}





struct CONSTRUCT_LIB typeless_graph_iterator {

	//the graph will know how to cast this value to the correct type
	mutable void* _ptr;
	mutable unsigned int _row;
	mutable unsigned int _col;


	typeless_graph_iterator(unsigned int row = 0, unsigned int col = 0, void* ptr = NULL);
	virtual ~typeless_graph_iterator() {}

	unsigned int row(void) const noexcept;

	unsigned int col(void) const noexcept;

	friend bool CONSTRUCT_LIB operator==(const typeless_graph_iterator& l, const typeless_graph_iterator& r) noexcept;

	friend bool CONSTRUCT_LIB operator!=(const typeless_graph_iterator& l, const typeless_graph_iterator& r) noexcept;

	virtual unsigned int index() const noexcept;

	virtual unsigned int max() const noexcept;

	virtual const typeless_graph_iterator& operator++(void) const;

	

};

// -------------------------------------------------------------------------------------------------------------------------------------------------------- Graph

template<typename T>
class CONSTRUCT_LIB Graph : public Typeless_Graph {


	
//allows graph and its child classes to be able to access the private members of the iterator classes
//the graph classes should do as much heavy lifting as possible compared to the iterator classes

	struct delta {
		unsigned int _row;
		unsigned int _col;
		T _data;
		delta(unsigned int row, unsigned int col, const T& data) :
			_row(row), _col(col), _data(data)
		{
		}
	};


	std::vector<delta> _deltas;

	
protected:
	Graph(const Nodeset* src, bool row_dense, const Nodeset* trg, bool col_dense, const Nodeset* slc, const T& def, const std::string& name);
public:
	virtual ~Graph() { ; }

	static Typeless_Graph::edge_types get_edge_type(void) noexcept;

	static std::string get_type_name(void) noexcept;

	//all elements are intiailized with this value
	//if an element is not held in memory, it is assumed that element equals this value
	const T def_val;

	//whether the data for each column is stored in an array or tree
	const bool col_dense;

	//whether the data for each row is stored in an array or tree
	const bool row_dense;


	// ------------ iterator parents -----------------------------




	struct CONSTRUCT_LIB graph_iterator : public typeless_graph_iterator {
		Graph<T>* _parent = NULL;
		graph_iterator(unsigned int row, unsigned int col, const Graph<T>* parent, void* ptr) : typeless_graph_iterator(row, col, ptr), _parent(const_cast<Graph<T>*>(parent)) {}
	};


	struct CONSTRUCT_LIB sparse_graph_iterator {
		const T _skip;
		sparse_graph_iterator(const T& skip_data) : _skip(skip_data) {}
	};


	struct CONSTRUCT_LIB row_graph_iterator : public graph_iterator {
		using graph_iterator::graph_iterator;

		unsigned int index() const noexcept;

		unsigned int max() const noexcept;

		const T& operator*(void) const;

		const T* operator->(void) const;
	};


	struct CONSTRUCT_LIB col_graph_iterator : public graph_iterator {
		using graph_iterator::graph_iterator;

		unsigned int index() const noexcept;

		unsigned int max() const noexcept;

		const T& operator*(void) const;

		const T* operator->(void) const;
	};






	// ------------ row iterators ------------------------------





	struct CONSTRUCT_LIB const_full_row_iterator : public row_graph_iterator {
		using row_graph_iterator::row_graph_iterator;

		const const_full_row_iterator& operator++(void) const;
	};


	struct CONSTRUCT_LIB full_row_iterator : public const_full_row_iterator {
		using const_full_row_iterator::const_full_row_iterator;
	};


	struct CONSTRUCT_LIB const_sparse_row_iterator : public row_graph_iterator, public sparse_graph_iterator {
		const_sparse_row_iterator(unsigned int row, unsigned int col, const Graph<T>* parent, void* ptr, const T& skip_data) :
			row_graph_iterator(row, col, parent, ptr), sparse_graph_iterator(skip_data) {
			if (row_graph_iterator::operator*() == skip_data) this->operator++();
		}

		const const_sparse_row_iterator& operator++(void) const;
	};


	struct CONSTRUCT_LIB sparse_row_iterator : public const_sparse_row_iterator {
		using const_sparse_row_iterator::const_sparse_row_iterator;
	};






	// ------------ column iterators ---------------------------




	struct CONSTRUCT_LIB const_full_col_iterator : public col_graph_iterator {
		using col_graph_iterator::col_graph_iterator;

		const const_full_col_iterator& operator++(void) const;
	};


	struct CONSTRUCT_LIB full_col_iterator : public const_full_col_iterator {
		using const_full_col_iterator::const_full_col_iterator;
	};


	struct CONSTRUCT_LIB const_sparse_col_iterator : public col_graph_iterator, public sparse_graph_iterator {
		const_sparse_col_iterator(unsigned int row, unsigned int col, const Graph<T>* parent, void* ptr, const T& skip_data) :
			col_graph_iterator(row, col, parent, ptr), sparse_graph_iterator(skip_data) {
			if (col_graph_iterator::operator*() == skip_data) this->operator++();
		}

		const const_sparse_col_iterator& operator++(void) const;
	};


	struct CONSTRUCT_LIB sparse_col_iterator : public const_sparse_col_iterator {

		using const_sparse_col_iterator::const_sparse_col_iterator;
	};






	// -------------- begin iterators ---------------------------





	struct CONSTRUCT_LIB const_row_begin_iterator : public col_graph_iterator {
		const_row_begin_iterator(unsigned int row, const Graph<T>* parent, void* ptr) : col_graph_iterator(row, 0, parent, ptr) {}

		unsigned int operator*(void) const;

		const_full_row_iterator full_begin(void) const;

		const_sparse_row_iterator sparse_begin(const T& data) const;

		typeless_graph_iterator end(void) const;

		const const_row_begin_iterator& operator++(void) const;
	};


	struct CONSTRUCT_LIB row_begin_iterator : public const_row_begin_iterator {
		using const_row_begin_iterator::const_row_begin_iterator;
		full_row_iterator full_begin(void);

		sparse_row_iterator sparse_begin(const T& data);
	};


	struct CONSTRUCT_LIB const_col_begin_iterator : public row_graph_iterator {
		const_col_begin_iterator(unsigned int col, const Graph<T>* parent, void* ptr) : row_graph_iterator(0, col, parent, ptr) {}
		
		unsigned int operator*(void) const;

		const_full_col_iterator full_begin(void) const;

		const_sparse_col_iterator sparse_begin(const T& data) const;

		typeless_graph_iterator end(void) const;

		const const_col_begin_iterator& operator++(void) const;
	};


	struct CONSTRUCT_LIB col_begin_iterator : public const_col_begin_iterator {
		using const_col_begin_iterator::const_col_begin_iterator;

		full_col_iterator full_begin(void);

		sparse_col_iterator sparse_begin(const T& data);
	};








	//returns a reference to the element
	virtual T& at(unsigned int row, unsigned int col) = 0;

	//updates an element's value
	void at(unsigned int row, unsigned int col, const T& data);

	//returns a constant reference to the element
	virtual const T& examine(unsigned int row, unsigned int col) const = 0;

	//sets all elements to the submitted value
	virtual void clear(const T& data) noexcept = 0;

	//records the value an element should become when deltas are pushed
	void add_delta(unsigned int row, unsigned int col, const T& data);

	//updates all elements based on queued deltas
	void push_deltas(void) noexcept;

	void get_data_state(std::ostream& out) const;

	//returns a reference to the element
	virtual T& at(row_graph_iterator& it) = 0;

	//returns a reference to the element
	virtual T& at(col_graph_iterator& it) = 0;

	//updates an element's value
	void at(row_graph_iterator& it, const T& data);

	//updates an element's value
	void at(col_graph_iterator& it, const T& data);

	virtual full_row_iterator full_row_begin(unsigned int row_index) = 0;

	virtual const_full_row_iterator full_row_begin(unsigned int row_index) const = 0;

	const_full_row_iterator full_row_cbegin(unsigned int row_index) const { return full_row_begin(row_index); };


	virtual sparse_row_iterator sparse_row_begin(unsigned int row_index, const T& skip_data) = 0;

	virtual const_sparse_row_iterator sparse_row_begin(unsigned int row_index, const T& skip_data) const = 0;

	const_sparse_row_iterator sparse_row_cbegin(unsigned int row_index, const T& skip_data) const { return sparse_row_begin(row_index, skip_data); };


	virtual row_begin_iterator begin_rows(void) noexcept = 0;

	virtual const_row_begin_iterator begin_rows(void) const noexcept = 0;

	const_row_begin_iterator cbegin_rows(void) const noexcept { return begin_rows(); };


	virtual row_begin_iterator begin_rows(unsigned int row_index) = 0;

	virtual const_row_begin_iterator begin_rows(unsigned int row_index) const = 0;

	const_row_begin_iterator cbegin_rows(unsigned int row_index) const { return begin_rows(row_index); };


	const typeless_graph_iterator row_end(unsigned int row_index) const;

	const typeless_graph_iterator end_rows(void) const noexcept;

	virtual full_col_iterator full_col_begin(unsigned int col_index) = 0;

	virtual const_full_col_iterator full_col_begin(unsigned int col_index) const = 0;

	const_full_col_iterator full_col_cbegin(unsigned int col_index) const { return full_col_begin(col_index); };


	virtual sparse_col_iterator sparse_col_begin(unsigned int col_index, const T& skip_data) = 0;

	virtual const_sparse_col_iterator sparse_col_begin(unsigned int col_index, const T& skip_data) const = 0;

	const_sparse_col_iterator sparse_col_cbegin(unsigned int col_index, const T& skip_data) const { return sparse_col_begin(col_index, skip_data); };


	virtual col_begin_iterator begin_cols(void) noexcept = 0;

	virtual const_col_begin_iterator begin_cols(void) const noexcept = 0;

	const_col_begin_iterator cbegin_cols(void) const noexcept { return begin_cols(); };


	virtual col_begin_iterator begin_cols(unsigned int col_index) = 0;

	virtual const_col_begin_iterator begin_cols(unsigned int col_index) const = 0;

	const_col_begin_iterator cbegin_cols(unsigned int col_index) const { return begin_cols(col_index); };


	const typeless_graph_iterator col_end(unsigned int col_index) const;

	const typeless_graph_iterator end_cols(void) const noexcept;
	

	//These functions are helper functions for the iterators
	virtual void clear(row_graph_iterator& it) = 0;
	virtual void clear(col_graph_iterator& it) = 0;
	virtual void clear(unsigned int row, unsigned int col) = 0;
	
	virtual const T& examine(const row_graph_iterator& it) const = 0;
	virtual const T& examine(const col_graph_iterator& it) const = 0;

	virtual void advance(const const_full_row_iterator& it) const = 0;
	virtual void advance(const const_sparse_row_iterator& it) const = 0;
	virtual void advance(const const_row_begin_iterator& it) const = 0;
	virtual void advance(const const_full_col_iterator& it) const = 0;
	virtual void advance(const const_sparse_col_iterator& it) const = 0;
	virtual void advance(const const_col_begin_iterator& it) const = 0;

	virtual full_row_iterator full_begin(const row_begin_iterator& it) = 0;
	virtual const_full_row_iterator full_begin(const const_row_begin_iterator& it) const = 0;
	virtual sparse_row_iterator sparse_begin(const row_begin_iterator& it, const T& skip_data) = 0;
	virtual const_sparse_row_iterator sparse_begin(const const_row_begin_iterator& it, const T& skip_data) const = 0;

	virtual full_col_iterator full_begin(const col_begin_iterator& it) = 0;
	virtual const_full_col_iterator full_begin(const const_col_begin_iterator& it) const = 0;
	virtual sparse_col_iterator sparse_begin(const col_begin_iterator& it, const T& skip_data) = 0;
	virtual const_sparse_col_iterator sparse_begin(const const_col_begin_iterator& it, const T& skip_data) const = 0;
};


class CONSTRUCT_LIB Graph_Intermediary {
	Typeless_Graph* ptr;
public:

	Graph_Intermediary(Typeless_Graph* _ptr) : ptr(_ptr) {}
	void check_ptr() const;

	template<typename T>
	operator Graph<T>& () {
		check_ptr();
		return *(Graph<T>*)(ptr);
	}

	template<typename T>
	operator Graph<T>* () {
		if (!ptr) return nullptr;
		if (ptr->edge_type != Graph<T>::get_edge_type())
			throw dynet::construct_exception("Network \"" + ptr->name + "\" requires an edge_type of " + Graph<T>::get_type_name());
		return dynamic_cast<Graph<T>*>(ptr);
	}

	template<typename T>
	operator const Graph<T>& () const {
		check_ptr();
		return *(const Graph<T>*)(ptr);
	}

	template<typename T>
	operator const Graph<T>* () const {
		if (!ptr) return nullptr;
		if (ptr->edge_type != Graph<T>::get_edge_type())
			throw dynet::construct_exception("Network \"" + ptr->name + "\" requires an edge_type of " + Graph<T>::get_type_name());
		return dynamic_cast<const Graph<T>*>(ptr);
	}

	operator bool() const { return (bool)ptr; }
};



namespace graph_utils {

	void CONSTRUCT_LIB it_align(std::vector<typeless_graph_iterator*>& it_list);

	void CONSTRUCT_LIB init_align(std::vector<typeless_graph_iterator*>& it_list);

	void CONSTRUCT_LIB it_align_before_first(std::vector<typeless_graph_iterator*>& it_list);

	void CONSTRUCT_LIB init_align_before_first(std::vector<typeless_graph_iterator*>& it_list);
}

namespace graph_names {
	const std::string active               = "agent active time network";					     // "agent active time network"
	const std::string current_loc          = "agent current location network";                   // "agent current location network"
	const std::string group_beliefs        = "agent group belief network";                       // "agent group belief network"
	const std::string group_knowledge      = "agent group knowledge network";                    // "agent group knowledge network"
	const std::string agent_groups         = "agent group membership network";                   // "agent group membership network"
	const std::string init_count           = "agent initiation count network";                   // "agent initiation count network"
	const std::string loc_learning_rate    = "agent location learning rate network";             // "agent location learning rate network"
	const std::string loc_preference       = "agent location preference network";                // "agent location preference network"
	const std::string mail_usage           = "agent mail usage by medium network";               // "agent mail usage by medium network"
	const std::string recep_count          = "agent reception count network";                    // "agent reception count network"
	const std::string b_k_wgt              = "belief knowledge weight network";                  // "belief knowledge weight network"
	const std::string belief_msg_complex   = "belief message complexity network";                // "belief message complexity network"
	const std::string beliefs              = "belief network";                                   // "belief network"
	const std::string b_sim_wgt            = "belief similarity weight network";                 // "belief similarity weight network"
	const std::string btm                  = "belief transactive memory network";                // "belief transactive memory network"
	const std::string comm_access          = "communication medium access network";              // "communication medium access network"
	const std::string comm_pref            = "communication medium preferences network";         // "communication medium preferences network"
	const std::string fb_friend            = "facebook friend network";                          // "facebook friend network"
	const std::string interact_k_wgt       = "interaction knowledge weight network";             // "interaction knowledge weight network"
	const std::string interact             = "interaction network";                              // "interaction network"
	const std::string interact_prob        = "interaction probability network";                  // "interaction probability network"
	const std::string soi                  = "interaction sphere network";                       // "interaction sphere network"
	const std::string k_exp_wgt            = "knowledge expertise weight network";               // "knowledge expertise weight network"
	const std::string k_forget_prob        = "knowledge forgetting prob network";                // "knowledge forgetting prob network"
	const std::string k_forget_rate        = "knowledge forgetting rate network";                // "knowledge forgetting rate network"
	const std::string k_diff               = "knowledge learning difficulty network";            // "knowledge learning difficulty network"
	const std::string k_msg_complex        = "knowledge message complexity network";             // "knowledge message complexity network"
	const std::string knowledge            = "knowledge network";                                // "knowledge network"
	const std::string k_trust	           = "knowledge trust network";							 // "knowledge trust network"
	const std::string k_priority           = "knowledge priority network";                       // "knowledge priority network"
	const std::string k_sim_wgt            = "knowledge similarity weight network";              // "knowledge similarity weight network"
	const std::string k_strength           = "knowledge strength network";                       // "knowledge strength network"
	const std::string ktm                  = "knowledge transactive memory network";			 // "knowledge transactive memory network"
	const std::string learnable_k          = "learnable knowledge network";                      // "learnable knowledge network"
	const std::string loc_knowledge	       = "location knowledge network";                       // "location knowledge network"
	const std::string loc_learning_limit   = "location learning limit network";                  // "location learning limit network"
	const std::string loc_medium_access    = "location medium access network";                   // "location medium access network"
	const std::string location_network     = "location network";                                 // "location network"
	const std::string mail_check_prob      = "mail check probability network";                   // "mail check probability network"
	const std::string medium_k_access      = "medium knowledge access network";                  // "medium knowledge access network"
	const std::string kttm		           = "knowledge trust transactive memory network";       // "knowledge trust transactive memory network"
	const std::string phys_prox            = "physical proximity network";                       // "physical proximity network"
	const std::string phys_prox_wgt        = "physical proximity weight network";                // "physical proximity weight network"
	const std::string propensity           = "public propensity network";                        // "public propensity network"
	const std::string soc_prox             = "social proximity network";                         // "social proximity network"
	const std::string soc_prox_wgt         = "social proximity weight network";                  // "social proximity weight network"
	const std::string dem_prox             = "sociodemographic proximity network";               // "sociodemographic proximity network"
	const std::string dem_prox_wgt         = "sociodemographic proximity weight network";        // "sociodemographic proximity weight network"
	const std::string subs                 = "subscription network";                             // "subscription network"
	const std::string sub_probability      = "subscription probability network";                 // "subscription probability network"
	const std::string target_info          = "target information network";                       // "target information network"
	const std::string task_assignment      = "task assignment network";                          // "task assignment network"
	const std::string task_availability    = "task availability network";                        // "task availability network"
	const std::string task_completion      = "task completion network";                          // "task completion network"
	const std::string task_guess_prob      = "task guess probability network";                   // "task guess probability network"
	const std::string task_k_importance    = "task knowledge importance network";                // "task knowledge importance network"
	const std::string task_k_req           = "task knowledge requirement network";               // "task knowledge requirement network"
	const std::string btm_msg_complex      = "transactive belief message complexity network";    // "transactive belief message complexity network"
	const std::string ktm_msg_complex      = "transactive knowledge message complexity network"; // "transactive knowledge message complexity network"
	const std::string twit_follow          = "twitter follower network";                         // "twitter follower network"
	const std::string unused               = "unused knowledge network";                         // "unused knowledge network"
}

namespace generator_names {

	//generator parameters

	const std::string density = "density"; //"density"
	const std::string symmetrical = "symmetrical"; //"symmetrical"
	const std::string min = "min"; //"min"
	const std::string max = "max"; //"max"
	const std::string begin = "begin"; //"begin"
	const std::string end = "end"; //"end"
	const std::string first = "first"; //"first"
	const std::string last = "last"; //"last"
	const std::string p_net = "perception network"; //"perception network"
	const std::string inf_net = "influence network"; //"influence network"
	const std::string variance = "variance"; //"variance"
	const std::string fpr = "false positive rate"; //"false positive rate"
	const std::string fnr = "false negative rate"; //"false positive rate"
	const std::string noise = "noise implementation"; //"noise implementation"
	const std::string unit_normal = "unit normal"; //"unit normal"
	const std::string normal = "normal"; //"normal"

	//generator names

	const std::string random_uniform = "random uniform"; //"random uniform"
	const std::string random_binary = "random binary"; //"random binary"
	const std::string perception = "perception"; //"perception"
	const std::string dynetml = "dynetml"; //"dynetml"
	const std::string csv = "csv";
}


// -------------------------------------------------------------------------------------------------------------------------------------------------------- Graph Manager


class CONSTRUCT_LIB GraphManager 
{
	GraphManager(Construct* _construct);
	~GraphManager();

	friend class Construct;

	Construct* construct;

	std::unordered_map<std::string, Typeless_Graph*> _graphs;

	//push all the deltas of all graphs
	void push_deltas(void);

public:

	std::set<Typeless_Graph*> imported_graphs;

	bool is_imported(Typeless_Graph* graph);

	// gets a graph
	// graph is required to already be loaded
	Graph_Intermediary load_required(
		const std::string& name, 
		const Nodeset* src, 
		const Nodeset* trg, 
		const Nodeset* slc = nullptr) const;

	// gets a graph
	// graph is required to already be loaded
	Graph_Intermediary load_required(
		const std::string& name,
		const std::string& source_nodeset,
		const std::string& target_nodeset,
		const std::string& slice_nodeset = "") const;

	// gets a graph
	// returns a null pointer if the graph can't be found
	Graph_Intermediary load_optional(
		const std::string& name, 
		const Nodeset* src, 
		const Nodeset* trg, 
		const Nodeset* slc = nullptr) const;

	// gets a graph
	// returns a null pointer if the graph can't be found
	Graph_Intermediary load_optional(
		const std::string& name,
		const std::string& source_nodeset,
		const std::string& target_nodeset,
		const std::string& slice_nodeset = "") const;

	// gets a graph
	// if the graph can't be found, one is created
	template<typename T>
	Graph_Intermediary load_optional(const std::string& name, const T& vals,
		const Nodeset* src, bool row_dense,
		const Nodeset* trg, bool col_dense,
		const Nodeset* slc = nullptr);

	// gets a graph
	// if the graph can't be found, one is created
	template<typename T>
	Graph_Intermediary load_optional(const std::string& name, const T& vals,
		const std::string& source_nodeset, bool row_dense,
		const std::string& target_nodeset, bool col_dense,
		const std::string& slice_nodeset = "");

	// gets a typeless graph
	// if the graph can't be found, a null pointer is returned
	Typeless_Graph* get_network(const std::string& name) noexcept;
	
	struct set_of_generators {
		GraphManager* const graph_manager;
		Random* const random;
		set_of_generators(GraphManager* _graph_manager, Random* _random) : graph_manager(_graph_manager), random(_random) { ; }

		template<typename T>
		void dynetml_generator(const dynet::ParameterMap& params, Graph<T>* graph);

		template<typename T>
		void binary_generator_2d(const dynet::ParameterMap& params, Graph<T>* graph);

		template<typename T>
		void binary_generator_3d(const dynet::ParameterMap& params, Graph<std::vector<T> >* graph);

		template<typename T>
		void uniform_generator_2d(const dynet::ParameterMap& params, Graph<T>* graph);

		template<typename T>
		void uniform_generator_3d(const dynet::ParameterMap& params, Graph<std::vector<T> >* graph);

		template<typename T>
		void uniform_generator_3d(const dynet::ParameterMap& params, Graph<std::map<unsigned int, T> >* graph);

		template<typename T>
		void csv_generator_2d(const dynet::ParameterMap& params, Graph<T>* graph);

		template<typename T>
		void csv_generator_3d(const dynet::ParameterMap& params, Graph<std::vector<T> >* graph);

		template<typename T>
		void csv_generator_3d(const dynet::ParameterMap& params, Graph<std::map<unsigned int, T> >* graph);

		template<typename T>
		void perception_generator(const dynet::ParameterMap& params, Graph<std::vector<T> >* graph);

		template<typename T>
		void perception_generator(const dynet::ParameterMap& params, Graph<std::map<unsigned int, T> >* graph);

	};

	set_of_generators generators;


	
#ifdef DEBUG
	const std::set<std::string>& get_accesses(std::string name) const;
#endif // DEBUG

	void import_network(Typeless_Graph* graph);

	void export_network(Typeless_Graph* graph) noexcept;

};


class Construct;


struct CONSTRUCT_LIB Model
{
	Construct* const construct;
	GraphManager* const graph_manager;
	NodesetManager* const ns_manager;
	Random* const random;

	Model(Construct* _construct, const std::string& name);

	Model(const std::string& name);

	virtual ~Model(void) { ; }

	virtual void initialize(void);

	virtual void think(void);

	virtual void update(void);

	virtual void communicate(InteractionMessageQueue::iterator msg);

	virtual void cleanup(void);

	const std::string name;

	bool valid;
};


struct CONSTRUCT_LIB PlaceHolder : virtual public Model {
	PlaceHolder(const std::string& model_name) : Model(model_name) { ; }
};


namespace model_names {
	//Interaction Models

	//"Standard Interaction Model"
	const std::string SIM		= "Standard Interaction Model";
	//"Knowledge Transactive Memory Interaction Model"
	const std::string KTM		= "Knowledge Transactive Memory Interaction Model";
	//"Belief Interaction Model"
	const std::string BELIEF	= "Belief Interaction Model";
	//"Task Interaction Model"
	const std::string TASK		= "Task Interaction Model";
	//"Grand Interaction Model"
	const std::string GRAND		= "Grand Interaction Model";

	//Interaction Models not ME with SIM
	
	//"Location Interaction Model"
	const std::string LOC		= "Location Interaction Model";
	//"Twitter Interaction Model"
	const std::string TWIT		= "Twitter Interaction Model";
	//"Facebook Interaction Model"
	const std::string FB		= "Facebook Interaction Model";

	//Modification Models

	//"Forget Model"
	const std::string FORGET	= "Forgetting Model";
	//"Mail Model"
	const std::string MAIL		= "Mail Model";
	//"Knowledge Learning Difficulty Model"
	const std::string KDIFF		= "Knowledge Learning Difficulty Model";
	//"Template Model"
	const std::string TEMP		= "Template Model";
	//"Subscription Model"
	const std::string SUB		= "Subscription Model";
	//"Knowledge Trust Parsing Model"
	const std::string TRUST		= "Knowledge Trust Model";

	//Parsing Models

	//"Knowledge Parsing Model"
	const std::string KPARSE	= "Knowledge Parsing Model";
	
}

class CONSTRUCT_LIB ModelManager
{
	ModelManager(void) { ; }
	~ModelManager(void);
	
	std::vector<Model*> models;

	friend class Construct;

	bool can_models_be_reorganized = true;	

	void InitializeAllModels(bool verbose_runtime);

	void ThinkAllModels(bool verbose_runtime);

	void UpdateAllModels(bool verbose_runtime);

	void CommunicateAllModels(InteractionMessageQueue::iterator& msg);

	void CleanUpAllModels(bool verbose_runtime);

public:

	void move_model_to_front(Model* model);

	Model* get_model_by_name(const std::string &model_name) noexcept;

	const Model* get_model_by_name(const std::string &model_name) const noexcept;

	void add_model(Model* model) noexcept;
		

};

#include <fstream>

namespace output_names {
	const std::string output_graph = "csv"; //csv
	const std::string output_dynetml = "dynetml"; //dynetml
	const std::string output_messages = "messages"; //messages
}

struct CONSTRUCT_LIB Output {
	virtual ~Output() { ; }

	//this function should be replaced by classes that inheriet from Output
	virtual void process(unsigned int t) = 0;
};


class CONSTRUCT_LIB OutputManager {

	std::vector<Output*> _output;
	friend class Construct;
	void ProcessAllOutput(unsigned int t);

	OutputManager(void) { ; };

	~OutputManager(void);
public:
	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced

	void add_output(Output* output) noexcept;
};


class CONSTRUCT_LIB Output_Graph : public Output {

	static const std::string timeperiods; //"timeperiods"
	static const std::string network_name; //"network name"
	static const std::string output_file; //"output file"

	const Typeless_Graph* _graph;
	std::ofstream _output_file;

	unsigned int _tmax;

	enum class output_types : char {
		initial_only,
		init_and_last,
		all
	};

	output_types output_type = output_types::initial_only;

	
	
	~Output_Graph(void);

	friend class OutputManager;
	void process(unsigned int t);
public:

	Output_Graph(const dynet::ParameterMap& params, Construct* construct);
};


class CONSTRUCT_LIB Output_dynetml : public Output {
	
	static const std::string network_name; //"network names"
	static const std::string output_file; //"output file"
	static const std::string timeperiods; //"timeperiods"

	std::vector<const Typeless_Graph*> _graphs;
	std::ofstream _output_file;

	enum class output_types : char {
		initial_only,
		init_and_last,
		all
	};

	unsigned int _tmax;
	std::vector<std::string> _column_names;
	std::string nodeset_out = "<nodes>";
	output_types output_type = output_types::initial_only;

	
	
	~Output_dynetml(void);

	void process(void);

	inline void add_output(const Graph<bool>* g);

	template<typename T>
	void add_output(const Graph<T>* g);

	friend class OutputManager;
	void process(unsigned int t);
public:

	Output_dynetml(const dynet::ParameterMap& params, Construct* construct);
};


class CONSTRUCT_LIB Output_Messages : public Output {

	

	static const std::string output_file; //"output file"

	

	InteractionMessageQueue* queue;
	std::ofstream _output_file;
	
	~Output_Messages(void);

	void item_out(InteractionMessage::iterator& item, std::string tabs);
	void msg_out(InteractionMessageQueue::iterator& msg, std::string tabs);

	friend class OutputManager;
	void process(unsigned int t);
public:

	Output_Messages(const dynet::ParameterMap& params, Construct* construct);
};


class CONSTRUCT_LIB Construct {
	void _run(void);
public:

	Construct();

	~Construct() {}

	void load_from_xml(const std::string& fname);
	
	bool run();

	// The manager that produces random variables and ensures exact reproduction of results given a random seed.
	Random random;

	// The manager that holds all nodesets.
	NodesetManager ns_manager;

	// The manager that holds all graphs/networks.
	GraphManager graph_manager;

	// The manager that holds all the models and executes all of their various functions.
	ModelManager model_manager;

	// The manager that holds all outputs.
	OutputManager output_manager;

	
	// central message queue for messages waiting to be dispersed.
	InteractionMessageQueue interaction_message_queue;

	//directory where all output gets sent.
	std::string working_directory = "";
	
	//current index in the time nodeset aka current time step
	unsigned int current_time = 0;

	//size of the time nodeset and total number of time periods
	//current_time increases up to but not including this value.
	unsigned int time_count = 1;

	// Set to true if a verbose initialization is requested.
	bool verbose_initialization = false;

	// Set to true if a verbose runtime is requested.
	bool verbose_runtime = false;

private:

	bool running = false;
};



struct CONSTRUCT_LIB KnowledgeParsing : public Model
{
public:

	//graph name - "knowledge network"
	//agent x knowledge
	Graph<bool>& knowledge_net = graph_manager->load_required(graph_names::knowledge, nodeset_names::agents, nodeset_names::knowledge);

	KnowledgeParsing(Construct* _construct) : Model(_construct, model_names::KPARSE) {}

	void communicate(InteractionMessageQueue::iterator msg);
};


class CONSTRUCT_LIB KnowledgeTrust : public Model
{
	const std::string relax_rate = "relax rate";
public:

	float rr;

	//graph name - "knowledge network"
	//agent x knowledge
	const Graph<bool>& knowledge_net = graph_manager->load_required(graph_names::knowledge, nodeset_names::agents, nodeset_names::knowledge);

	//graph name - "knowledge trust network"
	//agent x knowledge
	Graph<float>& knowledge_trust_net = graph_manager->load_optional(graph_names::k_trust, 0.5f, 
		nodeset_names::agents, knowledge_net.row_dense, nodeset_names::knowledge, knowledge_net.col_dense);

	//graph name - "knowledge trust transactive memory network"
	//agent x agent x knowledge
	Graph<std::map<unsigned int, float> >& kttm = graph_manager->load_optional(graph_names::kttm, std::map<unsigned int, float>(),
		nodeset_names::agents, true, nodeset_names::agents, false, nodeset_names::knowledge);

	

	KnowledgeTrust(const dynet::ParameterMap& parameters, Construct* construct);

	void initialize(void);

	void update(void);

	void communicate(InteractionMessageQueue::iterator msg);

	void cleanup(void);
};



class CONSTRUCT_LIB StandardInteraction : public virtual Model
{
public:

	StandardInteraction(const dynet::ParameterMap& parameters, Construct* construct);

	//adds the knowledge parsing model to the models list
	virtual void initialize(void);

	//sets interaction probabilities, creates messages, and adds them to construct's interaction queue
	virtual void think(void);

	//creates all the communication mediums based on the medium nodeset
	void initialize_communication_mediums();

	//sets all interaction probability network links
	virtual void set_probabilities(void);

	//forms interaction pairs based on the interaction probability network, mediums, and available knowledge.
	virtual void add_messages(void);


	//sets the corresponding element where agent_i corresponds to source index
	virtual void set_interaction_probability(unsigned int agent_i, unsigned int agent_j);

	// Grabs the send_msg_queue and decrements all of their time_to_live values
	// any msg that has gotten to zero gets added to the private message queue
	// this should be called before populating the send_msg_queue during interactions
	virtual void update_send_msg_queue();
	
	// generates a vector of paired values with first being the agent index
	// and second being the associated initiation/reception count
	virtual void initialize_interaction_lists(std::vector<std::pair<unsigned int, unsigned int> > &initiators, std::vector<std::pair<unsigned int,unsigned int> > &receivers);

	//gets the index in the initiators and receivers list of the interaction partners found
	std::pair<unsigned int, unsigned int> get_interaction_pair_index(std::vector<std::pair<unsigned int, unsigned int> >& initiators, std::vector<std::pair<unsigned int, unsigned int> >& receivers);

	// gets the communication medium based on the shared medium between sender and receiver
	const CommunicationMedium* get_comm_medium(unsigned int sender_index, unsigned int receiver_index);

	// uses the proximity networks and proximity weight networks to calculate a proximity
	// proximity is based on the perspective of the sender
	virtual float get_prox(unsigned int sender_index, unsigned int receiver_index);

	// compares the sender and receiver for how similar their knowledge is
	virtual float get_k_sim(unsigned int sender_index, unsigned int receiver_index);

	// finds how much knowledge the sender can send to the receiver
	virtual float get_k_exp(unsigned int sender_index, unsigned int receiver_index);

	// returns zero
	virtual float get_additions(unsigned int sender_index, unsigned int receiver_index) { return 0; }

	//given a sender, receiver, and medium, returns the vector of InteractionItems to be attached to an InteractionMessage
	virtual std::vector<InteractionItem> get_interaction_items(unsigned int sender, unsigned int receiver, const CommunicationMedium* comm);

	const Nodeset* agents = ns_manager->get_nodeset(nodeset_names::agents);
	const Nodeset* knowledge = ns_manager->get_nodeset(nodeset_names::knowledge);
	const Nodeset* comm = ns_manager->get_nodeset(nodeset_names::comm);
	const Nodeset* time = ns_manager->get_nodeset(nodeset_names::time);

	//The only required network

	//graph name - "knowledge network"
	//agent x knowledge
	Graph<bool>& knowledge_net = graph_manager->load_required(graph_names::knowledge, agents, knowledge);


	//These graphs are used to control which agents can interact with which other agents.

	//graph name - "sphere of influence network"
	//agent x agent
	Graph<bool>& soi = graph_manager->load_optional(graph_names::soi, true, agents, false, agents, false);
	//graph name - "agent active timeperiod network"
	//agent x timeperiod
	const Graph<bool>& agent_active = graph_manager->load_optional(graph_names::active, true, agents, false, time, false);
	//graph name - "agent initiation count network"
	//agent x timeperiod
	const Graph<unsigned int>& agent_initiation_count = graph_manager->load_optional(graph_names::init_count, 1u, agents, false, time, false);
	//graph name - "agent reception count network"
	//agent x timeperiod
	const Graph<unsigned int>& agent_reception_count = graph_manager->load_optional(graph_names::recep_count, 1u, agents, false, time, false);


	//these graphs are for internal use, not expected as input and are only used for output
	//if they are inputted, the networks get cleared at the beginning of each time step


	//graph name - "interaction probability network"
	//agent x agent
	Graph<float>& interaction_prob = graph_manager->load_optional(graph_names::interact_prob, 0.0f, agents, true, agents, true);
	//graph name - "interaction network"
	//agent x agent
	Graph<bool>& interactions = graph_manager->load_optional(graph_names::interact, false, agents, false, agents, false);

	//These graphs are used to determine how messages are constructed given an interaction pair



	//graph name - "knowledge message complexity network"
	//agent x timeperiod
	const Graph<unsigned int>& knowledge_message_complexity = graph_manager->load_optional(graph_names::k_msg_complex, 1u, agents, false, time, false);
	//graph name - "knowledge priority network"
	//agent x knowledge
	const Graph<float>& knowledge_priority_network = graph_manager->load_optional(graph_names::k_priority, 1.0f, agents, false, knowledge, false);
	//graph name - "learnable knowledge network"
	//agent x knowledge
	const Graph<bool>& learnable_knowledge_net = graph_manager->load_optional(graph_names::learnable_k, true, agents, false, knowledge, false);
	//graph name - "communication medium preferences network"
	//agent x medium
	const Graph<float>& comm_medium_pref = graph_manager->load_optional(graph_names::comm_pref, 1.0f, agents, false, comm, false);
	//graph name - "medium knowledge network"
	//medium x knowledge
	const Graph<bool>& medium_knowledge_access = graph_manager->load_optional(graph_names::medium_k_access, true, comm, false, knowledge, false);
	//graph name - "communication medium access network"
	//agent x CommunicationMedium
	const Graph<bool>& comm_access = graph_manager->load_optional(graph_names::comm_access, true, agents, false, comm, false);

	//These graphs determine proximity which is assumed to be static.



	//graph name - "physical proximity network"
	//agent x agent
	const Graph<float>& physical_prox = graph_manager->load_optional(graph_names::phys_prox, 1.0f, agents, false, agents, false);
	//graph name - "physical proximity weight network"
	//agent x timeperiod
	const Graph<float>& physical_prox_weight = graph_manager->load_optional(graph_names::phys_prox_wgt, 1.0f, agents, false, time, false);
	//graph name - "social proximity network"
	//agent x agent
	const Graph<float>& social_prox = graph_manager->load_optional(graph_names::soc_prox, 1.0f, agents, false, agents, false);
	//graph name - "social proximity weight network"
	//agent x timeperiod
	const Graph<float>& social_prox_weight = graph_manager->load_optional(graph_names::soc_prox_wgt, 1.0f, agents, false, time, false);
	//graph name - "sociodemographic proximity network"
	//agent x agent
	const Graph<float>& socdem_prox = graph_manager->load_optional(graph_names::dem_prox, 1.0f, agents, false, agents, false);
	//graph name - "sociodemographic proximity weight network"
	//agent x timeperiod
	const Graph<float>& socdem_prox_weight = graph_manager->load_optional(graph_names::dem_prox_wgt, 1.0f, agents, false, time, false);

	//These graphs indicate how important various pieces are for determining interaction probability



	//graph name - "interaction knowledge weight network"
	//agent x knowledge
	const Graph<float>& knowledge_importance = graph_manager->load_optional(graph_names::interact_k_wgt, 1.0f, agents, false, knowledge, false);
	//graph name - "knowledge similarity weight network"
	//agent x timeperiod
	const Graph<float>& knowledge_sim_weight = graph_manager->load_optional(graph_names::k_sim_wgt, 1.0f, agents, false, time, false);
	//graph name - "knowledge expertise weight network"
	//agent x timeperiod
	const Graph<float>& knowledge_exp_weight = graph_manager->load_optional(graph_names::k_exp_wgt, 1.0f, agents, false, time, false);


	//delayed messages waiting to be sent are here
	InteractionMessageQueue send_msg_queue; 

	
	//The communication mediums derived from the medium nodeset
	std::vector<CommunicationMedium> communication_mediums;
	//size of the agent nodeset
	unsigned int agent_count;
	//size of the knowledge nodeset
	unsigned int knowledge_count;
	//number of times a try for an interaction pair can happen before interaction pairing prematurely ends
	unsigned int interaction_rejection_limit;

	const unsigned int& t;

	//used to prevent multiple instances of rejection limit being printed
	static bool needs_printing;
};



class CONSTRUCT_LIB Beliefs : public StandardInteraction {
public:

	Beliefs(const dynet::ParameterMap& parameters, Construct* construct);

	//graph name - "belief network"
	//agent x belief
	Graph<float>& beliefs = graph_manager->load_required(graph_names::beliefs, nodeset_names::agents, nodeset_names::belief);

	//graph name - "belief knowledge weight network"
	//belief x knowledge
	const Graph<float>& belief_weights = graph_manager->load_optional(graph_names::b_k_wgt, 1.0f, nodeset_names::belief, false, nodeset_names::knowledge, false);

	//graph name - "belief similarity weight network"
	//agent - timeperiod
	const Graph<float>& belief_sim_weight = graph_manager->load_optional(graph_names::b_sim_wgt, 1.0f, agents, false, time, false);

	void initialize(void);
	void cleanup(void);

	float get_additions(unsigned int agent_i, unsigned int agent_j);
	float get_belief_sim(unsigned int agent_i, unsigned int agent_j);

	float get_belief_value(unsigned int agent_i, unsigned int belief);
};


struct CONSTRUCT_LIB Tasks: public StandardInteraction
{
		const Nodeset* tasks = ns_manager->get_nodeset(nodeset_names::task);

		// graph name - "task assignment network"
		// agent x task
		const Graph<bool>& task_assignment = graph_manager->load_optional(graph_names::task_assignment, true, agents, false, tasks, false);

		// graph name - "task knowledge requirement network"
		// task x knowledge
		const Graph<bool>& tk_req = graph_manager->load_required(graph_names::task_k_req, tasks, knowledge);

		// graph name - "knowledge importance network"
		// task x knowledge
		const Graph<float>& tk_import = graph_manager->load_optional(graph_names::task_k_importance, 1.0f, tasks, false, knowledge, false);

		// graph name - "task guess probability network"
		// task x knowledge
		const Graph<float>& tk_prob = graph_manager->load_optional(graph_names::task_guess_prob, 0.0f, tasks, false, knowledge, false);

		// graph name - "completed tasks network"
		// agent x task
		Graph<unsigned int>& task_completion = graph_manager->load_optional(graph_names::task_completion, 0u, agents, false, tasks, false);

		// graph name - "task availability network"
		// task x timeperiod
		const Graph<bool>& availablity = graph_manager->load_optional(graph_names::task_availability, true, tasks, false, time, false);


		Tasks(const dynet::ParameterMap& parameters, Construct* construct);

		void initialize(void);
		void cleanup(void);

		float get_k_exp(unsigned int agent_i, unsigned int agent_j);
};


class CONSTRUCT_LIB KnowledgeTransactiveMemory : public StandardInteraction
{
	const std::string send_ktm = "can send knowledgeTM";
	const std::string recv_ktm = "can receive knowledgeTM";
public:

	//graph name - "transactive knowledge message complexity network"
	//agent x timeperiod
	const Graph<unsigned int>& tmk_message_complexity = graph_manager->load_optional(graph_names::ktm_msg_complex, 1u, agents, false, time, false);

	//graph name - "agent group membership network"
	//agent x agentgroup
	const Graph<bool>* group_membership;

	//graph name "agent group knowledge network"
	//agentgroup x knowledge
	Graph<float>* group_knowledge;

	//graph name "knowledge transactive memory network"
	//agent x agent x knowledge
	Graph<std::vector<bool> >& tmk = graph_manager->load_optional(graph_names::ktm, std::vector<bool>(agents->size(), false), agents, true, knowledge, false);

	unsigned int group_count;

	const Nodeset* agents;

	std::vector<unsigned int> group_size;
	std::vector<float> generalized_other;
	

	KnowledgeTransactiveMemory(const dynet::ParameterMap& parameters, Construct* construct);
	
	void initialize(void);
	void communicate(InteractionMessageQueue::iterator msg);
	void cleanup();

	std::vector<InteractionItem> get_interaction_items(unsigned int sndr, unsigned int recv, const CommunicationMedium* comm);
	float get_k_sim(unsigned int agent_i,unsigned int agent_j);
	float get_k_exp(unsigned int agent_i, unsigned int agent_j);
	void update_group_knowledge(void);

};


class CONSTRUCT_LIB Social_Media_no_followers : public virtual Model
{


    //model parameter name who's value gets entered into Social_Media_with_followers::dt
    const std::string interval_time_duration = "interval time duration";

    //model parameter name who's value gets entered into Social_Media_with_followers::age
    const std::string maximum_post_inactivity = "maximum post inactivity";
public:

    struct CONSTRUCT_LIB media_event {

        //this goes through the entire chain of events recursively and updates the last_used
        //to the most recent time. Source call is only on the root event.
        void update_chain(float time);

        enum class event_type : char
        {
            post,
            repost,
            quote,
            reply
        };


        ~media_event();

        //this constructor is only for creating post events
        media_event(
            unsigned int        _user,
            unsigned int        _knowledge_index,
            float               _ktrust,
            float               _time_stamp
        );

        //this constructor will create a reply/quote to the submitted parent event
        media_event(
            event_type         _type,
            unsigned int        _user,
            float               _ktrust,
            float               _time_stamp,
            media_event* _event
        );

        //this constructor will create a repost which uses the same knowledge and trust as the submitted parent event 
        media_event(
            unsigned int _user,
            float        _time_stamp,
            media_event* _event
        );

        //default constructor does no operations
        media_event();

        media_event& operator=(const media_event& other);

        //gets the size of the tree of events with this event at its root (minimum size of 1)
        unsigned int child_size(void);

        //the pointer of the event this event is replying to.
        //if equal to this, the event has no parent and is a post.
        media_event* parent_event = this;

        //the pointer of the event that is at the root of the event tree that this event is apart of.
        //if equal to this, the event is the root event for the tree.
        media_event* root_event = this;

        //set of indexes the event contains
        std::unordered_map<InteractionItem::item_keys, unsigned int> indexes;

        //set of values the event contains
        std::unordered_map<InteractionItem::item_keys, float> values;

        //list of reposts that have shared this event
        std::vector<media_event*> reposts;

        //list of replies to this event
        std::vector<media_event*> replies;

        //list of quotes to this event
        std::vector<media_event*> quotes;

        //list of users mentioned by this event
        std::vector<unsigned int> mentions;

        //agent index of the user that posted the event
        unsigned int user = 0;

        //the time that this event was created
        float time_stamp = -1;

        //the last time any event was added to the event tree this event is apart of
        //all events in the same event tree have equal values for last_used
        float last_used = -1;

        //how trending an event is
        float score = 1;

        bool operator== (const media_event& a) const { return score == a.score; }
        bool operator!= (const media_event& a) const { return score != a.score; }
        bool operator<= (const media_event& a) const { return score <= a.score; }
        bool operator>= (const media_event& a) const { return score >= a.score; }
        bool operator< (const media_event& a) const { return score < a.score; }
        bool operator> (const media_event& a) const { return score > a.score; }

        //what type of event this is i.e. "post","repost", "quote", or "reply"
        event_type type = event_type::post;
    };


    //class that contains all settings for a user as well as functions that dictates how each user interacts
    struct CONSTRUCT_LIB media_user {

        virtual ~media_user() { ; }

        // called before reply, quote, or repost, allows the user to parse an event before responding to it
        virtual void read(media_event* read_event) = 0;

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
    };


    struct CONSTRUCT_LIB default_media_user : public virtual media_user {

        default_media_user(Social_Media_no_followers* _media, Nodeset::iterator node);

        //the social media that this user is interacting with
        Social_Media_no_followers* media;

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
        void read(media_event* _event);

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

        //mentions are added to the event if the event is a post by randomly selecting a followee
        virtual void add_mentions(media_event* post);

        //gets the trust of the knowledge index
        //if the "Knowledge Trust %Model" is not active, -1 is returned.
        virtual float get_trust(unsigned int knowledge_index);
    };


    class CONSTRUCT_LIB event_container : public std::list<media_event> {
        using std::list<media_event>::insert;
        using std::list<media_event>::push_back;
        using std::list<media_event>::emplace_back;
        using std::list<media_event>::emplace;
        using std::list<media_event>::swap;
        using std::list<media_event>::sort;
        using std::list<media_event>::reverse;
        using std::list<media_event>::merge;
    public:
        
        std::vector<media_event> removed_events;

        iterator erase(const const_iterator _Where) noexcept {
            removed_events.push_back(*_Where);
            return list::erase(_Where);
        }

        iterator erase(const const_iterator _First, const const_iterator _Last) noexcept {
            for (auto temp = _First; temp != _Last; temp++) removed_events.push_back(*temp);
            return list::erase(_First, _Last);
        }

        void clear() noexcept {
            for (auto it = begin(); it != end(); ++it) {
                removed_events.push_back(*it);
            }
            list::clear();
        }

        template<class... Args>
        void emplace_front(Args&&... args) {

            float prev_event_time = 0;
            if (size()) prev_event_time = front().time_stamp;
            std::list<media_event>::emplace_front(args...);
            if (size()) check_new_event(prev_event_time, front().time_stamp);
        }

        void push_front(const value_type& val) {
            check_new_event(front().time_stamp, val.time_stamp);
            std::list<media_event>::push_front(val);
        }

        void check_new_event(float previous_time_stamp, float new_time_stamp);
    };


    void check_list_order() const;

    //pointer to the "agent" nodeset
    const Nodeset* agents = ns_manager->get_nodeset(nodeset_names::agents);

    //pointer to the Knowledge Trust %Model if it has been created
    //otherwise the pointer remains a null pointer
    KnowledgeTrust* TRUST = nullptr;

    //this is the medium used for all messages created by this model
    //it is intended to have unlimited complexity and avoid models that interact based on medium
    CommunicationMedium medium;

    //this key is added to messages created by this model for items that contain the feed index
    InteractionItem::item_keys event_key = InteractionItem::item_keys::twitter_event;

    // list of all current active events, all users can access this list
    // new events should be added to the front of this list
    event_container list_of_events;

    //contains each user's feed of events pseudo-sorted by priority, also contains user-centric event info like whether a event has been read
    std::vector<std::vector<media_event*> > users_feed;

    // how many posts each agent reads each time step
    std::vector<unsigned int> read_count;

    //the maximum time a post can exist without its tree being added to
    float age;

    //the time duration between time steps
    float dt;

    //current time period
    unsigned int& time;

    //prefix for some of the node attributes names parsed by the media_user class
    std::string media_name;

    //graphs already used by other models in construct

    //graph name - "knowledge network"
    //agent x knowledge
    const Graph<bool>& knowledge_net = graph_manager->load_required(graph_names::knowledge, agents, ns_manager->get_nodeset(nodeset_names::knowledge));

    //graph name - "agent active time network"
    //agent x timeperiod
    const Graph<bool>& active = graph_manager->load_optional(graph_names::active, true, agents, false, ns_manager->get_nodeset(nodeset_names::time), false);

    //list of users
    std::vector<media_user*> users;

    //Loads all nodesets and graphs for this model and checks to ensure all required node attributes are present
    //Loads the parameters "interval time duration" into dt and "maximum post inactivity" into age
    //Uses the API function create_social_media_user to populate Social_Media_with_followers::users
    Social_Media_no_followers(const std::string& _media_name, const dynet::ParameterMap& parameters, Construct* _construct);

    //delete all pointers in stored in the Social_Media_with_followers::users data structure
    virtual ~Social_Media_no_followers();

    virtual void load_users();

    //agents read events in their feeds starting with the first event
    //reading an event will create a message with all relavant knowledge and trust information in items along with the event's feed index
    //messages are sent from the read event's author to the reading user and uses a CommunicationMedium with maximum complexity
    void think(void);

    //adds the Knowledge Parsing %Model, and attempts to find and save the pointer for the Knowledge Trust %Model if it has been added to the model list
    void initialize(void);

    //only parses messages that have an attribute equal to Social_Media_with_followers::event_key for the feed position index corresponding to a media_event pointer
    //that pointer is then given to media_user::read and if the user already knows the knowledge the event is passed to media_user::(reply, quote, repost)
    //the reading user will then decide whether to follow based on the event author's charisma and similarity based on the trust transactive memory
    void communicate(InteractionMessageQueue::iterator msg);

    //feeds are updated, the social media will recommend users to follow, and users can decide to unfollow other users
    void cleanup(void);

    //appends the array of InteractionItems based on the submitted event and the intended receiver of the message
    virtual void append_message(media_event* _event, InteractionMessage& msg);

    //updates each user's feeds with the new events created during the time step while also discarding read events from the feed
    //events are ordered by direct replies or mentions, events of followers, and all other events
    //within each category events are sorted based on media_event::score which is set to media_event::child_size * media_event::time_stamp
    //after the events have been organized stochastic shuffling is done on 10% of the feed to avoid a fully deterministic feed
    virtual void update_feeds(void);
};



struct CONSTRUCT_LIB Social_Media_with_followers : public virtual Social_Media_no_followers
{
public:

    //class that contains all settings for a user as well as functions that dictates how each user interacts
    struct CONSTRUCT_LIB media_user : virtual public Social_Media_no_followers::media_user {

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


    struct CONSTRUCT_LIB default_media_user : public Social_Media_no_followers::default_media_user, public media_user  {

        default_media_user(Social_Media_with_followers* _media, Nodeset::iterator node);

        //the social media that this user is interacting with
        Social_Media_with_followers* media;

        //probability density to recommend followers (time in hours) pdaf * dt = average number of recommendations in a time period.
        float pdaf;

        //scale factor to determine number of removed followees
        float rf;

        //determines how likable someone's event is going to be.
        //people with more likable posts get more followers
        float charisma;

        //If true, this user, when added as a followee by another user, will automatically reciprocate followings
        bool auto_follow;

        //mentions are added to the event if the event is a post by randomly selecting a followee
        virtual void add_mentions(media_event* post);

        //returns true if this user decides to follow an agent when called
        bool follow_user(unsigned int alter_agent_index);

        //Returns true if this user decides to unfollow an agent when called
        bool unfollow_user(unsigned int alter_agent_index);

        // An alter has decided to follow this user and can decide to reciprocate that following
        bool respond_to_follow(unsigned int alter_agent_index);

        // How many alters should be considered each time step for recommendations
        unsigned int consider_recommendations(void);

        float get_charisma();
    };


    std::vector < std::vector<unsigned int> > responses;

    //twittersim specific graphs

    //graph name - TBD
    //agent x agent
    // if (follower_net->examine(i,j)) // agent i is following agent j
    Graph<bool>* follower_net = nullptr;

    //list of users
    std::vector<media_user*> users;

    //Loads all nodesets and graphs for this model and checks to ensure all required node attributes are present
    //Loads the parameters "interval time duration" into dt and "maximum post inactivity" into age
    //Uses the API function create_social_media_user to populate Social_Media_with_followers::users
	Social_Media_with_followers(const std::string& _media_name, const dynet::ParameterMap& parameters, Construct* _construct);

    virtual void load_users();

    void communicate(InteractionMessageQueue::iterator msg);

    //feeds are updated, the social media will recommend users to follow, and users can decide to unfollow other users
    void cleanup(void);

    //computes the Jaccard Similarity in the follower network between the two agent indexes
    float follower_jaccard_similarity(unsigned int agent_i, unsigned int agent_j) const;

    //function gives each user recommended users to follow
    virtual void add_followees(void);

    //each users decides whether to unfollow any other user
    virtual void remove_followees(void);

    //updates each user's feeds with the new events created during the time step while also discarding read events from the feed
    //events are ordered by direct replies or mentions, events of followers, and all other events
    //within each category events are sorted based on media_event::score which is set to media_event::child_size * media_event::time_stamp
    //after the events have been organized stochastic shuffling is done on 10% of the feed to avoid a fully deterministic feed
    virtual void update_feeds(void);
};


struct CONSTRUCT_LIB Facebook : public virtual Social_Media_with_followers {
    Facebook(const dynet::ParameterMap& parameters, Construct* construct);
};



struct CONSTRUCT_LIB Twitter : public virtual Social_Media_with_followers {
    Twitter(const dynet::ParameterMap& parameters, Construct* construct);
};




struct CONSTRUCT_LIB Forget : public Model
{
	const Nodeset* agents;

	//graph name - "knowledge network"
	//agent x knowledge
	Graph<bool>* knowledge_net = 0;

	//graph name - "knowledge strength network"
	//agent x knowledge
	Graph<float>* knowledge_strength = 0;

	//graph name - "knowledge forgetting rate network"
	//agent x knowledge
	const Graph<float>* knowledge_forget_rate = 0;

	//graph name - "knowledge forgetting prob network"
	//agent x knowledge
	const Graph<float>* knowledge_forget_prob = 0;

	//graph name - "knowledge trust network"
	//agent x knowledge
	Graph<float>* ktrust_net = 0;

	//sparse matrix of users who used a piece of knowledge
	//gets reset each time step
	//not intended to get input any input will be cleared at the beginning of each time step
	Graph<bool>* unused_knowledge = 0;

	Forget(Construct* construct);
	void think(void);
	void communicate(InteractionMessageQueue::iterator msg);
	void cleanup(void);

};


class CONSTRUCT_LIB GrandInteraction : public StandardInteraction
{
	//model paramters - not all are required

	const std::string beliefs_enabled = "beliefs enabled"; //"beliefs enabled"
	const std::string beliefTM_enabled = "belief transactive memory enabled"; //"belief transactive memory enabled"
	const std::string tasks_enabled = "tasks enabled"; //"tasks enabled"
	const std::string ktm_enabled = "knowledge transactive memory enabled"; //"knowledge transactive memory enabled"
	const std::string belief_roc = "belief rate of change"; //"belief rate of change"

	struct triplet {
		triplet(unsigned int _alter, unsigned int _index, float _belief) : alter(_alter), index(_index), belief(_belief) { ; }
		unsigned int alter = 0;
		unsigned int index = 0;
		float belief = 0;
	};


public:

	KnowledgeTransactiveMemory* TMK = 0;

	Beliefs* BM = 0;

	//the following is for belief transactive memory
	//for more information on btm see DOI : 10.1109 / TSMCC.2012.2230255

	bool _btm_enabled = false;

	//graph name - "belief message complexity network"
	//agent x timeperiod
	const Graph<unsigned int>* belief_message_complexity = 0;

	//graph name - "transactive belief message complexity network"
	//agent x timeperiod
	const Graph<unsigned int>* btm_message_complexity = 0;

	//graph name - "belief transactive memory network"
	//agent x agent x belief
	Graph<std::map<unsigned int, float> >* btm = 0;

	//graph name - "group belief network"
	//agentgroup x belief
	Graph<float>* _group_beliefs = 0;

	float alpha = 0;

	Tasks* TASK = 0;



	std::vector<float> generalized_other_beliefs;
	std::vector<float> generalized_other_tasks;
	std::vector<unsigned int> group_sizes;



	//graph name - "agent group membership network"
	//agent x agentgroup
	Graph<bool>* group_membership = 0;



	GrandInteraction(const dynet::ParameterMap& parameters, Construct* construct);
	void initialize(void);
	void communicate(InteractionMessageQueue::iterator msg);
	void cleanup(void);

	//see Beliefs::think for why these functions are here
	float get_k_sim(unsigned int initiator, unsigned int receiver);
	float get_k_exp(unsigned int initiator, unsigned int receiver);
	float get_belief_sim(unsigned int initiator, unsigned int receiver);
	float get_additions(unsigned int agent_i, unsigned int agent_j);
	std::vector<InteractionItem> get_interaction_items(unsigned int initiator, unsigned int receiver, const CommunicationMedium* comm);

	void update_group_belief(void);
	unsigned int get_k_intersect(unsigned int initiator, unsigned int receiver);



	
};


class CONSTRUCT_LIB KnowledgeLearningDifficulty : public Model
{
public:

	//sifts through interaction queue and removes items that are too difficult to learn
	//if a message loses all of items, it is removed from the queue
	void update(void);

	KnowledgeLearningDifficulty(Construct* _construct) : Model(_construct, model_names::KDIFF) {}

	//graph name - "knowledge learning difficulty network"
	//agent x knowledge
	const Graph<float>& k_leanrning_difficulty_net = graph_manager->load_required(graph_names::k_diff, nodeset_names::agents, nodeset_names::knowledge);

};


class CONSTRUCT_LIB Location : public Model
{
public:

	const Nodeset* agents = ns_manager->get_nodeset(nodeset_names::agents);
	const Nodeset* knowledge = ns_manager->get_nodeset(nodeset_names::knowledge);
	const Nodeset* locations = ns_manager->get_nodeset(nodeset_names::loc);

	// graph name - "location network"
	// location x location 
	const Graph<bool>& loc_adj = graph_manager->load_optional(graph_names::location_network, true, locations, false, locations, false);

	// graph name - "agent current location network"
	// agent x location
	Graph<bool>& current_loc = graph_manager->load_required(graph_names::current_loc, agents, locations);

	// graph name - "location knowledge network"
	// location x knowledge
	const Graph<bool>& location_knowledge_net = graph_manager->load_optional(graph_names::loc_knowledge, true, locations, false, knowledge, false);

	// graph name - "agent location learning rate network"
	// agent x location
	const Graph<float>& agent_location_learning_rate_net = graph_manager->load_optional(graph_names::loc_learning_rate, 1.0f, agents, false, locations, false);

	// graph name - "knowledge expertise weight network"
	// agent x timperiod
	const Graph<float>& knowledge_expertise = graph_manager->load_optional(graph_names::k_exp_wgt, 1.0f, agents, false, ns_manager->get_nodeset(nodeset_names::time), false);

	// graph name - "location preference network"
	// agent x location
	const Graph<float>& location_preference = graph_manager->load_optional(graph_names::loc_preference, 1.0f, agents, false, locations, false);

	// graph name - "location medium access network"
	// location x CommunicationMedium
	const Graph<bool>& location_medium_access = graph_manager->load_optional(graph_names::loc_medium_access, true, locations, false, ns_manager->get_nodeset(nodeset_names::comm), false);

	// graph name - "knowledge network"
	// agent x knowledge
	const Graph<bool>& knowledge_net = graph_manager->load_required(graph_names::knowledge, agents, knowledge);

	//graph name - "interaction knowledge weight network"
	//agent x knowledge
	const Graph<float>& knowledge_importance = graph_manager->load_optional(graph_names::interact_k_wgt, 1.0f, agents, false, knowledge, false);

	//graph name - "agent location learning limit network"
	//agent x location
	const Graph<unsigned int>& loc_limit = graph_manager->load_optional(graph_names::loc_learning_limit, 1u, agents, false, locations, false);

	//graph name - "agent active timperiod network"
	//agent x timeperiod
	const Graph<bool>& active = graph_manager->load_optional(graph_names::active, true, agents, false, ns_manager->get_nodeset(nodeset_names::time), false);

	//graph name - "knowledge priority network"
	//agent x knowledge
	const Graph<float>& knowledge_priority_network = graph_manager->load_optional(graph_names::k_priority, 1.0f, agents, false, knowledge, false);

	//graph name - "communication medium access network"
	//agent x CommunicationMedium
	const Graph<bool>& comm_access = graph_manager->load_optional(graph_names::comm_access, true, agents, false, ns_manager->get_nodeset(nodeset_names::comm), false);

	//graph name - "communication medium preferences network"
	//agent x CommunicationMedium
	const Graph<float>& comm_medium_prefs = graph_manager->load_optional(graph_names::comm_pref, 1.0f, agents, false, ns_manager->get_nodeset(nodeset_names::comm), false);

	//unsigned int agent_count;
	//unsigned int location_count;
	//unsigned int knowledge_count;
	std::vector<unsigned int> _current_agent_locations;
	std::vector<CommunicationMedium> communication_mediums;

	Tasks* tasks = 0;

	Location(Construct* construct);
	void initialize(void);
	void think(void);
	void cleanup(void);

	virtual float get_expertise(unsigned int agent, unsigned int location);
	virtual const CommunicationMedium* get_medium(unsigned int agent);
};


class CONSTRUCT_LIB Mail : public Model
{
public:

	//graph name - "agent mail usage by medium"
	//agent x CommunicationMedium
	Graph<float>& mail_pref = graph_manager->load_optional(graph_names::mail_usage, 1.0f, nodeset_names::agents, false, nodeset_names::comm, false);

	//graph name - "mail check probability"
	//agent x timeperiod
	Graph<float>& mail_check_prob = graph_manager->load_optional(graph_names::mail_check_prob, 0.5f, nodeset_names::agents, false, nodeset_names::time, false);

	std::vector<InteractionMessageQueue> mailboxes = std::vector<InteractionMessageQueue>(mail_pref.source_nodeset->size());

	Mail(Construct* _construct) : Model(_construct, model_names::MAIL) {};

	void update(void);
};


struct CONSTRUCT_LIB Subscription : public Model
{
	Subscription(Construct* _construct) : Model(_construct, model_names::SUB) {};


	void think();
	void communicate(InteractionMessageQueue::iterator msg);
	void cleanup();

	//graph name - "public propensity"
	//agent x CommunicationMedium
	const Graph<float>& public_propensity = graph_manager->load_optional(graph_names::propensity, 
		0.01f, nodeset_names::agents, false, nodeset_names::comm, false);
	//graph name - "subscription network"
	//agent x agent
	//if row,column element is true row is subscribed to column
	Graph<bool>& subscriptions = graph_manager->load_optional(graph_names::subs, 
		false, nodeset_names::agents, false, nodeset_names::agents, false);
	//graph name - "subscription probability"
	//agent x agent
	const Graph<float>& sub_prob = graph_manager->load_optional(graph_names::sub_probability, 
		0.01f, nodeset_names::agents, false, nodeset_names::agents, false);

	InteractionMessageQueue public_queue;
};

