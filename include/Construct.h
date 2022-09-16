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


	struct end_early {
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

	struct CONSTRUCT_LIB ostream_wrapper {

		template <typename T>
		ostream_wrapper& operator<<(const T& data) {
#ifndef QUIET
			std::cout << data;
#endif
			return *this;
		}

		ostream_wrapper& operator<<(ostream_wrapper& (*end_line)(ostream_wrapper&)) {
			return end_line(*this);
		}

	};


	
	CONSTRUCT_LIB ostream_wrapper& endl(ostream_wrapper& out);

	// similar behaviour to std::cout except defining QUIET will supress all text output.
	extern ostream_wrapper cout;
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
	const std::string emotions		= "emotion";		//"emotions"
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
		feed_position,
		emotion
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

	template<typename T>
	static constexpr CONSTRUCT_LIB edge_types get_edge_type(void) noexcept;

	template<typename T>
	static constexpr CONSTRUCT_LIB std::string get_type_name(void) noexcept;

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


template<typename link_type>
class Graph;

template<typename T>
std::vector<T> operator+(const std::vector<T>& left, const std::vector<T>& right) {
	assert(left.size() == right.size());
	std::vector<T> ret(left.size());
	for(unsigned int i=0; i<ret.size(); ++i) {
		ret[i]=left[i]+right[i];
	}
	return ret;
}

template<typename T>
std::map<unsigned int, T> operator+(const std::map<unsigned int, T>& left, const std::map<unsigned int, T>& right) {
	std::map<unsigned int, T> ret;
	auto lit = left.begin();
	auto rit = right.begin();
	while(lit != left.end() && rit != right.end()) {
		if(lit->first < rit->first) {
			ret[lit->first] = lit->second;
			++lit;
		}
		else if (lit->first > rit->first) {
			ret[rit->first] = rit->second;
			++rit;
		}
		else {
			ret[rit->first] = rit->second + lit->second;
			++lit;
			++rit;
		}
	}
	while(lit != left.end()) {
		ret[lit->first] = lit->second;
		++lit;
	}
	while(rit != right.end()) {
		ret[rit->first] = rit->second;
		++rit;
	}
	return ret;
}

template<typename link_type>
struct CONSTRUCT_LIB Transpose {
	Graph<link_type>* g;
	Transpose(Graph<link_type>* _g) : g(_g) {}
	Transpose(const Graph<link_type>* _g) : g(const_cast<Graph<link_type>*>(_g)) {}
	Graph<link_type>& graph(void) { return *g; }
	const Graph<link_type>& graph(void) const { return *g; }
};


template<typename link_type>
class CONSTRUCT_LIB Temporary_Graph {
	Graph<link_type>* g;
public:
	Temporary_Graph() : g(nullptr) {}

	Temporary_Graph(const link_type& vals, const Nodeset* src, bool row_dense, const Nodeset* trg, bool col_dense);

	Temporary_Graph(Graph<link_type>* _g) : g(_g) {}
	Temporary_Graph(const Temporary_Graph& _g) : g(_g.g) { const_cast<Temporary_Graph*>(&_g)->g = nullptr; }
	Temporary_Graph& operator=(const Temporary_Graph& _g) {
		g = _g.g;
		const_cast<Temporary_Graph*>(&_g)->g = nullptr;
		return *this;
	}
	~Temporary_Graph() {
		if (g) delete g;
	}

	Graph<link_type>& graph(void) { return *g; }
	const Graph<link_type>& graph(void) const { return *g; }

	Transpose<link_type> T(void) { return Transpose<link_type>(g); }
	const Transpose<link_type> T(void) const { return Transpose<link_type>(g); }
};


// ------------ iterator parents -----------------------------

namespace graph_utils {

	void CONSTRUCT_LIB it_align(std::vector<typeless_graph_iterator*>& it_list);

	void CONSTRUCT_LIB init_align(std::vector<typeless_graph_iterator*>& it_list);

	void CONSTRUCT_LIB it_align_before_first(std::vector<typeless_graph_iterator*>& it_list);

	void CONSTRUCT_LIB init_align_before_first(std::vector<typeless_graph_iterator*>& it_list);

	template<typename link_type>
	struct CONSTRUCT_LIB graph_iterator : public typeless_graph_iterator {
		Graph<link_type>* _parent = NULL;
		graph_iterator(unsigned int row, unsigned int col, const Graph<link_type>* parent, void* ptr) :
			typeless_graph_iterator(row, col, ptr), _parent(const_cast<Graph<link_type>*>(parent)) {}

		virtual const link_type& examine(void) const = 0;


	};


	template<typename link_type>
	struct CONSTRUCT_LIB sparse_graph_iterator {
		const link_type _skip;
		sparse_graph_iterator(const link_type& skip_data) : _skip(skip_data) {}
	};


	template<typename link_type>
	struct CONSTRUCT_LIB row_graph_iterator : public graph_iterator<link_type> {
		using graph_iterator<link_type>::graph_iterator;

		unsigned int index() const noexcept;

		unsigned int max() const noexcept;

		const link_type& operator*(void) const;

		const link_type* operator->(void) const;

		const link_type& examine(void) const { return this->operator*(); }
	};


	template<typename link_type>
	struct CONSTRUCT_LIB col_graph_iterator : public graph_iterator<link_type> {
		using graph_iterator<link_type>::graph_iterator;

		unsigned int index() const noexcept;

		unsigned int max() const noexcept;

		const link_type& operator*(void) const;

		const link_type* operator->(void) const;

		const link_type& examine(void) const { return this->operator*(); }
	};


	template<typename link_type>
	struct CONSTRUCT_LIB const_full_row_iterator : public row_graph_iterator<link_type> {

		using row_graph_iterator<link_type>::row_graph_iterator;
		using row_graph_iterator<link_type>::operator*;
		//using graph_iterator::_parent;

		const const_full_row_iterator& operator++(void) const;
	};


	template<typename link_type>
	struct CONSTRUCT_LIB full_row_iterator : public const_full_row_iterator<link_type> {
		using const_full_row_iterator<link_type>::const_full_row_iterator;
	};


	template<typename link_type>
	struct CONSTRUCT_LIB const_sparse_row_iterator : public row_graph_iterator<link_type>, public sparse_graph_iterator<link_type> {
		const_sparse_row_iterator(unsigned int row, unsigned int col, const Graph<link_type>* parent, void* ptr, const link_type& skip_data) :
			row_graph_iterator<link_type>(row, col, parent, ptr), sparse_graph_iterator<link_type>(skip_data) {
			if (row_graph_iterator<link_type>::operator*() == skip_data) this->operator++();
		}

		using row_graph_iterator<link_type>::operator*;

		const const_sparse_row_iterator& operator++(void) const;
	};


	template<typename link_type>
	struct CONSTRUCT_LIB sparse_row_iterator : public const_sparse_row_iterator<link_type> {
		using const_sparse_row_iterator<link_type>::const_sparse_row_iterator;
	};


	template<typename link_type>
	struct CONSTRUCT_LIB const_full_col_iterator : public col_graph_iterator<link_type> {
		using col_graph_iterator<link_type>::col_graph_iterator;
		using col_graph_iterator<link_type>::operator*;

		const const_full_col_iterator& operator++(void) const;
	};


	template<typename link_type>
	struct CONSTRUCT_LIB full_col_iterator : public const_full_col_iterator<link_type> {
		using const_full_col_iterator<link_type>::const_full_col_iterator;
	};


	template<typename link_type>
	struct CONSTRUCT_LIB const_sparse_col_iterator : public col_graph_iterator<link_type>, public sparse_graph_iterator<link_type> {
		const_sparse_col_iterator(unsigned int row, unsigned int col, const Graph<link_type>* parent, void* ptr, const link_type& skip_data) :
			col_graph_iterator<link_type>(row, col, parent, ptr), sparse_graph_iterator<link_type>(skip_data) {
			if (col_graph_iterator<link_type>::operator*() == skip_data) this->operator++();
		}

		using col_graph_iterator<link_type>::operator*;

		const const_sparse_col_iterator& operator++(void) const;
	};


	template<typename link_type>
	struct CONSTRUCT_LIB sparse_col_iterator : public const_sparse_col_iterator<link_type> {
		using const_sparse_col_iterator<link_type>::const_sparse_col_iterator;
	};


	template<typename link_type>
	struct CONSTRUCT_LIB const_row_begin_iterator : public col_graph_iterator<link_type> {
		const_row_begin_iterator(unsigned int row, const Graph<link_type>* parent, void* ptr) : col_graph_iterator<link_type>(row, 0, parent, ptr) {}

		unsigned int operator*(void) const;

		const_full_row_iterator<link_type> full_begin(void) const;

		const_sparse_row_iterator<link_type> sparse_begin(const link_type& data) const;

		typeless_graph_iterator end(void) const;

		const const_row_begin_iterator& operator++(void) const;

		template<typename other, class output = decltype(other()* link_type())>
		std::vector<output> ewise_product(const std::vector<other>& vec) const {
			assert(vec.size() == this->_parent->col_size);
			std::vector<output> ret = vec;

			for (auto it = full_begin(); it != end(); ++it) {
				ret[it.col()] *= *it;
			}

			return ret;
		}

		template<typename other, class output = decltype(other()* link_type())>
		std::vector<output> ewise_division(const std::vector<other>& vec) const {
			assert(vec.size() == this->_parent->col_size);
			std::vector<output> ret;

			for (auto it = full_begin(); it != end(); ++it) {
				// can't divide by zero
				assert(vec[it.col()] != 0);
				ret[it.col()] = *it / vec[it.col()];
			}

			return ret;
		}

		template<typename other, class output = decltype(other()* link_type())>
		std::map<unsigned int, output> ewise_product(const std::map<unsigned int, other>& vec) const {
			std::map<unsigned int, output> ret = vec;

			auto git = sparse_begin(0);
			auto rit = ret.begin();
			while (git != end() && rit != ret.end()) {
				if (git.col() < rit->first) ++git;
				else if (rit->first < git.col()) rit = ret.erase(rit);
				else {
					rit->second *= *git;
					++rit;
					++git;
				}
			}
			while (rit != ret.end()) rit = ret.erase(rit);

			return ret;
		}

		template<class output = decltype(link_type() + link_type())>
		output sum() const {
			output ret = 0;
			for (auto it = sparse_begin(0); it != end(); ++it) ret += *it;
			return ret;
		}

		std::vector<link_type> get_dense() const {
			std::vector<link_type> ret;
			for (auto it = full_begin(); it != end(); ++it) ret[it.index()] = *it;
			return ret;
		}

		std::map<unsigned int, link_type> get_sparse(const link_type& skip) const {
			std::map<unsigned int, link_type> ret;
			for (auto it = sparse_begin(skip); it != end(); ++it) ret[it.index()] = *it;
			return ret;
		}
	};


	template<typename link_type>
	struct CONSTRUCT_LIB row_begin_iterator : public const_row_begin_iterator<link_type> {
		using const_row_begin_iterator<link_type>::const_row_begin_iterator;
		full_row_iterator<link_type> full_begin(void);

		sparse_row_iterator<link_type> sparse_begin(const link_type& data);
	};


	template<typename link_type>
	struct CONSTRUCT_LIB const_col_begin_iterator : public row_graph_iterator<link_type> {
		const_col_begin_iterator(unsigned int col, const Graph<link_type>* parent, void* ptr) : row_graph_iterator<link_type>(0, col, parent, ptr) {}

		unsigned int operator*(void) const;

		const_full_col_iterator<link_type> full_begin(void) const;

		const_sparse_col_iterator<link_type> sparse_begin(const link_type& data) const;

		typeless_graph_iterator end(void) const;

		const const_col_begin_iterator& operator++(void) const;

		template<typename other, class output = decltype(other()* link_type())>
		std::vector<output> element_wise_product(const std::vector<other>& vec) const {
			assert(vec.size() == this->_parent->row_size);
			std::vector<output> ret = vec;

			for (auto it = full_begin(); it != end(); ++it) {
				ret[it.row()] *= *it;
			}

			return ret;
		}

		template<typename other, class output = decltype(other()* link_type())>
		std::map<unsigned int, output> element_wise_product(const std::map<unsigned int, other>& vec) const {
			std::map<unsigned int, output> ret = vec;

			auto git = sparse_begin(0);
			auto rit = ret.begin();
			while (git != end() && rit != ret.end()) {
				if (git.col() < rit->first) ++git;
				else if (rit->first < git.row()) rit = ret.erase(rit);
				else {
					rit->second *= *git;
					++rit;
					++git;
				}
			}
			while (rit != ret.end()) rit = ret.erase(rit);

			return ret;
		}

		template<class output = decltype(link_type() + link_type())>
		output sum() const {
			output ret = 0;
			for (auto it = sparse_begin(0); it != end(); ++it) ret += *it;
			return ret;
		}

		std::vector<link_type> get_dense() const {
			std::vector<link_type> ret;
			for (auto it = full_begin(); it != end(); ++it) ret[it.index()] = *it;
			return ret;
		}

		std::map<unsigned int, link_type> get_sparse(const link_type& skip) const {
			std::map<unsigned int, link_type> ret;
			for (auto it = sparse_begin(skip); it != end(); ++it) ret[it.index()] = *it;
			return ret;
		}
	};


	template<typename link_type>
	struct CONSTRUCT_LIB col_begin_iterator : public const_col_begin_iterator<link_type> {
		using const_col_begin_iterator<link_type>::const_col_begin_iterator;

		full_col_iterator<link_type> full_begin(void);

		sparse_col_iterator<link_type> sparse_begin(const link_type& data);
	};

}






// -------------------------------------------------------------------------------------------------------------------------------------------------------- Graph


template<typename link_type>
class CONSTRUCT_LIB Graph : public Typeless_Graph {


	
//allows graph and its child classes to be able to access the private members of the iterator classes
//the graph classes should do as much heavy lifting as possible compared to the iterator classes

	struct delta {
		unsigned int _row;
		unsigned int _col;
		link_type _data;
		delta(unsigned int row, unsigned int col, const link_type& data) :
			_row(row), _col(col), _data(data)
		{
		}
	};


	std::vector<delta> _deltas;

	
protected:
	Graph(const Nodeset* src, bool row_dense, const Nodeset* trg, bool col_dense, const Nodeset* slc, const link_type& def, const std::string& name);
public:
	virtual ~Graph() { ; }

	using graph_iterator			= graph_utils::graph_iterator<link_type>;
	using sparse_graph_iterator		= graph_utils::sparse_graph_iterator<link_type>;
	using row_graph_iterator		= graph_utils::row_graph_iterator<link_type>;
	using col_graph_iterator		= graph_utils::col_graph_iterator<link_type>;

	using const_full_row_iterator	= graph_utils::const_full_row_iterator<link_type>;
	using full_row_iterator			= graph_utils::full_row_iterator<link_type>;
	using const_sparse_row_iterator = graph_utils::const_sparse_row_iterator<link_type>;
	using sparse_row_iterator		= graph_utils::sparse_row_iterator<link_type>;

	using const_full_col_iterator	= graph_utils::const_full_col_iterator<link_type>;
	using full_col_iterator			= graph_utils::full_col_iterator<link_type>;
	using const_sparse_col_iterator = graph_utils::const_sparse_col_iterator<link_type>;
	using sparse_col_iterator		= graph_utils::sparse_col_iterator<link_type>;

	using const_row_begin_iterator	= graph_utils::const_row_begin_iterator<link_type>;
	using row_begin_iterator		= graph_utils::row_begin_iterator<link_type>;
	using const_col_begin_iterator	= graph_utils::const_col_begin_iterator<link_type>;
	using col_begin_iterator		= graph_utils::col_begin_iterator<link_type>;

	

	//all elements are intiailized with this value
	//if an element is not held in memory, it is assumed that element equals this value
	const link_type def_val;

	//whether the data for each column is stored in an array or tree
	const bool col_dense;

	//whether the data for each row is stored in an array or tree
	const bool row_dense;

	











	//returns a reference to the element
	virtual link_type& at(unsigned int row, unsigned int col) = 0;

	//updates an element's value
	void at(unsigned int row, unsigned int col, const link_type& data);

	//returns a constant reference to the element
	virtual const link_type& examine(unsigned int row, unsigned int col) const = 0;

	//sets all elements to the submitted value
	virtual void clear(const link_type& data) noexcept = 0;

	template<typename Callable>
	void apply_operation(Callable lambda) {
		for (auto row = begin_rows(); row != end_rows(); ++row) {
			for (auto it = row.full_begin(); it != row.end(); ++it) {
				at(it, lambda(it));
			}
		}
	}

	template<typename Callable>
	void apply_row_operation(unsigned int row_index, Callable lambda) {
		for (auto it = full_row_begin(row_index); it != row_end(row_index); ++it) at(it, lambda(it));
	}

	template<typename Callable>
	void apply_col_operation(unsigned int col_index, Callable lambda) {
		for (auto it = full_col_begin(col_index); it != col_end(col_index); ++it) at(it, lambda(it));
	}

	//records the value an element should become when deltas are pushed
	void add_delta(unsigned int row, unsigned int col, const link_type& data);

	//updates all elements based on queued deltas
	void push_deltas(void) noexcept;

	void get_data_state(std::ostream& out) const;

	//returns a reference to the element
	virtual link_type& at(row_graph_iterator& it) = 0;

	//returns a reference to the element
	virtual link_type& at(col_graph_iterator& it) = 0;

	//updates an element's value
	void at(row_graph_iterator& it, const link_type& data);

	//updates an element's value
	void at(col_graph_iterator& it, const link_type& data);

	virtual full_row_iterator full_row_begin(unsigned int row_index) = 0;

	virtual const_full_row_iterator full_row_begin(unsigned int row_index) const = 0;

	const_full_row_iterator full_row_cbegin(unsigned int row_index) const { return full_row_begin(row_index); };


	virtual sparse_row_iterator sparse_row_begin(unsigned int row_index, const link_type& skip_data) = 0;

	virtual const_sparse_row_iterator sparse_row_begin(unsigned int row_index, const link_type& skip_data) const = 0;

	const_sparse_row_iterator sparse_row_cbegin(unsigned int row_index, const link_type& skip_data) const { return sparse_row_begin(row_index, skip_data); };


	virtual row_begin_iterator begin_rows(void) noexcept = 0;

	virtual const_row_begin_iterator begin_rows(void) const noexcept = 0;

	const_row_begin_iterator cbegin_rows(void) const noexcept { return begin_rows(); };


	virtual row_begin_iterator get_row(unsigned int row_index) = 0;

	virtual const_row_begin_iterator get_row(unsigned int row_index) const = 0;

	const_row_begin_iterator cget_row(unsigned int row_index) const { return get_row(row_index); };


	const typeless_graph_iterator row_end(unsigned int row_index) const;

	const typeless_graph_iterator end_rows(void) const noexcept;

	virtual full_col_iterator full_col_begin(unsigned int col_index) = 0;

	virtual const_full_col_iterator full_col_begin(unsigned int col_index) const = 0;

	const_full_col_iterator full_col_cbegin(unsigned int col_index) const { return full_col_begin(col_index); };


	virtual sparse_col_iterator sparse_col_begin(unsigned int col_index, const link_type& skip_data) = 0;

	virtual const_sparse_col_iterator sparse_col_begin(unsigned int col_index, const link_type& skip_data) const = 0;

	const_sparse_col_iterator sparse_col_cbegin(unsigned int col_index, const link_type& skip_data) const { return sparse_col_begin(col_index, skip_data); };


	virtual col_begin_iterator begin_cols(void) noexcept = 0;

	virtual const_col_begin_iterator begin_cols(void) const noexcept = 0;

	const_col_begin_iterator cbegin_cols(void) const noexcept { return begin_cols(); };


	virtual col_begin_iterator get_col(unsigned int col_index) = 0;

	virtual const_col_begin_iterator get_col(unsigned int col_index) const = 0;

	const_col_begin_iterator cget_col(unsigned int col_index) const { return get_col(col_index); };


	const typeless_graph_iterator col_end(unsigned int col_index) const;

	const typeless_graph_iterator end_cols(void) const noexcept;

	std::vector<link_type> get_dense_row(unsigned int row_index) const {
		std::vector<link_type> ret(col_size);
		for (auto it = full_row_begin(row_index); it != row_end(row_index); ++it) {
			ret[it.col()] = *it;
		}
		return ret;
	}

	std::vector<link_type> get_dense_col(unsigned int col_index) const {
		std::vector<link_type> ret(row_size);
		for (auto it = full_col_begin(col_index); it != col_end(col_index); ++it) {
			ret[it.row()] = *it;
		}
		return ret;
	}

	std::map<unsigned int, link_type> get_sparse_row(unsigned int row_index, const link_type& skip_value) const {
		std::map<unsigned int, link_type> ret;
		for (auto it = sparse_row_begin(row_index, skip_value); it != row_end(row_index); ++it) {
			ret[it.col()] = *it;
		}
		return ret;
	}

	std::map<unsigned int, link_type> get_sparse_col(unsigned int col_index, const link_type& skip_value) const {
		std::map<unsigned int, link_type> ret;
		for (auto it = sparse_col_begin(col_index, skip_value); it != col_end(col_index); ++it) {
			ret[it.row()] = *it;
		}
		return ret;
	}

	Transpose<link_type> T(void) { return Transpose<link_type>(this); }

	const Transpose<link_type> T(void) const { return Transpose<link_type>(this); }


	//These functions are helper functions for the iterators
	virtual void clear(row_graph_iterator& it) = 0;
	virtual void clear(col_graph_iterator& it) = 0;
	virtual void clear(unsigned int row, unsigned int col) = 0;
	
	virtual const link_type& examine(const row_graph_iterator& it) const = 0;
	virtual const link_type& examine(const col_graph_iterator& it) const = 0;

	virtual void advance(const const_full_row_iterator& it) const = 0;
	virtual void advance(const const_sparse_row_iterator& it) const = 0;
	virtual void advance(const const_row_begin_iterator& it) const = 0;
	virtual void advance(const const_full_col_iterator& it) const = 0;
	virtual void advance(const const_sparse_col_iterator& it) const = 0;
	virtual void advance(const const_col_begin_iterator& it) const = 0;

	virtual full_row_iterator full_begin(const row_begin_iterator& it) = 0;
	virtual const_full_row_iterator full_begin(const const_row_begin_iterator& it) const = 0;
	virtual sparse_row_iterator sparse_begin(const row_begin_iterator& it, const link_type& skip_data) = 0;
	virtual const_sparse_row_iterator sparse_begin(const const_row_begin_iterator& it, const link_type& skip_data) const = 0;

	virtual full_col_iterator full_begin(const col_begin_iterator& it) = 0;
	virtual const_full_col_iterator full_begin(const const_col_begin_iterator& it) const = 0;
	virtual sparse_col_iterator sparse_begin(const col_begin_iterator& it, const link_type& skip_data) = 0;
	virtual const_sparse_col_iterator sparse_begin(const const_col_begin_iterator& it, const link_type& skip_data) const = 0;
	
	template<typename input>
	Graph<link_type>& operator=(const Temporary_Graph<input>& other) {
		assert(source_nodeset == other.graph().source_nodeset);
		assert(target_nodeset == other.graph().target_nodeset);

		auto row = begin_rows();
		auto orow = other.graph().begin_rows();
		while (row != end_rows() && orow != other.graph().end_rows()) {
			auto tit = row.full_begin();
			auto oit = orow.full_begin();
			while (tit != row.end() && oit != orow.end()) {
				at(tit, *oit);
				++tit;
				++oit;
			}
			++row;
			++orow;
		}

		return *this;
	}

	template<typename other>
	Graph<link_type>& operator+=(const other& val) {
		if (val != 0)
			apply_operation(
				[&](Graph<link_type>::full_row_iterator& it) { return *it + val; }
			);

		return *this;
	}

	template<typename other>
	Graph<link_type>& operator-=(const other& val) {
		if (val != 0)
			apply_operation(
				[&](Graph<link_type>::full_row_iterator& it) { return *it - val; }
			);

		return *this;
	}

	template<typename other>
	Graph<link_type>& operator*=(const other& val) { 
		if (val != 1)
			apply_operation(
				[&](Graph<link_type>::full_row_iterator& it) { return *it * val; }
			);

		return *this;
	}

	template<typename other>
	Graph<link_type>& operator/=(const other& val) {
		assert(val != 0);
		if (val != 1)
			apply_operation(
				[&](Graph<link_type>::full_row_iterator& it) {return *it / val; }
			);
		return *this;
	}

	template<class output = decltype(link_type() + link_type())>
	output row_sum(unsigned int row_index) const {
		return get_row(row_index).sum();
	}

	template<class output = decltype(link_type() + link_type())>
	output col_sum(unsigned int col_index) const {
		return get_col(col_index).sum();
	}
};



class CONSTRUCT_LIB Graph_Intermediary {
	Typeless_Graph* ptr;
public:

	Graph_Intermediary(Typeless_Graph* _ptr) : ptr(_ptr) {}
	void check_ptr() const;

	template<typename T>
	operator Graph<T>* () {
		if (!ptr) return nullptr;
		if (ptr->edge_type != Typeless_Graph::get_edge_type<T>())
			throw dynet::construct_exception("Network \"" + ptr->name + "\" requires an edge_type of " + Typeless_Graph::get_type_name<T>());
		return dynamic_cast<Graph<T>*>(ptr);
	}

	template<typename T>
	operator Graph<T>& () {
		check_ptr();
		return *(Graph<T>*)(*this);
	}

	template<typename T>
	operator const Graph<T>* () const {
		if (!ptr) return nullptr;
		if (ptr->edge_type != Typeless_Graph::get_edge_type<T>())
			throw dynet::construct_exception("Network \"" + ptr->name + "\" requires an edge_type of " + Typeless_Graph::get_type_name<T>());
		return dynamic_cast<const Graph<T>*>(ptr);
	}

	template<typename T>
	operator const Graph<T>& () const {
		check_ptr();
		return *(const Graph<T>*)(*this);
	}

	operator bool() const { return (bool)ptr; }
};


namespace graph_utils {
	

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> ewise_product(const Graph<left>& lhs, const Graph<right>& rhs, bool row_dense, bool col_dense) {
		assert(lhs.source_nodeset == rhs.source_nodeset);
		assert(lhs.target_nodeset == rhs.target_nodeset);

		Temporary_Graph<output> ret(lhs.def_val * rhs.def_val, lhs.source_nodeset, row_dense, lhs.target_nodeset, col_dense);
		Graph<output>& out_matrix = ret.graph();

		auto out_row = out_matrix.begin_rows();
		auto left_row = lhs.begin_rows();
		auto right_row = rhs.begin_rows();
		while (out_row != out_matrix.end_rows()) {
			auto out_it = out_row.full_begin();
			auto left_it = left_row.full_begin();
			auto right_it = right_row.full_begin();
			while (out_it != out_row.end()) {
				out_matrix.at(out_it, *left_it * (*right_it));

				++out_it;
				++left_it;
				++right_it;
			}

			++out_row;
			++left_row;
			++right_row;
		}

		return ret;
	}

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> ewise_product(const Graph<left>& lhs, const Graph<right>& rhs) {
		return ewise_product(lhs, rhs, lhs.row_dense || rhs.row_dense, lhs.col_dense || rhs.col_dense);
	}

	template<typename left, typename right>
	auto ewise_product(const Temporary_Graph<left>& lhs, const Graph<right>& rhs) { return ewise_product(lhs.graph(), rhs); }

	template<typename left, typename right>
	auto ewise_product(const Graph<left>& lhs, const Temporary_Graph<right>& rhs) { return ewise_product(lhs, rhs.graph()); }

	template<typename left, typename right>
	auto ewise_product(const Temporary_Graph<left>& lhs, const Temporary_Graph<right>& rhs) { return ewise_product(lhs.graph(), rhs.graph()); }

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> ewise_product(const Graph<left>& lhs, const Transpose<right>& rhs, bool row_dense, bool col_dense) {
		assert(lhs.source_nodeset == rhs.target_nodeset);
		assert(lhs.target_nodeset == rhs.source_nodeset);

		Temporary_Graph<output> ret(lhs.def_val * rhs.graph().def_val, lhs.source_nodeset, row_dense, lhs.target_nodeset, col_dense);
		Graph<output>& out_matrix = ret.graph();

		auto out_row = out_matrix.begin_rows();
		auto left_row = lhs.begin_rows();
		auto right_row = rhs.graph().begin_cols();
		while (out_row != out_matrix.end_rows()) {
			auto out_it = out_row.full_begin();
			auto left_it = left_row.full_begin();
			auto right_it = right_row.full_begin();
			while (out_it != out_row.end()) {
				out_matrix.at(out_it, *left_it * (*right_it));

				++out_it;
				++left_it;
				++right_it;
			}

			++out_row;
			++left_row;
			++right_row;
		}

		return ret;
	}

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> ewise_product(const Graph<left>& lhs, const Transpose<right>& rhs) {
		return ewise_product(lhs, rhs, lhs.row_dense || rhs.col_dense, lhs.col_dense || rhs.row_dense);
	}

	template<typename left, typename right>
	auto ewise_product(const Transpose<left>& lhs, const Graph<right>& rhs) { return ewise_product(rhs, lhs.graph()); }

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> ewise_product(const Transpose<left>& lhs, const Transpose<right>& rhs, bool row_dense, bool col_dense) {
		assert(lhs.source_nodeset == rhs.source_nodeset);
		assert(lhs.target_nodeset == rhs.target_nodeset);

		Temporary_Graph<output> ret(lhs.graph().def_val * rhs.graph().def_val, lhs.graph().target_nodeset, row_dense, lhs.graph().source_nodeset, col_dense);
		Graph<output>& out_matrix = ret.graph();

		auto out_row = out_matrix.begin_rows();
		auto left_row = lhs.graph().begin_cols();
		auto right_row = rhs.graph().begin_cols();
		while (out_row != out_matrix.end_rows()) {
			auto out_it = out_row.full_begin();
			auto left_it = left_row.full_begin();
			auto right_it = right_row.full_begin();
			while (out_it != out_row.end()) {
				out_matrix.at(out_it, *left_it * (*right_it));

				++out_it;
				++left_it;
				++right_it;
			}

			++out_row;
			++left_row;
			++right_row;
		}

		return ret;
	}
	
	template<typename left, typename right>
	auto ewise_product(const Transpose<left>& lhs, const Transpose<right>& rhs) { 
		return ewise_product(lhs, rhs, lhs.graph().col_dense || rhs.graph().col_dense, lhs.graph().row_dense || rhs.graph().row_dense); 
	}

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> ewise_divide(const Graph<left>& lhs, const Graph<right>& rhs, bool row_dense, bool col_dense) {
		assert(lhs.source_nodeset == rhs.source_nodeset);
		assert(lhs.target_nodeset == rhs.target_nodeset);
		assert(rhs.def_val != 0);

		Temporary_Graph<output> ret(lhs.def_val / rhs.def_val, lhs.source_nodeset, row_dense, lhs.target_nodeset, col_dense);
		Graph<output>& out_matrix = ret.graph();

		auto out_row = out_matrix.begin_rows();
		auto left_row = lhs.begin_rows();
		auto right_row = rhs.begin_rows();
		while (out_row != out_matrix.end_rows()) {
			auto out_it = out_row.full_begin();
			auto left_it = left_row.full_begin();
			auto right_it = right_row.full_begin();
			while (out_it != out_row.end()) {
				out_matrix.at(out_it, *left_it / (*right_it));

				++out_it;
				++left_it;
				++right_it;
			}

			++out_row;
			++left_row;
			++right_row;
		}

		return ret;
	}

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> ewise_divide(const Graph<left>& lhs, const Graph<right>& rhs) {
		return ewise_divide(lhs, rhs, lhs.row_dense || rhs.row_dense, lhs.col_dense || rhs.col_dense);
	}

	template<typename left, typename right>
	auto ewise_divide(const Temporary_Graph<left>& lhs, const Graph<right>& rhs) { return ewise_divide(lhs.graph(), rhs); }

	template<typename left, typename right>
	auto ewise_divide(const Graph<left>& lhs, const Temporary_Graph<right>& rhs) { return ewise_divide(lhs, rhs.graph()); }

	template<typename left, typename right>
	auto ewise_divide(const Temporary_Graph<left>& lhs, const Temporary_Graph<right>& rhs) { return ewise_divide(lhs.graph(), rhs.graph()); }

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> ewise_divide(const Graph<left>& lhs, const Transpose<right>& rhs, bool row_dense, bool col_dense) {
		assert(lhs.source_nodeset == rhs.graph().target_nodeset);
		assert(lhs.target_nodeset == rhs.graph().source_nodeset);
		assert(rhs.graph().def_val != 0);

		Temporary_Graph<output> ret(lhs.def_val / rhs.graph().def_val, lhs.source_nodeset, row_dense, lhs.target_nodeset, col_dense);
		Graph<output>& out_matrix = ret.graph();

		auto out_row = out_matrix.begin_rows();
		auto left_row = lhs.begin_rows();
		auto right_row = rhs.graph().begin_cols();
		while (out_row != out_matrix.end_rows()) {
			auto out_it = out_row.full_begin();
			auto left_it = left_row.full_begin();
			auto right_it = right_row.full_begin();
			while (out_it != out_row.end()) {
				out_matrix.at(out_it, *left_it / (*right_it));

				++out_it;
				++left_it;
				++right_it;
			}

			++out_row;
			++left_row;
			++right_row;
		}

		return ret;
	}

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> ewise_divide(const Graph<left>& lhs, const Transpose<right>& rhs) {
		return ewise_divide(lhs, rhs, lhs.row_dense || rhs.col_dense, lhs.col_dense || rhs.row_dense);
	}

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> ewise_divide(const Transpose<left>& lhs, const Graph<right>& rhs, bool row_dense, bool col_dense) {
		assert(lhs.graph().source_nodeset == rhs.target_nodeset);
		assert(lhs.graph().target_nodeset == rhs.source_nodeset);
		assert(rhs.def_val != 0);

		Temporary_Graph<output> ret(lhs.graph().def_val / rhs.def_val, lhs.source_nodeset, row_dense, lhs.target_nodeset, col_dense);
		Graph<output>& out_matrix = ret.graph();

		auto out_row = out_matrix.begin_rows();
		auto left_row = lhs.graph().begin_cols();
		auto right_row = rhs.begin_rows();
		while (out_row != out_matrix.end_rows()) {
			auto out_it = out_row.full_begin();
			auto left_it = left_row.full_begin();
			auto right_it = right_row.full_begin();
			while (out_it != out_row.end()) {
				out_matrix.at(out_it, *left_it / (*right_it));

				++out_it;
				++left_it;
				++right_it;
			}

			++out_row;
			++left_row;
			++right_row;
		}

		return ret;
	}

	template<typename left, typename right>
	auto ewise_divide(const Transpose<left>& lhs, const Graph<right>& rhs) { 
		return ewise_divide(lhs, rhs, lhs.graph().col_dense || rhs.row_dense, lhs.graph().row_dense || rhs.col_dense); 
	}

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> ewise_divide(const Transpose<left>& lhs, const Transpose<right>& rhs, bool row_dense, bool col_dense) {
		assert(lhs.source_nodeset == rhs.source_nodeset);
		assert(lhs.target_nodeset == rhs.target_nodeset);

		Temporary_Graph<output> ret(lhs.graph().def_val * rhs.graph().def_val, lhs.graph().target_nodeset, row_dense, lhs.graph().source_nodeset, col_dense);
		Graph<output>& out_matrix = ret.graph();

		auto out_row = out_matrix.begin_rows();
		auto left_row = lhs.graph().begin_cols();
		auto right_row = rhs.graph().begin_cols();
		while (out_row != out_matrix.end_rows()) {
			auto out_it = out_row.full_begin();
			auto left_it = left_row.full_begin();
			auto right_it = right_row.full_begin();
			while (out_it != out_row.end()) {
				out_matrix.at(out_it, *left_it / (*right_it));

				++out_it;
				++left_it;
				++right_it;
			}

			++out_row;
			++left_row;
			++right_row;
		}

		return ret;
	}

	template<typename left, typename right>
	auto ewise_divide(const Transpose<left>& lhs, const Transpose<right>& rhs) {
		return ewise_divide(lhs, rhs, lhs.graph().col_dense || rhs.graph().col_dense, lhs.graph().row_dense || rhs.graph().row_dense);
	}
}



template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator*(const graph_utils::const_full_row_iterator<left>& lhs, const Graph<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs.source_nodeset);
	assert(lhs._col == 0);

	std::vector<output> ret(rhs.target_nodeset->size(), 0);

	for (auto col = rhs.begin_cols(); col != rhs.end_cols(); ++col) {
		auto temp = lhs;
		for (auto it = col.full_begin(); it != col.end(); ++it) {
			ret[*col] += (*it) * (*temp);
			++temp;
		}
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator*(const graph_utils::const_full_row_iterator<left>& lhs, const Transpose<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs.graph().target_nodeset);
	assert(lhs._col == 0);

	std::vector<output> ret(rhs.graph().source_nodeset->size(), 0);

	for (auto row = rhs.graph().begin_rows(); row != rhs.graph().end_rows(); ++row) {
		auto temp = lhs;
		for (auto it = row.full_begin(); it != row.end(); ++it) {
			ret[*row] += (*it) * (*temp);
		}
	}

	return ret;
}

template<typename left, typename right>
auto operator*(const graph_utils::const_full_row_iterator<left>& lhs, const Temporary_Graph<right>& rhs) { return lhs * rhs; }

template<typename left, typename right>
auto operator*(const Graph<left>& lhs, const graph_utils::const_full_row_iterator<right>& rhs) { return rhs * lhs.T(); }

template<typename left, typename right>
auto operator*(const Transpose<left>& lhs, const graph_utils::const_full_row_iterator<right>& rhs) { return rhs * lhs.graph(); }

template<typename left, typename right>
auto operator*(const Temporary_Graph<left>& lhs, const graph_utils::const_full_row_iterator<right>& rhs) { return lhs.graph() * rhs; }

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator*(const graph_utils::const_sparse_row_iterator<left>& lhs, const Graph<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs.source_nodeset);
	assert(lhs._skip == 0);

	std::map<unsigned int, output> ret;

	for (auto col = rhs.begin_cols(); col != rhs.end_cols(); ++col) {
		auto temp = lhs;
		auto it = col.sparse_begin(0);
		std::vector<typeless_graph_iterator*> it_list = { &temp, &it };

		output val = 0;

		for (graph_utils::init_align(it_list); it != col.end(); graph_utils::it_align(it_list)) {
			val += (*it) * (*temp);
		}

		if (val) ret[*col] = val;
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator*(const graph_utils::const_sparse_row_iterator<left>& lhs, const Transpose<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs.graph().target_nodeset);
	assert(lhs._skip == 0);

	std::map<unsigned int, output> ret;

	for (auto row = rhs.graph().begin_rows(); row != rhs.graph().end_rows(); ++row) {
		auto temp = lhs;
		auto it = row.sparse_begin(0);
		std::vector<typeless_graph_iterator*> it_list = { &temp, &it };

		output val = 0;

		for (graph_utils::init_align(it_list); it != row.end(); graph_utils::it_align(it_list)) {
			val += (*it) * (*temp);
		}

		if (val) ret[*row] = val;
	}

	return ret;
}

template<typename left, typename right>
auto operator*(const graph_utils::const_sparse_row_iterator<left>& lhs, const Temporary_Graph<right>& rhs) { return lhs * rhs.graph(); }

template<typename left, typename right>
auto operator*(const Graph<left>& lhs, const graph_utils::const_sparse_row_iterator<right>& rhs) { return rhs * lhs.T(); }

template<typename left, typename right>
auto operator*(const Transpose<left>& lhs, const graph_utils::const_sparse_row_iterator<right>& rhs) { return rhs * lhs.graph(); }

template<typename left, typename right>
auto operator*(const Temporary_Graph<left>& lhs, const graph_utils::const_sparse_row_iterator<right>& rhs) { return lhs.graph() * rhs; }



template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator*(const graph_utils::const_full_col_iterator<left>& lhs, const Graph<right>& rhs) {
	assert(lhs._parent->source_nodeset == rhs.source_nodeset);
	assert(lhs._col == 0);

	std::vector<output> ret(rhs.target_nodeset->size(), 0);

	for (auto col = rhs.begin_cols(); col != rhs.end_cols(); ++col) {
		auto temp = lhs;
		for (auto it = col.full_begin(); it != col.end(); ++it) {
			ret[*col] += (*it) * (*temp);
			++temp;
		}
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator*(const graph_utils::const_full_col_iterator<left>& lhs, const Transpose<right>& rhs) {
	assert(lhs._parent->source_nodeset == rhs.graph().target_nodeset);
	assert(lhs._col == 0);

	std::vector<output> ret(rhs.graph().source_nodeset->size(), 0);

	for (auto row = rhs.graph().begin_rows(); row != rhs.graph().end_rows(); ++row) {
		auto temp = lhs;
		for (auto it = row.full_begin(); it != row.end(); ++it) {
			ret[*row] += (*it) * (*temp);
		}
	}

	return ret;
}

template<typename left, typename right>
auto operator*(const graph_utils::const_full_col_iterator<left>& lhs, const Temporary_Graph<right>& rhs) { return lhs * rhs.graph(); }

template<typename left, typename right>
auto operator*(const Graph<left>& lhs, const graph_utils::const_full_col_iterator<right>& rhs) { return rhs * lhs.T(); }

template<typename left, typename right>
auto operator*(const Transpose<left>& lhs, const graph_utils::const_full_col_iterator<right>& rhs) { return rhs * lhs.graph(); }

template<typename left, typename right>
auto operator*(const Temporary_Graph<left>& lhs, const graph_utils::const_full_col_iterator<right>& rhs) { return lhs.graph() * rhs; }

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator*(const graph_utils::const_sparse_col_iterator<left>& lhs, const Graph<right>& rhs) {
	assert(lhs._parent->source_nodeset == rhs.source_nodeset);
	assert(lhs._skip == 0);

	std::map<unsigned int, output> ret;

	for (auto col = rhs.begin_cols(); col != rhs.end_cols(); ++col) {
		auto temp = lhs;
		auto it = col.sparse_begin(0);
		std::vector<typeless_graph_iterator*> it_list = { &temp, &it };

		output val = 0;

		for (graph_utils::init_align(it_list); it != col.end(); graph_utils::it_align(it_list)) {
			val += (*it) * (*temp);
		}

		if (val) ret[*col] = val;
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator*(const graph_utils::const_sparse_col_iterator<left>& lhs, const Transpose<right>& rhs) {
	assert(lhs._parent->source_nodeset == rhs.graph().target_nodeset);
	assert(lhs._skip == 0);

	std::map<unsigned int, output> ret;

	for (auto row = rhs.graph().begin_rows(); row != rhs.graph().end_rows(); ++row) {
		auto temp = lhs;
		auto it = row.sparse_begin(0);
		std::vector<typeless_graph_iterator*> it_list = { &temp, &it };

		output val = 0;

		for (graph_utils::init_align(it_list); it != row.end(); graph_utils::it_align(it_list)) {
			val += (*it) * (*temp);
		}

		if (val) ret[*row] = val;
	}

	return ret;
}

template<typename left, typename right>
auto operator*(const graph_utils::const_sparse_col_iterator<left>& lhs, const Temporary_Graph<right>& rhs) { return lhs * rhs.graph(); }

template<typename left, typename right>
auto operator*(const Graph<left>& lhs, const graph_utils::const_sparse_col_iterator<right>& rhs) { return rhs * lhs.T(); }

template<typename left, typename right>
auto operator*(const Transpose<left>& lhs, const graph_utils::const_sparse_col_iterator<right>& rhs) { return rhs * lhs.graph(); }

template<typename left, typename right>
auto operator*(const Temporary_Graph<left>& lhs, const graph_utils::const_sparse_col_iterator<right>& rhs) { return lhs.graph() * rhs; }



template<typename left, typename right>
auto operator*(const graph_utils::const_row_begin_iterator<left>& lhs, const std::vector<right>& rhs) {
	assert(rhs.size() == lhs._parent->col_size);

	// return type will be the what the compiler decide right * left should convert to
	// i.e. int * float => float
	// fairly straightforward except for bool * bool => int
	decltype(right() * left()) ret = 0;
	auto it = lhs.full_begin();
	for (auto val : rhs) {
		ret += *it * val;
		++it;
	}
	return ret;
}

template<typename left, typename right>
auto operator*(const std::vector<left>& lhs, const graph_utils::const_row_begin_iterator<right>& rhs) { return rhs * lhs; }

template<typename left, typename right>
auto operator*(const graph_utils::const_row_begin_iterator<left>& lhs, const std::map<unsigned int, right>& rhs) {

	// return type will be the what the compiler decide right * left should convert to
	// i.e. int * float => float
	// fairly straightforward except for bool * bool => int
	decltype(right() * left()) ret = 0;
	auto git = lhs.sparse_begin(0);
	auto mit = rhs.begin();
	while (git != lhs.end() && mit != rhs.end()) {
		if (git.index() < mit->first) ++git;
		else if (git.index() > mit->first) ++mit;
		else {
			ret += *git * mit->second;
			++git;
			++mit;
		}
	}
	return ret;
}

template<typename left, typename right>
auto operator*(const std::map<unsigned int, left>& lhs, const graph_utils::const_row_begin_iterator<right>& rhs) { return rhs * lhs; }

template<typename left, typename right>
auto operator*(const graph_utils::const_col_begin_iterator<left>& lhs, const std::vector<right>& rhs) {
	assert(rhs.size() == lhs._parent->row_size);

	// return type will be the what the compiler decide right * left should convert to
	// i.e. int * float => float
	// fairly straightforward except for bool * bool => int
	decltype(right() * left()) ret = 0;
	auto it = lhs.full_begin();
	for (auto val : rhs) {
		ret += *it * val;
		++it;
	}
	return ret;
}

template<typename left, typename right>
auto operator*(const std::vector<left>& lhs, const graph_utils::const_col_begin_iterator<right>& rhs) { return rhs * lhs; }

template<typename left, typename right>
auto operator*(const graph_utils::const_col_begin_iterator<left>& lhs, const std::map<unsigned int, right>& rhs) {
	assert(rhs.size() == lhs._parent->row_size);

	// return type will be the what the compiler decide right * left should convert to
	// i.e. int * float => float
	// fairly straightforward except for bool * bool => int
	decltype(right() * left()) ret = 0;
	auto git = lhs.sparse_begin(0);
	auto mit = rhs.begin();
	while (git != lhs.end() && mit != rhs.end()) {
		if (git.index() < mit->first) ++git;
		else if (git.index() > mit->first) ++mit;
		else {
			ret += *git * mit->second;
			++git;
			++mit;
		}
	}
	return ret;
}

template<typename left, typename right>
auto operator*(const std::map<unsigned int, left>& lhs, const graph_utils::const_col_begin_iterator<right>& rhs) { return rhs * lhs; }

template<typename left, typename right>
auto operator*(const graph_utils::const_row_begin_iterator<left>& lhs, const graph_utils::const_row_begin_iterator<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs._parent->target_nodeset);
	decltype(left() * right()) ret = 0;

	auto lit = lhs.sparse_begin(0);
	auto rit = rhs.sparse_begin(0);
	std::vector<typeless_graph_iterator*> it_list = { &lit, &rit };


	for (graph_utils::init_align(it_list); lit != lhs.end(); graph_utils::it_align(it_list)) {
		ret += (*lit) * (*rit);
	}

	return ret;
}

template<typename left, typename right>
auto operator*(const graph_utils::const_col_begin_iterator<left>& lhs, const graph_utils::const_col_begin_iterator<right>& rhs) {
	assert(lhs._parent->source_nodeset == rhs._parent->source_nodeset);
	decltype(left() * right()) ret = 0;

	auto lit = lhs.sparse_begin(0);
	auto rit = rhs.sparse_begin(0);
	std::vector<typeless_graph_iterator*> it_list = { &lit, &rit };


	for (graph_utils::init_align(it_list); lit != lhs.end(); graph_utils::it_align(it_list)) {
		ret += (*lit) * (*rit);
	}

	return ret;
}

template<typename left, typename right>
auto operator*(const graph_utils::const_row_begin_iterator<left>& lhs, const graph_utils::const_col_begin_iterator<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs._parent->source_nodeset);
	decltype(left() * right()) ret = 0;

	auto lit = lhs.sparse_begin(0);
	auto rit = rhs.sparse_begin(0);
	std::vector<typeless_graph_iterator*> it_list = { &lit, &rit };


	for (graph_utils::init_align(it_list); lit != lhs.end(); graph_utils::it_align(it_list)) {
		ret += (*lit) * (*rit);
	}

	return ret;
}

template<typename left, typename right>
auto operator*(const graph_utils::const_col_begin_iterator<left>& lhs, const graph_utils::const_row_begin_iterator<right>& rhs) { return rhs * lhs; }



template<typename left, typename right, class output = decltype(left()* right())>
Temporary_Graph<output> operator+(const Graph<left>& lhs, const Graph<right>& rhs) {
	assert(rhs.source_nodeset == lhs.source_nodeset);
	assert(rhs.target_nodeset == lhs.target_nodeset);

	Temporary_Graph<output> ret(lhs.def_val + rhs.def_val, lhs.source_nodeset, lhs.row_dense || rhs.row_dense, lhs.target_nodeset, lhs.col_dense || rhs.col_dense);

	Graph<right>& out_matrix = ret.graph();

	auto left_row = lhs.begin_rows();
	auto right_row = rhs.begin_rows();
	auto out_row = out_matrix.begin_rows();

	while (left_row != lhs.end_rows()) {

		auto left_it = left_row.full_begin();
		auto right_it = right_row.full_begin();
		auto out_it = out_row.full_begin();

		while (left_it != left_row.end()) {
			out_matrix.at(out_it, *left_it + *right_it);

			++left_it;
			++right_it;
			++out_it;
		}

		++left_row;
		++right_row;
		++out_row;
	}
	return ret;

}

template<typename left, typename right, class output = decltype(left()* right())>
Temporary_Graph<output> operator+(const Graph<left>& lhs, const Transpose<right>& rhsT) {
	const Graph<right>& rhs = rhsT.graph();

	assert(rhs.target_nodeset == lhs.source_nodeset);
	assert(rhs.source_nodeset == lhs.target_nodeset);

	Temporary_Graph<output> ret(lhs.def_val + rhs.def_val, lhs.source_nodeset, lhs.row_dense || rhs.col_dense, lhs.target_nodeset, lhs.col_dense || rhs.row_dense);

	Graph<right>& out_matrix = ret.graph();

	auto left_row = lhs.begin_rows();
	auto right_col = rhs.begin_cols();
	auto out_row = out_matrix.begin_rows();

	while (left_row != lhs.end_rows()) {

		auto left_it = left_row.full_begin();
		auto right_it = right_col.full_begin();
		auto out_it = out_row.full_begin();

		while (left_it != left_row.end()) {
			out_matrix.at(out_it, *left_it + *right_it);

			++left_it;
			++right_it;
			++out_it;
		}

		++left_row;
		++right_col;
		++out_row;
	}
	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
Temporary_Graph<output> operator+(const Transpose<left>& lhsT, const Graph<right>& rhs) {
	const Graph<left>& lhs = lhsT.graph();

	assert(rhs.target_nodeset == lhs.source_nodeset);
	assert(rhs.source_nodeset == lhs.target_nodeset);

	Temporary_Graph<output> ret(lhs.def_val + rhs.def_val, rhs.source_nodeset, lhs.col_dense || rhs.row_dense, rhs.target_nodeset, lhs.row_dense || rhs.col_dense);

	Graph<right>& out_matrix = ret.graph();

	auto left_row = lhs.begin_cols();
	auto right_col = rhs.begin_rows();
	auto out_row = out_matrix.begin_rows();

	while (left_row != lhs.end_rows()) {

		auto left_it = left_row.full_begin();
		auto right_it = right_col.full_begin();
		auto out_it = out_row.full_begin();

		while (left_it != left_row.end()) {
			out_matrix.at(out_it, *left_it + *right_it);

			++left_it;
			++right_it;
			++out_it;
		}

		++left_row;
		++right_col;
		++out_row;
	}
	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
Temporary_Graph<output> operator+(const Transpose<left>& lhsT, const Transpose<right>& rhsT) {
	const Graph<left>& lhs = lhsT.graph();
	const Graph<right>& rhs = rhsT.graph();

	assert(rhs.source_nodeset == lhs.source_nodeset);
	assert(rhs.target_nodeset == lhs.target_nodeset);

	Temporary_Graph<output> ret(lhs.def_val + rhs.def_val, rhs.target_nodeset, lhs.col_dense || rhs.col_dense, rhs.source_nodeset, lhs.row_dense || rhs.row_dense);

	Graph<right>& out_matrix = ret.graph();

	auto left_row = lhs.begin_cols();
	auto right_col = rhs.begin_cols();
	auto out_row = out_matrix.begin_rows();

	while (left_row != lhs.end_rows()) {

		auto left_it = left_row.full_begin();
		auto right_it = right_col.full_begin();
		auto out_it = out_row.full_begin();

		while (left_it != left_row.end()) {
			out_matrix.at(out_it, *left_it + *right_it);

			++left_it;
			++right_it;
			++out_it;
		}

		++left_row;
		++right_col;
		++out_row;
	}
	return ret;
}

template<typename left, typename right>
auto operator+(const Graph<left>& lhs, const Temporary_Graph<right>& rhs) { return lhs + rhs.graph(); }

template<typename left, typename right>
auto operator+(const Temporary_Graph<left>& lhs, const Graph<right>& rhs) { return lhs.graph() + rhs; }

template<typename left, typename right>
auto operator+(const Temporary_Graph<left>& lhs, const Temporary_Graph<right>& rhs) { return lhs.graph() + rhs.graph(); }

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator+(const graph_utils::const_row_begin_iterator<left>& lhs, const std::vector<right>& rhs) {
	assert(rhs.size() == lhs._parent->col_size);

	std::vector<output> ret;
	for (auto it = lhs.full_begin(); it != lhs.end(); ++it) ret[it.col()] = *it + rhs[it.col()];

	return ret;
}

template<typename left, typename right>
auto operator+(const std::vector<left>& lhs, const graph_utils::const_row_begin_iterator<right>& rhs) { return rhs + lhs; }

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator+(const graph_utils::const_row_begin_iterator<left>& lhs, const std::map<unsigned int, right>& rhs) {
	std::map<unsigned int, output> ret = rhs;

	auto git = lhs.sparse_begin(0);
	auto rit = ret.begin();
	while (git != lhs.end() && rit != ret.end()) {
		if (rit->first < git.col()) ++rit;
		else if (git.col() < rit->first) {
			rit = ret.insert(std::pair(git.col(), *git), rit);
			++rit;
			++git;
		}
		else {
			rit->second += *git;
			++rit;
			++git;
		}
	}

	return ret;
}

template<typename left, typename right>
auto operator+(const std::map<unsigned int, left>& lhs, const graph_utils::const_row_begin_iterator<right>& rhs) { return rhs + lhs; }

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator+(const graph_utils::const_col_begin_iterator<left>& lhs, const std::vector<right>& rhs) {
	assert(rhs.size() == lhs._parent->row_size);

	std::vector<output> ret;
	for (auto it = lhs.full_begin(); it != lhs.end(); ++it) ret[it.row()] = *it + rhs[it.row()];

	return ret;
}

template<typename left, typename right>
auto operator+(const std::vector<left>& lhs, const graph_utils::const_col_begin_iterator<right>& rhs) { return rhs + lhs; }

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator+(const graph_utils::const_col_begin_iterator<left>& lhs, const std::map<unsigned int, right>& rhs) {
	std::map<unsigned int, output> ret = rhs;

	auto git = lhs.sparse_begin(0);
	auto rit = ret.begin();
	while (git != lhs.end() && rit != ret.end()) {
		if (rit->first < git.row()) ++rit;
		else if (git.row() < rit->first) {
			rit = ret.insert(std::pair(git.row(), *git), rit);
			++rit;
			++git;
		}
		else {
			rit->second += *git;
			++rit;
			++git;
		}
	}

	return ret;

}

template<typename left, typename right>
auto operator+(const std::map<unsigned int, left>& lhs, const graph_utils::const_col_begin_iterator<right>& rhs) { return rhs + lhs; }

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator+(const graph_utils::const_full_row_iterator<left>& lhs, const graph_utils::const_full_row_iterator<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs._parent->target_nodeset);
	assert(lhs.index() == rhs.index());
	assert(lhs.index() == 0);
	std::vector<output> ret(lhs._parent->col_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max()) {
		ret[lit.index()] = *lit + *rit;
		++lit;
		++rit;
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator+(const graph_utils::const_full_row_iterator<left>& lhs, const graph_utils::const_full_col_iterator<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs._parent->source_nodeset);
	assert(lhs.index() == rhs.index());
	assert(lhs.index() == 0);
	std::vector<output> ret(lhs._parent->col_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max()) {
		ret[lit.index()] = *lit + *rit;
		++lit;
		++rit;
	}

	return ret;
}

template<typename left, typename right>
auto operator+(const graph_utils::const_full_col_iterator<left>& lhs, const graph_utils::const_full_row_iterator<right>& rhs) { return rhs + lhs; }

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator+(const graph_utils::const_full_col_iterator<left>& lhs, const graph_utils::const_full_col_iterator<right>& rhs) {
	assert(lhs._parent->source_nodeset == rhs._parent->source_nodeset);
	assert(lhs.index() == rhs.index());
	assert(lhs.index() == 0);
	std::vector<output> ret(lhs._parent->row_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max()) {
		ret[lit.index()] = *lit + *rit;
		++lit;
		++rit;
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator+(const graph_utils::const_sparse_row_iterator<left>& lhs, const graph_utils::const_sparse_row_iterator<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs._parent->target_nodeset);
	assert(lhs._skip == 0);
	assert(rhs._skip == 0);
	std::map<unsigned int, output> ret(lhs._parent->col_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max() && rit.index() < rit.max()) {
		if (lit.index() < rit.index()) {
			ret[lit.index()] = *lit;
			++lit;
		}
		else if (rit.index() < lit.index()) {
			ret[rit.index()] = *rit;
			++rit;
		}
		else {
			ret[lit.index()] = *lit + *rit;
			++lit;
			++rit;
		}
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator+(const graph_utils::const_sparse_row_iterator<left>& lhs, const graph_utils::const_sparse_col_iterator<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs._parent->source_nodeset);
	assert(lhs._skip == 0);
	assert(rhs._skip == 0);
	std::map<unsigned int, output> ret(lhs._parent->col_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max() && rit.index() < rit.max()) {
		if (lit.index() < rit.index()) {
			ret[lit.index()] = *lit;
			++lit;
		}
		else if (rit.index() < lit.index()) {
			ret[rit.index()] = *rit;
			++rit;
		}
		else {
			ret[lit.index()] = *lit + *rit;
			++lit;
			++rit;
		}
	}

	return ret;
}

template<typename left, typename right>
auto operator+(const graph_utils::const_sparse_col_iterator<left>& lhs, const graph_utils::const_sparse_row_iterator<right>& rhs) { return rhs + lhs; }

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator+(const graph_utils::const_sparse_col_iterator<left>& lhs, const graph_utils::const_sparse_col_iterator<right>& rhs) {
	assert(lhs._parent->source_nodeset == rhs._parent->source_nodeset);
	assert(lhs._skip == 0);
	assert(rhs._skip == 0);
	std::map<unsigned int, output> ret(lhs._parent->row_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max() && rit.index() < rit.max()) {
		if (lit.index() < rit.index()) {
			ret[lit.index()] = *lit;
			++lit;
		}
		else if (rit.index() < lit.index()) {
			ret[rit.index()] = *rit;
			++rit;
		}
		else {
			ret[lit.index()] = *lit + *rit;
			++lit;
			++rit;
		}
	}

	return ret;
}



template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator-(const graph_utils::const_row_begin_iterator<left>& lhs, const std::vector<right>& rhs) {
	assert(rhs.size() == lhs._parent->col_size);

	std::vector<output> ret;
	for (auto it = lhs.full_begin(); it != lhs.end(); ++it) ret[it.col()] = *it - rhs[it.col()];

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator-(const std::vector<left>& lhs, const graph_utils::const_row_begin_iterator<right>& rhs) { 
	assert(lhs.size() == rhs._parent->col_size);

	std::vector<output> ret;
	for (auto it = rhs.full_begin(); it != rhs.end(); ++it) ret[it.col()] = lhs[it.col()] - (*it);

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator-(const graph_utils::const_row_begin_iterator<left>& lhs, const std::map<unsigned int, right>& rhs) {
	std::map<unsigned int, output> ret = rhs;

	auto git = lhs.sparse_begin(0);
	auto rit = ret.begin();
	while (git != lhs.end() && rit != ret.end()) {
		if (rit->first < git.col()) {
			rit->second *= -1;
			++rit;
		}
		else if (git.col() < rit->first) {
			rit = ret.insert(std::pair(git.col(), *git), rit);
			++rit;
			++git;
		}
		else {
			rit->second = *git - rit->second;
			++rit;
			++git;
		}
	}

	return ret;

}

template<typename left, typename right, class output = decltype(left()* right())>
auto operator-(const std::map<unsigned int, left>& lhs, const graph_utils::const_row_begin_iterator<right>& rhs) {
	std::map<unsigned int, output> ret = lhs;

	auto git = rhs.sparse_begin(0);
	auto rit = ret.begin();
	while (git != rhs.end() && rit != ret.end()) {
		if (rit->first < git.col()) ++rit;
		else if (git.col() < rit->first) {
			rit = ret.insert(std::pair(git.col(), (*git) * -1), rit);
			++rit;
			++git;
		}
		else {
			rit->second -= *git;
			++rit;
			++git;
		}
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator-(const graph_utils::const_col_begin_iterator<left>& lhs, const std::vector<right>& rhs) {
	assert(rhs.size() == lhs._parent->row_size);

	std::vector<output> ret;
	for (auto it = lhs.full_begin(); it != lhs.end(); ++it) ret[it.row()] = *it - rhs[it.row()];

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator-(const std::vector<left>& lhs, const graph_utils::const_col_begin_iterator<right>& rhs) {
	assert(lhs.size() == rhs._parent->row_size);

	std::vector<output> ret;
	for (auto it = rhs.full_begin(); it != rhs.end(); ++it) ret[it.row()] = rhs[it.row()] - (*it);

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator-(const graph_utils::const_col_begin_iterator<left>& lhs, const std::map<unsigned int, right>& rhs) {
	std::map<unsigned int, output> ret = rhs;

	auto git = lhs.sparse_begin(0);
	auto rit = ret.begin();
	while (git != lhs.end() && rit != ret.end()) {
		if (rit->first < git.row()) {
			rit->second *= -1;
			++rit;
		}
		else if (git.row() < rit->first) {
			rit = ret.insert(std::pair(git.row(), *git), rit);
			++rit;
			++git;
		}
		else {
			rit->second = *git - rit->second;
			++rit;
			++git;
		}
	}

	return ret;

}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator-(const std::map<unsigned int, left>& lhs, const graph_utils::const_col_begin_iterator<right>& rhs) {
	std::map<unsigned int, output> ret = lhs;

	auto git = rhs.sparse_begin(0);
	auto rit = ret.begin();
	while (git != lhs.end() && rit != ret.end()) {
		if (rit->first < git.row()) ++rit;
		else if (git.row() < rit->first) {
			rit = ret.insert(std::pair(git.row(), (*git) * -1), rit);
			++rit;
			++git;
		}
		else {
			rit->second -= *git;
			++rit;
			++git;
		}
	}

	return ret;

}

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator-(const graph_utils::const_full_row_iterator<left>& lhs, const graph_utils::const_full_row_iterator<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs._parent->target_nodeset);
	assert(lhs.index() == rhs.index());
	assert(lhs.index() == 0);
	std::vector<output> ret(lhs._parent->col_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max()) {
		ret[lit.index()] = *lit - *rit;
		++lit;
		++rit;
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator-(const graph_utils::const_full_row_iterator<left>& lhs, const graph_utils::const_full_col_iterator<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs._parent->source_nodeset);
	assert(lhs.index() == rhs.index());
	assert(lhs.index() == 0);
	std::vector<output> ret(lhs._parent->col_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max()) {
		ret[lit.index()] = *lit - *rit;
		++lit;
		++rit;
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator-(const graph_utils::const_full_col_iterator<left>& lhs, const graph_utils::const_full_row_iterator<right>& rhs) {
	assert(lhs._parent->source_nodeset == rhs._parent->target_nodeset);
	assert(lhs.index() == rhs.index());
	assert(lhs.index() == 0);
	std::vector<output> ret(lhs._parent->row_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max()) {
		ret[lit.index()] = *lit - *rit;
		++lit;
		++rit;
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator-(const graph_utils::const_full_col_iterator<left>& lhs, const graph_utils::const_full_col_iterator<right>& rhs) {
	assert(lhs._parent->source_nodeset == rhs._parent->source_nodeset);
	assert(lhs.index() == rhs.index());
	assert(lhs.index() == 0);
	std::vector<output> ret(lhs._parent->row_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max()) {
		ret[lit.index()] = *lit - *rit;
		++lit;
		++rit;
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator-(const graph_utils::const_sparse_row_iterator<left>& lhs, const graph_utils::const_sparse_row_iterator<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs._parent->target_nodeset);
	assert(lhs._skip == 0);
	assert(rhs._skip == 0);
	std::map<unsigned int, output> ret(lhs._parent->col_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max() && rit.index() < rit.max()) {
		if (lit.index() < rit.index()) {
			ret[lit.index()] = *lit;
			++lit;
		}
		else if (rit.index() < lit.index()) {
			ret[rit.index()] = -1 * (*rit);
			++rit;
		}
		else {
			ret[lit.index()] = *lit - *rit;
			++lit;
			++rit;
		}
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator-(const graph_utils::const_sparse_row_iterator<left>& lhs, const graph_utils::const_sparse_col_iterator<right>& rhs) {
	assert(lhs._parent->target_nodeset == rhs._parent->source_nodeset);
	assert(lhs._skip == 0);
	assert(rhs._skip == 0);
	std::map<unsigned int, output> ret(lhs._parent->col_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max() && rit.index() < rit.max()) {
		if (lit.index() < rit.index()) {
			ret[lit.index()] = *lit;
			++lit;
		}
		else if (rit.index() < lit.index()) {
			ret[rit.index()] = -1 * (*rit);
			++rit;
		}
		else {
			ret[lit.index()] = *lit - *rit;
			++lit;
			++rit;
		}
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator-(const graph_utils::const_sparse_col_iterator<left>& lhs, const graph_utils::const_sparse_row_iterator<right>& rhs) {
	assert(lhs._parent->source_nodeset == rhs._parent->target_nodeset);
	assert(lhs._skip == 0);
	assert(rhs._skip == 0);
	std::map<unsigned int, output> ret(lhs._parent->row_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max() && rit.index() < rit.max()) {
		if (lit.index() < rit.index()) {
			ret[lit.index()] = *lit;
			++lit;
		}
		else if (rit.index() < lit.index()) {
			ret[rit.index()] = -1 * *rit;
			++rit;
		}
		else {
			ret[lit.index()] = *lit - *rit;
			++lit;
			++rit;
		}
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator-(const graph_utils::const_sparse_col_iterator<left>& lhs, const graph_utils::const_sparse_col_iterator<right>& rhs) {
	assert(lhs._parent->source_nodeset == rhs._parent->source_nodeset);
	assert(lhs._skip == 0);
	assert(rhs._skip == 0);
	std::map<unsigned int, output> ret(lhs._parent->row_size);

	auto lit = lhs;
	auto rit = rhs;
	while (lit.index() < lit.max() && rit.index() < rit.max()) {
		if (lit.index() < rit.index()) {
			ret[lit.index()] = *lit;
			++lit;
		}
		else if (rit.index() < lit.index()) {
			ret[rit.index()] = *rit;
			++rit;
		}
		else {
			ret[lit.index()] = *lit - *rit;
			++lit;
			++rit;
		}
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
Temporary_Graph<output> operator-(const Graph<left>& lhs, const Graph<right>& rhs) {
	assert(rhs.source_nodeset == lhs.source_nodeset);
	assert(rhs.target_nodeset == lhs.target_nodeset);

	Temporary_Graph<output> ret(lhs.def_val + rhs.def_val, lhs.source_nodeset, lhs.row_dense || rhs.row_dense, lhs.target_nodeset, lhs.col_dense || rhs.col_dense);

	Graph<right>& out_matrix = ret.graph();

	auto left_row = lhs.begin_rows();
	auto right_row = rhs.begin_rows();
	auto out_row = out_matrix.begin_rows();

	while (left_row != lhs.end_rows()) {

		auto left_it = left_row.full_begin();
		auto right_it = right_row.full_begin();
		auto out_it = out_row.full_begin();

		while (left_it != left_row.end()) {
			out_matrix.at(out_it, *left_it - *right_it);

			++left_it;
			++right_it;
			++out_it;
		}

		++left_row;
		++right_row;
		++out_row;
	}
	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
Temporary_Graph<output> operator-(const Graph<left>& lhs, const Transpose<right>& rhsT) {
	const Graph<right>& rhs = rhsT.graph();

	assert(rhs.target_nodeset == lhs.source_nodeset);
	assert(rhs.source_nodeset == lhs.target_nodeset);

	Temporary_Graph<output> ret(lhs.def_val + rhs.def_val, lhs.source_nodeset, lhs.row_dense || rhs.col_dense, lhs.target_nodeset, lhs.col_dense || rhs.row_dense);

	Graph<right>& out_matrix = ret.graph();

	auto left_row = lhs.begin_rows();
	auto right_col = rhs.begin_cols();
	auto out_row = out_matrix.begin_rows();

	while (left_row != lhs.end_rows()) {

		auto left_it = left_row.full_begin();
		auto right_it = right_col.full_begin();
		auto out_it = out_row.full_begin();

		while (left_it != left_row.end()) {
			out_matrix.at(out_it, *left_it - *right_it);

			++left_it;
			++right_it;
			++out_it;
		}

		++left_row;
		++right_col;
		++out_row;
	}
	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
Temporary_Graph<output> operator-(const Transpose<left>& lhsT, const Graph<right>& rhs) {
	const Graph<right>& lhs = lhsT.graph();

	assert(rhs.target_nodeset == lhs.source_nodeset);
	assert(rhs.source_nodeset == lhs.target_nodeset);

	Temporary_Graph<output> ret(lhs.def_val + rhs.def_val, rhs.source_nodeset, rhs.row_dense || lhs.col_dense, rhs.target_nodeset, rhs.col_dense || lhs.row_dense);

	Graph<right>& out_matrix = ret.graph();

	auto left_row = lhs.begin_cols();
	auto right_col = rhs.begin_rows();
	auto out_row = out_matrix.begin_rows();

	while (left_row != lhs.end_rows()) {

		auto left_it = left_row.full_begin();
		auto right_it = right_col.full_begin();
		auto out_it = out_row.full_begin();

		while (left_it != left_row.end()) {
			out_matrix.at(out_it, *left_it - *right_it);

			++left_it;
			++right_it;
			++out_it;
		}

		++left_row;
		++right_col;
		++out_row;
	}
	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
Temporary_Graph<output> operator-(const Transpose<left>& lhsT, const Transpose<right>& rhsT) {
	const Graph<right>& rhs = rhsT.graph();
	const Graph<left>& lhs = lhsT.graph();

	assert(rhs.source_nodeset == lhs.source_nodeset);
	assert(rhs.target_nodeset == lhs.target_nodeset);

	Temporary_Graph<output> ret(lhs.def_val + rhs.def_val, lhs.target_nodeset, lhs.col_dense || rhs.col_dense, lhs.source_nodeset, lhs.row_dense || rhs.row_dense);

	Graph<right>& out_matrix = ret.graph();

	auto left_row = lhs.begin_cols();
	auto right_col = rhs.begin_cols();
	auto out_row = out_matrix.begin_rows();

	while (left_row != lhs.end_rows()) {

		auto left_it = left_row.full_begin();
		auto right_it = right_col.full_begin();
		auto out_it = out_row.full_begin();

		while (left_it != left_row.end()) {
			out_matrix.at(out_it, *left_it - *right_it);

			++left_it;
			++right_it;
			++out_it;
		}

		++left_row;
		++right_col;
		++out_row;
	}
	return ret;
}

template<typename left, typename right>
auto operator-(const Graph<left>& lhs, const Temporary_Graph<right>& rhs) { return lhs - rhs.graph(); }

template<typename left, typename right>
auto operator-(const Temporary_Graph<left>& lhs, const Graph<right>& rhs) { return lhs.graph() - rhs; }

template<typename left, typename right>
auto operator-(const Temporary_Graph<left>& lhs, const Temporary_Graph<right>& rhs) { return lhs.graph() - rhs.graph(); }


namespace graph_utils {
	// output dimensions for multipling two graphs is not trivial
	// each element in array corresponds to a specific combination of left and right matrix densities
	// ex	left_row	left_col	right_row	right_col
	//		dense		sparse		dense		dense
	//		1			0			1			1			= 10 <- corresponding index for the dimensions of the output graph
	// 
	// 2 = transform[10] =	1			0
	//						row_dense	col_sparse
	// 
	static constexpr unsigned char transform[16] = {
		//[false, false] * [x,x]
		0,
		0,
		0,
		0,
		//[false, true] * [x,x]
		0,
		1,
		0,
		1,
		//[true, false] * [x,x]
		0,
		0,
		2,
		3,
		//[true, true] * [x,x]
		0,
		3,
		2,
		3
	};


	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> dot_product(const Graph<left>& lhs, const Graph<right>& rhs, bool output_row_dense, bool output_col_dense) {
		// target dimension of the left_rhs and source dimension of right_rhs must match
		assert(lhs.target_nodeset == rhs.source_nodeset);

		Temporary_Graph<output> ret(lhs.def_val * rhs.def_val * lhs.col_size,
			lhs.source_nodeset, output_row_dense,
			rhs.target_nodeset, output_col_dense);

		Graph<output>& out_matrix = ret.graph();

		auto out_row = out_matrix.begin_rows();
		auto left_row = lhs.begin_rows();
		while (out_row != out_matrix.end_rows() && left_row != lhs.end_rows()) {
			auto out_it = out_row.full_begin();
			auto right_col = rhs.begin_cols();
			while (out_it != out_row.end() && right_col != rhs.end_cols()) {
				out_matrix.at(out_it, left_row * right_col);
				++out_it;
				++right_col;
			}
			++out_row;
			++left_row;
		}

		return ret;
	}

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> dot_product(const Graph<left>& lhs, const Transpose<right>& rhsT, bool output_row_dense, bool output_col_dense) {
		const Graph<right>& rhs = rhsT.graph();
		assert(lhs.target_nodeset == rhs.target_nodeset);

		Temporary_Graph<output> ret(lhs.def_val * rhs.def_val * lhs.col_size,
			lhs.source_nodeset, output_row_dense, rhs.source_nodeset, output_col_dense);

		Graph<output>& out_matrix = ret.graph();

		auto out_row = out_matrix.begin_rows();
		auto left_row = lhs.begin_rows();

		while (out_row != out_matrix.end_rows() && left_row != lhs.end_rows()) {

			auto out_it = out_row.full_begin();
			auto right_col = rhs.begin_rows();
			while (out_it != out_row.end() && right_col != rhs.end_rows()) {

				out_matrix.at(out_it, left_row * right_col);

				++out_it;
				++right_col;
			}
			++out_row;
			++left_row;
		}

		return ret;
	}

	template<typename left, typename right, class output = decltype(left()* right())>
	Temporary_Graph<output> dot_product(const Transpose<left>& lhsT, const Graph<right>& rhs, bool output_row_dense, bool output_col_dense) {
		const Graph<left>& lhs = lhsT.graph();
		assert(lhs.source_nodeset == rhs.source_nodeset);

		Temporary_Graph<output> ret(lhs.def_val * rhs.def_val * lhs.col_size,
			lhs.target_nodeset, output_row_dense, rhs.target_nodeset, output_col_dense);

		Graph<output>& out_matrix = ret.graph();

		auto out_row = out_matrix.begin_rows();
		auto left_row = lhs.begin_cols();

		while (out_row != out_matrix.end_rows() && left_row != lhs.end_cols()) {

			auto out_it = out_row.full_begin();
			auto right_col = rhs.begin_cols();
			while (out_it != out_row.end() && right_col != rhs.end_cols()) {

				out_matrix.at(out_it, left_row * right_col);

				++out_it;
				++right_col;
			}
			++out_row;
			++left_row;
		}

		return ret;
	}

}


template<typename left, typename right>
auto operator*(const Graph<left>& lhs, const Graph<right>& rhs) {
	const char& t = graph_utils::transform[rhs.col_dense + 2 * rhs.row_dense + 4 * lhs.col_dense + 8 * lhs.row_dense];
	bool new_row_dim = t & 2;
	bool new_col_dim = t & 1;
	return graph_utils::dot_product(lhs, rhs, new_row_dim, new_col_dim);
}

template<typename left, typename right>
auto operator*(const Graph<left>& lhs, const Temporary_Graph<right>& rhs) { return lhs * rhs.graph(); }

template<typename left, typename right>
auto operator*(const Temporary_Graph<left>& lhs, const Graph<right>& rhs) { return lhs.graph() * rhs; }

template<typename left, typename right>
auto operator*(const Temporary_Graph<left>& lhs, const Temporary_Graph<left>& rhs) { return lhs.graph() * rhs.graph(); }

template<typename left, typename right>
auto operator*(const Graph<left>& lhs, const Transpose<right>& rhs) {
	const char& t = graph_utils::transform[rhs.graph().col_dense + 2 * rhs.graph().row_dense + 4 * lhs.col_dense + 8 * lhs.row_dense];
	bool new_row_dim = t & 2;
	bool new_col_dim = t & 1;
	return graph_utils::dot_product(lhs, rhs, new_row_dim, new_col_dim);
}

template<typename left, typename right>
auto operator*(const Transpose<left>& lhs, const Graph<right>& rhs) {
	const char& t = graph_utils::transform[rhs.col_dense + 2 * rhs.row_dense + 4 * lhs.graph().col_dense + 8 * lhs.graph().row_dense];
	bool new_row_dim = t & 2;
	bool new_col_dim = t & 1;
	return graph_utils::dot_product(lhs, rhs, new_row_dim, new_col_dim);
}

template<typename left, typename right>
auto operator*(const Transpose<left>& lhs, const Transpose<right>& rhs) { return rhs.graph() * lhs.graph(); }


template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator*(const Graph<left>& lhs, const std::vector<right>& rhs) {
	assert(rhs.size() == lhs.col_size);
	std::vector<output> ret(lhs.row_size);

	for (auto row = lhs.begin_rows(); row != lhs.end_rows(); ++row) 
		ret[*row] = row * rhs;

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::vector<output> operator*(const std::vector<left>& lhs, const Graph<right>& rhs) {
	assert(lhs.size() == rhs.row_size);
	std::vector<output> ret(rhs.col_size);

	for (auto col = rhs.begin_cols(); col != rhs.end_cols(); ++col) 
		ret[*col] = col * lhs;

	return ret;
}

template<typename left, typename right>
auto operator*(const Temporary_Graph<left>& lhs, const std::vector<right>& rhs) { return lhs.graph() * rhs; }

template<typename left, typename right>
auto operator*(const std::vector<left>& lhs, const Temporary_Graph<right>& rhs) { return lhs * rhs.graph(); }

template<typename left, typename right>
auto operator*(const Transpose<left>& lhs, const std::vector<right>& rhs) { return rhs * lhs.graph(); }

template<typename left, typename right>
auto operator*(const std::vector<left>& lhs, const Transpose<right>& rhs) { return rhs.graph() * lhs; }

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator*(const Graph<left>& lhs, const std::map<unsigned int, right>& rhs) {
	std::map<unsigned int, output> ret;

	for (auto row = lhs.begin_rows(); row != lhs.end_rows(); ++row) {
		output temp = row * rhs;
		if (temp) ret[*row] = temp;
	}

	return ret;
}

template<typename left, typename right, class output = decltype(left()* right())>
std::map<unsigned int, output> operator*(const std::map<unsigned int, left>& lhs, const Graph<right>& rhs) {
	std::map<unsigned int, output> ret;

	for (auto col = rhs.begin_cols(); col != rhs.end_cols(); ++col) {
		output temp = col * lhs;
		if (temp) ret[*col] = temp;
	}

	return ret;
}

template<typename left, typename right>
auto operator*(const Temporary_Graph<left>& lhs, const std::map<unsigned int, right>& rhs) { return lhs.graph() * rhs; }

template<typename left, typename right>
auto operator*(const std::map<unsigned int, left>& lhs, const Temporary_Graph<right>& rhs) { return lhs * rhs.graph(); }

template<typename left, typename right>
auto operator*(const Transpose<left>& lhs, const std::map<unsigned int, right>& rhs) { return rhs * lhs.graph(); }

template<typename left, typename right>
auto operator*(const std::map<unsigned int, left>& lhs, const Transpose<right>& rhs) { return rhs.graph() * lhs; }


#define dense true
#define sparse false

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
	const std::string emotion_net		   = "emotion network";									 // "emotion network"
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
class Construct;

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

	void import_network(Typeless_Graph* graph);

	void export_network(Typeless_Graph* graph) noexcept;

};




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
	const std::string TWIT_nf_emot = "Twitter Emotion Interaction Model no followers";
	const std::string TWIT_wf_emot = "Twitter Emotion Interaction Model";
	const std::string FB_nf_emot = "Facebook Emotion Interaction Model no followers";
	const std::string FB_wf_emot = "Facebook Emotion Interaction Model";

	//Modification Models

	//"Emotion Model"
	const std::string EMOT = "Emotion Model";
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


#include <ctime>

class CONSTRUCT_LIB Construct {
	time_t begTim;

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


