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



#ifdef USE_DLL
	#define CONSTRUCT_LIB __declspec(dllexport)
#else
	#ifdef CONSTRUCTDLL_EXPORTS
		#define CONSTRUCT_LIB __declspec(dllimport)
	#else
		#define CONSTRUCT_LIB
	#endif
#endif



namespace dynet
{
	using ParameterMap = std::unordered_map<std::string, std::string>;
	
	template<typename T> CONSTRUCT_LIB std::string get_type_name(void);


	constexpr inline char seperator() {
#ifdef _WIN32
		return '\\';
#else
		return '/';
#endif // _WIN32

	}

	template<typename T>
	T minimum(T v1, T v2) { return (v1 > v2) * v2 + (v1 <= v2) * v1; }

	template<typename T>
	T maximum(T v1, T v2) { return (v1 > v2) * v1 + (v1 <= v2) * v2; }

	CONSTRUCT_LIB std::vector<std::string> split(std::string s, std::string delimiter);

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
	public:
		Type_Interface(bool data) { _data = data; }
		operator bool() const { return _data; }
		operator int() const { return (int)_data; }
		operator unsigned() const { return (unsigned)_data; }
		operator float() const { return (float)_data; }
		operator std::string() const;
	};


	template<>
	class CONSTRUCT_LIB Type_Interface<int> {
		int _data;
	public:
		Type_Interface(int data) { _data = data; }
		operator bool() const { return (bool)_data; }
		operator int() const { return _data; }
		operator unsigned() const;
		operator float() const { return (float)_data; }
		operator std::string() const;
	};

	template<>
	class CONSTRUCT_LIB Type_Interface<unsigned int> {
		unsigned int _data;
	public:
		Type_Interface(unsigned int data) { _data = data; }
		operator bool() const { return (bool)_data; }
		operator int() const { return (unsigned)_data; }
		operator unsigned() const { return _data; }
		operator float() const { return (float)_data; }
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
		operator std::string() const { return _data; };

	};

	template<>
	class CONSTRUCT_LIB Type_Interface<float> {
		float _data;
	public:
		Type_Interface(float data) { _data = data; }
		operator bool() const { return (bool)_data; }
		operator int() const { return (int)_data; }
		operator unsigned() const;
		operator float() const { return _data; }
		operator std::string() const;
	};


	template<typename T> Type_Interface<T> convert(T data) { return Type_Interface<T>(data); }

}
#include <random>


class CONSTRUCT_LIB Random
{
	
	Random() { ; }
	std::default_random_engine generator;
	~Random() { ; }

public:
	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced

	float uniform();

	float uniform(float min, float max);

	unsigned int integer(unsigned int max);

	unsigned int poisson_number(float lambda);

	bool randombool(float probability = .5);

	float normal(float mean, float variance);

	float exponential(float mean);


	template<typename T>
	void vector_shuffle(std::vector<T>& A) {
		for (unsigned int i = 0; i < A.size(); i++) {
			unsigned int index = i + integer((unsigned int)A.size() - i);
			T temp = A[index];
			A[index] = A[i];
			A[i] = temp;
		}
	}

	unsigned int find_dist_index(std::vector<float>& pdf);

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
class CONSTRUCT_LIB Node
{
	Node() : attributes(NULL), index(NULL) { ; }	
	~Node() { ; }

public:

	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced

	const dynet::ParameterMap* const attributes;

	const std::string name;

	const unsigned int index;

	using iterator = dynet::ParameterMap::const_iterator;

	const std::string& get_attribute(std::string _name) const;
};


class CONSTRUCT_LIB Nodeset
{
	Nodeset(const std::string& _name) : name(_name) { ; }
	~Nodeset();
public:

	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced

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

	const Node* get_node_by_name(const std::string& name) const;
	
	//checks to make sure the attribute once converted is in range [min,max]
	template<typename T>
	void check_attributes(std::string attribute, T min, T max) const;

	template<typename T>
	//checks to make sure the attribute can be converted to the type specialiazation
	void check_attributes(std::string attribute) const;
};


class CONSTRUCT_LIB NodesetManager
{
	NodesetManager(void) { ; }
	~NodesetManager(void);

public:

	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced

	//creates a mutable nodeset which become immutable after calling turn_to_const
	//only immutable nodesets can be found by get_nodeset or does_nodeset_exist
	//the pointer returned is still owned by the ns_manager
	Nodeset* create_nodeset(const std::string& name);

	//can only find a nodeset if it has been turned to constant
	const Nodeset* get_nodeset(const std::string& name) const;

	//only checks nodesets that have been turned to constant
	bool does_nodeset_exist(std::string name)const;

	//sets a mutable nodeset to an immutable nodeset
	void turn_to_const(Nodeset* nodeset);
};


namespace comms_att {
	const std::string msg_complex = "maximum message complexity";		//"maximum message complexity"
	const std::string tts = "time to send";								//"time to send"
	const std::string percent_learnable = "maximum percent learnable";	//"maximum percent learnable"
}

struct CONSTRUCT_LIB CommunicationMedium
{
	//maximum value the knowledge strength network can be increased to from a message with this medium
	const float max_percent_learnable;

	//how many timeperiods must pass before a message with this medium is sent to the central message queue
	const unsigned int time_to_send;

	//the maximum number of InteractionItem that can be included in a message with this medium
	const unsigned int max_msg_complexity;



	//name of the medium
	const std::string name;

	//messages with valid = false mediums can not be added to queues
	const bool valid;

	//node index of the medium in the "medium" nodeset
	const unsigned int index;

	//Constructor that uses a Nodeset iterator that points to a node in the "medium" nodeset
	CommunicationMedium(Nodeset::iterator node);

	//Constructor that sets each member variable explicitly
	CommunicationMedium(const std::string& name, unsigned int medium_node_index,
		float max_percent_learnable, unsigned int max_msg_complexity, unsigned int time_to_send);
};


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


constexpr const char* get_name(item_keys key) {
	switch (key)
	{
	case item_keys::knowledge:
		return "knowledge";
	case item_keys::alter:
		return "alter";
	case item_keys::belief:
		return "belief";
	case item_keys::ktm:
		return "knowledgeTM";
	case item_keys::btm:
		return "beliefTM";
	case item_keys::ktrust:
		return "knowledge trust";
	case item_keys::twitter_event:
		return "twitter event";
	case item_keys::facebook_event:
		return "facebook event";
	case item_keys::feed_position:
		return "feed position";
	default:
		assert(false);
		return NULL;
	}
}


struct CONSTRUCT_LIB InteractionItem
{
	using attribute_iterator = std::unordered_set<item_keys>::iterator;
	using attribute_const_iterator = std::unordered_set<item_keys>::const_iterator;

	using index_iterator = std::unordered_map<item_keys, unsigned int>::iterator;
	using index_const_iterator = std::unordered_map<item_keys, unsigned int>::const_iterator;

	using value_iterator = std::unordered_map<item_keys, float>::iterator;
	using value_const_iterator = std::unordered_map<item_keys, float>::const_iterator;
	
	InteractionItem& set_knowledge_item(unsigned int knowledge_index);

	InteractionItem& set_knowledgeTM_item(unsigned int knowledge_index, unsigned int alter_agent);

	InteractionItem& set_belief_item(unsigned int belief_index, float belief_value);

	InteractionItem& set_beliefTM_item(unsigned int belief_index, unsigned int alter_agent, float belief_value);

	InteractionItem& set_knowledge_trust_item(unsigned int knowledge_index, float ktrust);

	static InteractionItem create_knowledge_item(unsigned int knowledge_index);

	static InteractionItem create_knowledgeTM_item(unsigned int knowledge_index, unsigned int alter_agent);

	static InteractionItem create_belief_item(unsigned int belief_index, float belief_value);

	static InteractionItem create_beliefTM_item(unsigned int belief_index, unsigned int alter_agent, float belief_value);

	static InteractionItem create_knowledge_trust_item(unsigned int knowledge_index, float ktrust);

	bool get_knowledge_item(unsigned int& knowledge_index);

	bool get_knowledgeTM_item(unsigned int& knowledge_index, unsigned int& alter_agent);

	bool get_belief_item(unsigned int& belief_index, float& belief_value);

	bool get_beliefTM_item(unsigned int& belief_index, unsigned int& alter_agent, float& belief_value);

	bool get_knowledge_trust_item(unsigned int& knowledge_index, float& ktrust);

	void clear(void);

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

	unsigned int sender;

	unsigned int receiver;

	bool valid;

	unsigned int time_to_send;

	const CommunicationMedium* medium;

	using iterator = std::vector<InteractionItem>::iterator;
	using const_iterator = std::vector<InteractionItem>::const_iterator;

	//InteractionMessage(void);

	InteractionMessage(
		unsigned int senderAgentIndex,
		unsigned int receiverAgentIndex,
		const CommunicationMedium* _medium,
		const std::vector<InteractionItem>& interactionItems = std::vector<InteractionItem>());

	

	iterator begin(void){return items.begin();}

	iterator end(void){return items.end();}

	const_iterator begin(void) const {return items.begin();}
	
	const_iterator end(void) const {return items.end();}

	unsigned int size() {return (unsigned int)items.size();}

	iterator erase(iterator itr);

	bool add_item(const InteractionItem& item);

	bool add_knowledge_item(unsigned int knowledge_index);

	bool add_knowledgeTM_item(unsigned int knowledge_index, unsigned int alter);

	bool add_belief_item(unsigned int belief_index, float belief_value);

	bool add_beliefTM_item(unsigned int belief_index, unsigned int alter, float belief_value);

	bool add_knowledge_trust_item(unsigned int knowledge_index, float ktrust);
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

	iterator begin(void) { return _queue.begin(); }

	iterator end(void) { return _queue.end(); }

	const_iterator begin(void)const { return _queue.begin(); }

	const_iterator end(void)const { return _queue.end(); }

	reverse_iterator rbegin(void) { return _queue.rbegin(); }

	reverse_iterator rend(void) { return _queue.rend(); }

	const_reverse_iterator rbegin(void)const { return _queue.rbegin(); }

	const_reverse_iterator rend(void)const { return _queue.rend(); }

	void clear(void) { _queue.clear(); }

	void addMessage(const InteractionMessage& msg);

	iterator erase(iterator itr);
};



#ifdef max
#undef max
#endif // max

class CONSTRUCT_LIB Graph_Interface {
	
	

protected:


	Graph_Interface(const Nodeset* const src, const Nodeset* const trg, const Nodeset* const slc, const std::string& network_name, const std::string& edge);
	virtual ~Graph_Interface() { ; }
public:

	virtual void push_deltas(void) { ; } 

	const Nodeset* const source_nodeset;

	const Nodeset* const target_nodeset;

	const Nodeset* const slice_nodeset;

	const std::string name;

	const std::string edge_type;

	const unsigned int row_size;

	const unsigned int col_size;

};



class CONSTRUCT_LIB Graph_iterator {
protected:
	mutable unsigned int _row;
	mutable unsigned int _col;

public:

	Graph_iterator(unsigned int row, unsigned int col);

	const unsigned int row(void) const;

	const unsigned int col(void) const;

	friend bool CONSTRUCT_LIB operator==(const Graph_iterator& l, const Graph_iterator& r);

	friend bool CONSTRUCT_LIB operator!=(const Graph_iterator& l, const Graph_iterator& r);

	virtual unsigned int index() const;

	virtual unsigned int max() const;

	virtual const Graph_iterator& operator++(void) const;

	

};

// -------------------------------------------------------------------------------------------------------------------------------------------------------- Graph

template<typename T>
class CONSTRUCT_LIB Graph : public Graph_Interface {
protected:
	virtual ~Graph() { ; }
	Graph(const Nodeset* const src, bool row_dense, const Nodeset* const trg, bool col_dense, const Nodeset* const slc, const T& def, const std::string& name);
public:

	//all elements are intiailized with this value
	//if an element is not held in memory, it is assumed that element equals this value
	const T def_val;

	//whether the data for each column is stored in an array or map
	const bool col_dense;

	//whether the data for each row is stored in an array or map
	const bool row_dense;

	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced

#define include_iterator_indexes \
	using Graph_iterator::_row; \
	using Graph_iterator::_col

	//this a macro for the body of virtual functions
	//it creates an assertion bringing you here if called
	//while also making the compiler happy
	//unravel the stack to find which function is raising the assertion
#define virtual_func_body(class_name) \
	assert(false); \
	class_name* t = NULL; \
	return *t

	class full_row_iterator;

	class CONSTRUCT_LIB const_full_row_iterator : public Graph_iterator {
		
		include_iterator_indexes;
		
		const Graph<T>* _parent;
		mutable T* _cd;
		mutable typename std::map<unsigned int, T>::const_iterator _cs;
		const_full_row_iterator() : _parent(), _cd(), _row(), _col() { ; }

	public:

		const_full_row_iterator operator=(const full_row_iterator& other);

		const const_full_row_iterator& operator++(void) const;

		const T& operator*(void) const;

		const T* operator->(void) const;

		unsigned int index() const;

		unsigned int max() const;
	};


	class CONSTRUCT_LIB full_row_iterator : public Graph_iterator {
		include_iterator_indexes;
		
		Graph<T>* _parent;
		mutable T* _cd;
		mutable typename std::map<unsigned int, T>::iterator _cs;
		full_row_iterator() : _parent(), _cd(), _row(), _col() { ; }

	public:

		//operator const_full_row_iterator();

		const full_row_iterator& operator++(void) const;

		const T& operator*(void) const;

		const T* operator->(void) const;

		unsigned int index() const;

		unsigned int max() const;
	};


	class sparse_row_iterator;

	class CONSTRUCT_LIB const_sparse_row_iterator : public Graph_iterator {
		include_iterator_indexes;

		const Graph<T>* _parent;
		mutable T* _cd;
		mutable typename std::map<unsigned int, T>::const_iterator _cs;
		T _skip;
		const_sparse_row_iterator() : _parent(), _cd(), _row(), _col() { ; }

	public:

		const_sparse_row_iterator operator=(const sparse_row_iterator& other);

		const const_sparse_row_iterator& operator++(void) const;

		const T& operator*(void) const;

		const T* operator->(void) const;

		unsigned int index() const;

		unsigned int max() const;
	};


	class CONSTRUCT_LIB sparse_row_iterator : public Graph_iterator {
		include_iterator_indexes;

		Graph<T>* _parent;
		mutable T* _cd;
		mutable typename std::map<unsigned int, T>::iterator _cs;
		T _skip;
		sparse_row_iterator() : _parent(), _cd(), _row(), _col() { ; }

	public:

		//operator const_sparse_row_iterator();

		const sparse_row_iterator& operator++(void) const;

		const T& operator*(void) const;

		const T* operator->(void) const;

		unsigned int index() const;

		unsigned int max() const;
	};


	class row_begin_iterator;

	class CONSTRUCT_LIB const_row_begin_iterator : public Graph_iterator {
		include_iterator_indexes;
		
		const Graph<T>* _parent;
		mutable T* _dd;
		mutable std::map<unsigned int, T>* _ds;
		mutable typename std::map<unsigned int, T*>::const_iterator _sd;
		mutable typename std::map<unsigned int, std::map<unsigned int, T> >::const_iterator _ss;
		const_row_begin_iterator() : _parent(), _dd(), _ds(), _row(), _col() { ; }

	public:

		const_row_begin_iterator operator=(const row_begin_iterator& other);

		const_full_row_iterator full_begin(void) const;

		const_sparse_row_iterator sparse_begin(const T& skip_data) const;

		Graph_iterator end(void) const;

		const const_row_begin_iterator& operator++(void) const;

		unsigned int operator*(void) const;

		unsigned int index() const;

		unsigned int max() const;
	};


	class CONSTRUCT_LIB row_begin_iterator : public Graph_iterator {
		include_iterator_indexes;
		
		Graph<T>* _parent;
		mutable T* _dd;
		mutable std::map<unsigned int, T>* _ds;
		mutable typename std::map<unsigned int, T*>::iterator _sd;
		mutable typename std::map<unsigned int, std::map<unsigned int, T> >::iterator _ss;
		row_begin_iterator() : _parent(), _dd(), _ds(), _row(), _col() { ; }

	public:

		//operator const_row_begin_iterator();

		full_row_iterator full_begin(void);

		sparse_row_iterator sparse_begin(const T& skip_data);

		Graph_iterator end(void) const;

		const row_begin_iterator& operator++(void) const;

		unsigned int operator*(void) const;

		unsigned int index() const;

		unsigned int max() const;
	};


	class full_col_iterator;

	class CONSTRUCT_LIB const_full_col_iterator : public Graph_iterator {
		include_iterator_indexes;
		
		const Graph<T>* _parent;
		mutable T* _dd;
		mutable typename std::map<unsigned int, T*>::const_iterator _ndd;
		const_full_col_iterator() : _parent(), _dd(), _row(), _col() { ; }

	public:

		const_full_col_iterator operator=(const full_col_iterator& other);

		const const_full_col_iterator& operator++(void) const;

		const T& operator*(void) const;

		const T* operator->(void) const;

		unsigned int index() const;

		unsigned int max() const;
	};


	class CONSTRUCT_LIB full_col_iterator : public Graph_iterator {
		include_iterator_indexes;

		Graph<T>* _parent;
		mutable T* _dd;
		mutable typename std::map<unsigned int, T*>::iterator _ndd;
		full_col_iterator() : _parent(), _dd(), _row(), _col() { ; }

	public:

		//operator const_full_col_iterator();

		const full_col_iterator& operator++(void) const;

		const T& operator*(void) const;

		const T* operator->(void) const;

		unsigned int index() const;

		unsigned int max() const;
	};


	class sparse_col_iterator;

	class CONSTRUCT_LIB const_sparse_col_iterator : public Graph_iterator {
		include_iterator_indexes;

		const Graph<T>* _parent;

		//each member corresponds to a way to find an element in the graph for each representation
		mutable T* _dd;
		mutable typename std::map<unsigned int, T*>::const_iterator _ndd;
		T _skip;
		const_sparse_col_iterator() : _parent(), _dd(), _row(), _col() { ; }


	public:

		const_sparse_col_iterator operator=(const sparse_col_iterator& other);

		const const_sparse_col_iterator& operator++(void) const;

		const T& operator*(void) const;

		const T* operator->(void) const;

		unsigned int index() const;

		unsigned int max() const;
	};


	class CONSTRUCT_LIB sparse_col_iterator : public Graph_iterator {
		include_iterator_indexes;
		Graph<T>* _parent;

		//each member corresponds to a way to find an element in the graph for each representation
		mutable T* _dd;
		mutable typename std::map<unsigned int, T*>::iterator _ndd;
		const T _skip;
		sparse_col_iterator() : _parent(), _dd(), _row(), _col() { ; }


	public:

		//operator const_sparse_col_iterator();

		const sparse_col_iterator& operator++(void) const;

		const T& operator*(void) const;

		const T* operator->(void) const;

		unsigned int index() const;

		unsigned int max() const;
	};



	class col_begin_iterator;

	class CONSTRUCT_LIB const_col_begin_iterator : public Graph_iterator {
		include_iterator_indexes;
		const Graph<T>* _parent;

		//each member coresponds to the beginning of a row
		mutable T* _dd;
		mutable typename std::map<unsigned int, std::map<unsigned int, T*> >::const_iterator _cs;
		mutable typename std::map<unsigned int, T*>::const_iterator _sd;
		const_col_begin_iterator() : _parent(), _dd(), _row(), _col() { ; }


	public:

		const_col_begin_iterator operator=(const col_begin_iterator& other);

		const_full_col_iterator full_begin(void) const;

		const_sparse_col_iterator sparse_begin(const T& skip_data) const;

		Graph_iterator end(void) const;

		const const_col_begin_iterator& operator++(void) const;

		unsigned int operator*(void) const;

		unsigned int index() const;

		//returns target dimension size
		unsigned int max() const;
	};


	class CONSTRUCT_LIB col_begin_iterator : public Graph_iterator {
		include_iterator_indexes;
		Graph<T>* _parent;

		//each member coresponds to the beginning of a row
		mutable T* _dd;
		mutable typename std::map<unsigned int, std::map<unsigned int, T*> >::iterator _cs;
		mutable typename std::map<unsigned int, T*>::iterator _sd;
		col_begin_iterator() : _parent(), _dd(), _row(), _col() { ; }


	public:

		//operator const_col_begin_iterator();

		full_col_iterator full_begin(void);

		sparse_col_iterator sparse_begin(const T& skip_data);

		Graph_iterator end(void) const;

		const col_begin_iterator& operator++(void) const;

		unsigned int operator*(void) const;

		//returns col index
		unsigned int index() const;

		//returns target dimension size
		unsigned int max() const;
	};


	//returns a reference to the element
	virtual T& at(unsigned int row, unsigned int col) { virtual_func_body(T); }

	//updates an element's value
	void at(unsigned int row, unsigned int col, const T& data);

	//returns a constant reference to the element
	virtual const T& examine(unsigned int row, unsigned int col) const { virtual_func_body(T); }

	//sets all elements to the submitted value
	virtual void clear(const T& data) { ; }

	//records the value an element should become when deltas are pushed
	void add_delta(unsigned int row, unsigned int col, const T& data);

	//updates all elements based on queued deltas
	void push_deltas(void); 

	//returns a reference to the element
	virtual T& at(full_row_iterator& it) { virtual_func_body(T); }

	//returns a reference to the element
	virtual T& at(sparse_row_iterator& it) { virtual_func_body(T); }

	//returns a reference to the element
	virtual T& at(full_col_iterator& it) { virtual_func_body(T); }

	//returns a reference to the element
	virtual T& at(sparse_col_iterator& it) { virtual_func_body(T); }

	//updates an element's value
	void at(full_row_iterator& it, const T& data);
	//updates an element's value
	void at(sparse_row_iterator& it, const T& data);
	//updates an element's value
	void at(full_col_iterator& it, const T& data);
	//updates an element's value
	void at(sparse_col_iterator& it, const T& data);

	virtual full_row_iterator full_row_begin(unsigned int row_index) { virtual_func_body(full_row_iterator); }

	virtual const_full_row_iterator full_row_begin(unsigned int row_index) const { virtual_func_body(const_full_row_iterator); }

	virtual const_full_row_iterator full_row_cbegin(unsigned int row_index) const { virtual_func_body(const_full_row_iterator); }

	virtual sparse_row_iterator sparse_row_begin(unsigned int row_index, const T& skip_data) { virtual_func_body(sparse_row_iterator); }

	virtual const_sparse_row_iterator sparse_row_begin(unsigned int row_index, const T& skip_data) const { virtual_func_body(const_sparse_row_iterator); }

	virtual const_sparse_row_iterator sparse_row_cbegin(unsigned int row_index, const T& skip_data) const { virtual_func_body(const_sparse_row_iterator); }

	virtual row_begin_iterator begin_rows(void) { virtual_func_body(row_begin_iterator); }

	virtual const_row_begin_iterator begin_rows(void) const { virtual_func_body(const_row_begin_iterator); }

	virtual const_row_begin_iterator cbegin_rows(void) const { virtual_func_body(const_row_begin_iterator); }

	virtual row_begin_iterator begin_rows(unsigned int row_index) { virtual_func_body(row_begin_iterator); }

	virtual const_row_begin_iterator begin_rows(unsigned int row_index) const { virtual_func_body(const_row_begin_iterator); }

	virtual const_row_begin_iterator cbegin_rows(unsigned int row_index) const { virtual_func_body(const_row_begin_iterator); }

	const Graph_iterator row_end(unsigned int row_index) const;

	const Graph_iterator end_rows(void) const;

	virtual full_col_iterator full_col_begin(unsigned int col_index) { virtual_func_body(full_col_iterator); }

	virtual const_full_col_iterator full_col_begin(unsigned int col_index) const { virtual_func_body(const_full_col_iterator); }

	virtual const_full_col_iterator full_col_cbegin(unsigned int col_index) const { virtual_func_body(const_full_col_iterator); }

	virtual sparse_col_iterator sparse_col_begin(unsigned int col_index, const T& skip_data) { virtual_func_body(sparse_col_iterator); }

	virtual const_sparse_col_iterator sparse_col_begin(unsigned int col_index, const T& skip_data) const { virtual_func_body(const_sparse_col_iterator); }

	virtual const_sparse_col_iterator sparse_col_cbegin(unsigned int col_index, const T& skip_data) const { virtual_func_body(const_sparse_col_iterator); }

	virtual col_begin_iterator begin_cols(void) { virtual_func_body(col_begin_iterator); }

	virtual const_col_begin_iterator begin_cols(void) const { virtual_func_body(const_col_begin_iterator); }

	virtual const_col_begin_iterator cbegin_cols(void) const { virtual_func_body(const_col_begin_iterator); }

	virtual col_begin_iterator begin_cols(unsigned int col_index) { virtual_func_body(col_begin_iterator); }

	virtual const_col_begin_iterator begin_cols(unsigned int col_index) const { virtual_func_body(const_col_begin_iterator); }

	virtual const_col_begin_iterator cbegin_cols(unsigned int col_index) const { virtual_func_body(const_col_begin_iterator); }

	const Graph_iterator col_end(unsigned int col_index) const;

	const Graph_iterator end_cols(void) const;
};

namespace graph_utils {

	void CONSTRUCT_LIB it_align(std::vector<Graph_iterator*>& it_list);

	void CONSTRUCT_LIB init_align(std::vector<Graph_iterator*>& it_list);

	void CONSTRUCT_LIB it_align_before_first(std::vector<Graph_iterator*>& it_list);

	void CONSTRUCT_LIB init_align_before_first(std::vector<Graph_iterator*>& it_list);
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

// -------------------------------------------------------------------------------------------------------------------------------------------------------- Graph Manager

class CONSTRUCT_LIB GraphManager 
{
	GraphManager(NodesetManager* ns_manager, Random* random);
	~GraphManager();


public:

	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced

	template<typename T>
	void hard_load(std::string name, const Nodeset* src, const Nodeset* trg, const Nodeset* slc, Graph<T>*& g, std::string model_name="unknown model");

	template<typename T>
	void hard_load(std::string name, const Nodeset* src, const Nodeset* trg, Graph<T>*& g, std::string model_name = "unknown model");

	template<typename T>
	void hard_load(std::string name, const Nodeset* src, const Nodeset* trg, const Nodeset* slc, const Graph<T>*& g, std::string model_name = "unknown model") const;

	template<typename T>
	void hard_load(std::string name, const Nodeset* src, const Nodeset* trg, const Graph<T>*& g, std::string model_name = "unknown model") const;

	template<typename T>
	void soft_load(std::string name, const Nodeset* src, const Nodeset* trg, Graph<T>*& g, std::string model_name = "unknown model");

	template<typename T>
	void soft_load(std::string name, const Nodeset* src, const Nodeset* trg, const Graph<T>*& g, std::string model_name = "unknown model") const;

	template<typename T>
	void soft_load(std::string name, const Nodeset* src, const Nodeset* trg, const Nodeset* slc, Graph<T>*& g, std::string model_name = "unknown model");

	template<typename T>
	void soft_load(std::string name, const Nodeset* src, const Nodeset* trg, const Nodeset* slc, const Graph<T>*& g, std::string model_name = "unknown model") const;

	template<typename T>
	Graph<T>* add_network(std::string name,
		const Nodeset* src, bool row_dense,
		const Nodeset* trg, bool col_dense,
		const T& vals, std::string model_name = "unknown model", bool verbose = true);

	template<typename T>
	Graph<T>* add_3dnetwork(std::string name,
		const Nodeset* src, bool row_dense,
		const Nodeset* trg, bool col_dense,
		const Nodeset* slc,
		const T& vals, std::string model_name = "unknown model", bool verbose = true);

	Graph_Interface* get_network(std::string name);
	
#ifdef DEBUG
	const std::set<std::string>& get_accesses(std::string name) const;
#endif // DEBUG

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

	bool valid;

	const std::string name;
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

	//@python->add_line

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
public:

	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced

	void move_model_to_front(Model* model);

	Model* get_model_by_name(const std::string &model_name);

	const Model* get_model_by_name(const std::string &model_name) const;

	void add_model(Model* model);
		

};


namespace output_names {
	const std::string output_graph = "csv"; //csv
	const std::string output_dynetml = "dynetml"; //dynetml
	const std::string output_messages = "messages"; //messages
}

struct CONSTRUCT_LIB Output {
	virtual ~Output() { ; }

	//this function should be replaced by classes that inheriet from Output
	virtual void process(unsigned int t) { assert(false); }
};


class CONSTRUCT_LIB OutputManager {
	OutputManager() { ; }
	~OutputManager(void);
public:
	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced

	void add_output(Output* output);
};


class CONSTRUCT_LIB Output_Graph : public Output {

	friend class OutputManager;
	void process(unsigned int t);
public:
	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced

	Output_Graph(const dynet::ParameterMap& params, Construct* construct);
};


class CONSTRUCT_LIB Output_dynetml : public Output {

	friend class OutputManager;
	void process(unsigned int t);
public:
	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced
	Output_dynetml(const dynet::ParameterMap& params, Construct* construct);
};


class CONSTRUCT_LIB Output_Messages : public Output {

	friend class OutputManager;
	void process(unsigned int t);
public:
	void operator*(void) const { assert(false); } //Pointers to this class should not be dereferenced
	Output_Messages(const dynet::ParameterMap& params, Construct* construct);
};


class CONSTRUCT_LIB Construct
{

	unsigned int _currentTimePeriod = 0;
	unsigned int time_count = 1;
	bool running = false;

	
public:
	~Construct();

	Construct(unsigned int seed);
	
	bool run();

	// returns the size of the "time" nodeset if it exists
	// otherwise it returns 1
	unsigned int getTimeCount() const;

	// gets the current time period index
	// the first time period index is zero
	unsigned int getCurrentTimePeriod(void)const;

	// The manager that produces random variables and ensures exact reproduction of results given a random seed.
	Random* const random;

	// The manager that holds all nodesets.
	NodesetManager* const ns_manager;

	// The manager that holds all graphs/networks.
	GraphManager* const graph_manager;

	// The manager that holds all the models and executes all of their various functions.
	ModelManager* const model_manager;

	// The manager that holds all outputs.
	OutputManager* const output_manager;

	
	// central message queue for messages waiting to be dispersed.
	InteractionMessageQueue interaction_message_queue;

	InteractionMessageQueue public_message_queue;

	// Set to true if a verbose initialization is requested.
	bool verbose_initialization = false;

	// Set to true if a verbose runtime is requested.
	bool verbose_runtime = false;

	//directory where all output gets sent.
	std::string working_directory = "";

};



class CONSTRUCT_LIB KnowledgeParsing : public Model
{
public:

	//graph name - "knowledge network"
	//agent x knowledge
	Graph<bool>* _knowledge_net;

	KnowledgeParsing(Construct* construct);

	void communicate(InteractionMessageQueue::iterator msg);
};


class CONSTRUCT_LIB KnowledgeTrust : public Model
{
	const std::string relax_rate = "relax rate";
public:

	float rr;

	//graph name - "knowledge trust network"
	//agent x knowledge
	Graph<float>* _knowledge_trust_net;

	//graph name - "knowledge trust transactive memory network"
	//agent x agent x knowledge
	Graph<std::map<unsigned int, float> >* _kttm;

	//graph name - "knowledge network"
	//agent x knowledge
	Graph<bool>* _knowledge_net;

	KnowledgeTrust(const dynet::ParameterMap& parameters, Construct* construct);

	void initialize(void);

	void update(void);

	void communicate(InteractionMessageQueue::iterator& msg);

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

	// Grabs the _send_msg_queue and decrements all of their time_to_live values
	// any msg that has gotten to zero gets added to the private message queue
	// this should be called before populating the _send_msg_queue during interactions
	virtual void update_send_msg_queue();
	
	// generates a vector of paired values with first being the agent index
	// and second being the associated initiation/reception count
	virtual void initialize_interaction_lists(std::vector<std::pair<unsigned int, unsigned int> > &initiators, std::vector<std::pair<unsigned int,unsigned int> > &receivers);

	//gets the index in the initiators and receivers list of the interaction partners found
	std::pair<unsigned int, unsigned int> get_interaction_pair_index(std::vector<std::pair<unsigned int, unsigned int> >& initiators, std::vector<std::pair<unsigned int, unsigned int> >& receivers);

	// gets the communication medium based on the shared medium between sender and receiver
	const CommunicationMedium* get_comm_medium(int sender_index, int receiver_index);

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

	//graph name - "knowledge network"
	//agent x knowledge
	Graph<bool>* _knowledge_net;

	//these graphs are for internal use, not expected as input and are only used for output
	//if they are inputted, the networks get cleared at the beginning of each time step
 

	//graph name - "interaction probability network"
	//agent x agent
	Graph<float>* _interaction_prob;
	//graph name - "interaction network"
	//agent x agent
	Graph<bool>* _interactions;

	//These graphs are used to control which agents can interact with which other agents.

	//graph name - "sphere of influence network"
	//agent x agent
	Graph<bool>* _soi;
	//graph name - "agent active timeperiod network"
	//agent x timeperiod
	const Graph<bool>* _agent_active;
	//graph name - "agent initiation count network"
	//agent x timeperiod
	const Graph<unsigned int>* _agent_initiation_count;
	//graph name - "agent reception count network"
	//agent x timeperiod
	const Graph<unsigned int>* _agent_reception_count;


	//These graphs are used to determine how messages are constructed given an interaction pair



	//graph name - "knowledge message complexity network"
	//agent x timeperiod
	const Graph<unsigned int>* _knowledge_message_complexity;
	//graph name - "knowledge priority network"
	//agent x knowledge
	const Graph<float>* _knowledge_priority_network;
	//graph name - "learnable knowledge network"
	//agent x knowledge
	const Graph<bool>* _learnable_knowledge_net;
	//graph name - "communication medium preferences network"
	//agent x medium
	const Graph<float>* _comm_medium_pref;
	//graph name - "medium knowledge network"
	//medium x knowledge
	const Graph<bool>* _medium_knowledge_access;
	//graph name - "communication medium access network"
	//agent x CommunicationMedium
	const Graph<bool>* _comm_access;

	//These graphs determine proximity which is assumed to be static.



	//graph name - "physical proximity network"
	//agent x agent
	const Graph<float>* _physical_prox;
	//graph name - "physical proximity weight network"
	//agent x timeperiod
	const Graph<float>* _physical_prox_weight;
	//graph name - "social proximity network"
	//agent x agent
	const Graph<float>* _social_prox;
	//graph name - "social proximity weight network"
	//agent x timeperiod
	const Graph<float>* _social_prox_weight;
	//graph name - "sociodemographic proximity network"
	//agent x agent
	const Graph<float>* _socdem_prox;
	//graph name - "sociodemographic proximity weight network"
	//agent x timeperiod
	const Graph<float>* _socdem_prox_weight;

	//These graphs indicate how important various pieces are for determining interaction probability



	//graph name - "interaction knowledge weight network"
	//agent x knowledge
	const Graph<float>* _knowledge_importance;
	//graph name - "knowledge similarity weight network"
	//agent x timeperiod
	const Graph<float>* _knowledge_sim_weight;
	//graph name - "knowledge expertise weight network"
	//agent x timeperiod
	const Graph<float>* _knowledge_exp_weight;


	//delayed messages waiting to be sent are here
	InteractionMessageQueue _send_msg_queue; 

	const Nodeset* agents;
	//The communication mediums derived from the medium nodeset
	std::vector<CommunicationMedium> _communication_mediums;
	//size of the agent nodeset
	unsigned int agent_count;
	//size of the knowledge nodeset
	unsigned int knowledge_count;
	//number of times a try for an interaction pair can happen before interaction pairing prematurely ends
	unsigned int interaction_rejection_limit;

	//used to prevent multiple instances of rejection limit being printed
	static bool needs_printing;
};



class CONSTRUCT_LIB Beliefs : public StandardInteraction {
public:

	Beliefs(const dynet::ParameterMap& parameters, Construct* construct);

	unsigned int belief_count;

	//graph name - "belief network"
	//agent x belief
	Graph<float>* _beliefs;

	//graph name - "belief knowledge weight network"
	//belief x knowledge
	const Graph<float>* _belief_weights;

	//graph name - "belief similarity weight network"
	//agent - timeperiod
	const Graph<float>* _belief_sim_weight;

	void initialize(void);
	void cleanup(void);

	float get_additions(unsigned int agent_i, unsigned int agent_j);
	float get_belief_sim(unsigned int agent_i, unsigned int agent_j);

	float get_belief_value(unsigned int agent_i, unsigned int belief);
};


class CONSTRUCT_LIB Tasks: public StandardInteraction
{
	public:

		unsigned int task_count;
		unsigned int agent_count;
		unsigned int knowledge_count;

		// graph name - "task assignment network"
		// agent x task
		const Graph<bool>* _task_assignment;

		// graph name - "task knowledge requirement network"
		// task x knowledge
		const Graph<bool>* _tk_req;

		// graph name - "knowledge importance network"
		// task x knowledge
		const Graph<float>* _tk_import;

		// graph name - "task guess probability network"
		// task x knowledge
		const Graph<float>* _tk_prob;

		// graph name - "completed tasks network"
		// agent x task
		Graph<unsigned int>* _task_completion;

		// graph name - "task availability network"
		// task x timeperiod
		const Graph<bool>* _availablity;


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
	const Graph<unsigned int>* _tmk_message_complexity;

	//graph name - "agent group membership network"
	//agent x agentgroup
	const Graph<bool>* _group_membership;

	//graph name "agent group knowledge network"
	//agentgroup x knowledge
	Graph<float>* _group_knowledge;

	//graph name "knowledge transactive memory network"
	//agent x agent x knowledge
	Graph<std::vector<bool> >* _tmk;

	unsigned int agent_count;
	unsigned int knowledge_count;
	unsigned int group_count;

	const Nodeset* agents;

	std::vector<unsigned int> group_size;
	std::vector<float> generalized_other;
	

	KnowledgeTransactiveMemory(const dynet::ParameterMap& parameters, Construct* construct);
	
	void initialize(void);
	void communicate(InteractionMessageQueue::iterator& msg);
	void cleanup();

	std::vector<InteractionItem> get_interaction_items(unsigned int sndr, unsigned int recv, const CommunicationMedium* comm);
	float get_k_sim(unsigned int agent_i,unsigned int agent_j);
	float get_k_exp(unsigned int agent_i, unsigned int agent_j);
	void update_group_knowledge(void);

};


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
        media_event*        _event
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

    //when events are saved to disk they are given this id and its incremented
    static unsigned int id_tracker;

    //what type of event this is i.e. "post","repost", "quote", or "reply"
    event_type type = event_type::post;

    //agent index of the user that posted the event
    unsigned int user = 0;

    //set of indexes the event contains
    std::unordered_map<item_keys, unsigned int> indexes;

    //set of values the event contains
    std::unordered_map<item_keys, float> values;

    //the time that this event was created
    float time_stamp = -1;

    //the last time any event was added to the event tree this event is apart of
    //all events in the same event tree have equal values for last_used
    float last_used = -1;

    //the pointer of the event this event is replying to.
    //if equal to this, the event has no parent and is a post.
    media_event* parent_event = this;

    //the pointer of the event that is at the root of the event tree that this event is apart of.
    //if equal to this, the event is the root event for the tree.
    media_event* root_event = this;

    //list of reposts that have shared this event
    std::vector<media_event*> reposts;

    //list of replies to this event
    std::vector<media_event*> replies;

    //list of quotes to this event
    std::vector<media_event*> quotes;

    //list of users mentioned by this event
    std::vector<unsigned int> mentions;

    //how trending an event is
    float score = 1;

    bool operator== (const media_event& a) const { return score == a.score; }
    bool operator!= (const media_event& a) const { return score != a.score; }
    bool operator<= (const media_event& a) const { return score <= a.score; }
    bool operator>= (const media_event& a) const { return score >= a.score; }
    bool operator< (const media_event& a) const { return score < a.score; }
    bool operator> (const media_event& a) const { return score > a.score; }

};


struct media_user;

class CONSTRUCT_LIB Social_Media : public virtual Model
{
    

    //model parameter name who's value gets entered into Social_Media::dt
    const std::string interval_time_duration = "interval time duration";

    //model parameter name who's value gets entered into Social_Media::age
    const std::string maximum_post_inactivity = "maximum post inactivity";
public:
	
    std::vector < std::vector<unsigned int> > responses;

    //pointer to the "agent" nodeset
    const Nodeset* agents;

    //pointer to the Knowledge Trust %Model if it has been created
    //otherwise the pointer remains a null pointer
    KnowledgeTrust* TRUST = NULL;

    //this is the medium used for all messages created by this model
    //it is intended to have unlimited complexity and avoid models that interact based on medium
    CommunicationMedium medium;

    //this key is added to messages created by this model for items that contain the feed index
    item_keys event_key = item_keys::twitter_event;

    //list of all current active events, all users can access this list
    std::list<media_event> list_of_events;

    //keeps track of event ids that mention users to speed up search algorithms
    std::vector<std::unordered_set<media_event*> > mention_graph;

    //contains each user's feed of events pseudo-sorted by priority, also contains user-centric event info like whether a event has been read
    std::vector<std::vector<media_event*> > users_feed;

    //the maximum time a post can exist without its tree being added to
    float age;

    //the time duration between time steps
    float dt;

    //current time period
    unsigned int time = 0;

    //prefix for some of the node attributes names parsed by the media_user class
    std::string media_name;

    //graphs already used by other models in construct

    //graph name - "knowledge network"
    //agent x knowledge
    Graph<bool>* _knowledge_net;

    //graph name - "agent active time network"
    //agent x timeperiod
    const Graph<bool>* _active;

    //twittersim specific graphs

    //graph name - TBD
    //agent x agent
    Graph<bool>* _follower_net = NULL;

    //list of users
    std::vector<media_user*> users;

    //Loads all nodesets and graphs for this model and checks to ensure all required node attributes are present
    //Loads the parameters "interval time duration" into dt and "maximum post inactivity" into age
    //Uses the API function create_social_media_user to populate Social_Media::users
	Social_Media(const std::string& _media_name, const dynet::ParameterMap& parameters, Construct* construct);

    //delete all pointers in stored in the Social_Media::users data structure
    virtual ~Social_Media();

    //agents read events in their feeds starting with the first event
    //reading an event will create a message with all relavant knowledge and trust information in items along with the event's feed index
    //messages are sent from the read event's author to the reading user and uses a CommunicationMedium with maximum complexity
	void think(void);

    //adds the Knowledge Parsing %Model, and attempts to find and save the pointer for the Knowledge Trust %Model if it has been added to the model list
	void initialize(void);

    //only parses messages that have an attribute equal to Social_Media::event_key for the feed position index corresponding to a media_event pointer
    //that pointer is then given to media_user::read and if the user already knows the knowledge the event is passed to media_user::(reply, quote, repost)
    //the reading user will then decide whether to follow based on the event author's charisma and similarity based on the trust transactive memory
    void communicate(InteractionMessageQueue::iterator& msg);

    //feeds are updated, the social media will recommend users to follow, and users can decide to unfollow other users
    void cleanup(void);

    //computes the Jaccard Similarity in the follower network between the two agent indexes
    float follower_jaccard_similarity(unsigned int agent_i, unsigned int agent_j);

    //function gives each user recommended users to follow
    virtual void add_followees(void);

    //each users decides whether to unfollow any other user
    virtual void remove_followees(void);

    //appends the array of InteractionItems based on the submitted event and the intended receiver of the message
    virtual void append_message(media_event* _event, InteractionMessage& msg);

    //updates each user's feeds with the new events created during the time step while also discarding read events from the feed
    //events are ordered by direct replies or mentions, events of followers, and all other events
    //within each category events are sorted based on media_event::score which is set to media_event::child_size * media_event::time_stamp
    //after the events have been organized stochastic shuffling is done on 10% of the feed to avoid a fully deterministic feed
    virtual void update_feeds(void);
};


struct CONSTRUCT_LIB Facebook : public virtual Social_Media {
    Facebook(const dynet::ParameterMap& parameters, Construct* construct);
};



struct CONSTRUCT_LIB Twitter : public virtual Social_Media {
    Twitter(const dynet::ParameterMap& parameters, Construct* construct);
};


//class that contains all settings for a user as well as functions that dictates how each user interacts
struct CONSTRUCT_LIB media_user {

    media_user(Social_Media* _media, Nodeset::iterator node);

    virtual ~media_user() { ; }

    //the social media that this user is interacting with
    Social_Media* media;

    //this user's agent index
    unsigned int id;

    //probability density to post (time in hours) pdtw * dt = average number of events in a time period
    float pdp;

    //probability to reply when an event or reply is read
    float pr;

    //probability to repost when an event is read
    float prp;

    //probability to quote when an event is read
    float pqu;

    //probability density to read events (time in hours) pdread*dt=average number of read messages in a time period.
    float pdread;

    //probability density to recommend followers (time in hours) pdaf * dt = average number of recommendations in a time period.
    float pdaf;

    //scale factor to determine number of removed followees
    float rf;

    //If true, this user, when added as a followee by another user, will automatically reciprocate followings
    bool auto_follow;

    //determines how likable someone's event is going to be.
    //people with more likable posts get more followers
    float charisma;

    //the number of events this user has read
    unsigned int read_count = 0;

    //this reads the post given and performs any actions before the post is potentially responded to
    virtual void read(media_event* _event);

    //this adds a reply to the post with probability equal to media_user::pr
    //if an event is created media_user::add_mentions is called on that event
    virtual void reply(media_event* _event);

    //this adds a quote to the post with probability equal to media_user::prp
    //if an event is created media_user::add_mentions is called on that event
    virtual void quote(media_event* _event);

    //this adds a repost to the post with probability equal to media_user::pqu
    //if an event is created media_user::add_mentions is called on that event
    virtual void repost(media_event* _event);

    //user adds a number of post events based on media_user::pdp
    virtual void generate_post_events(void);

    //mentions are added to the event if the event is a post by randomly selecting a followee
    virtual void add_mentions(media_event* post);

    //gets the trust of the knowledge index
    //if the "Knowledge Trust %Model" is not active, -1 is returned.
    virtual float get_trust(unsigned int knowledge_index);

    //returns true if this user decides to follow an agent when called
    virtual bool follow_user(unsigned int alter_agent_index);

    //Returns true if this user decides to unfollow an agent when called
    virtual bool unfollow_user(unsigned int alter_agent_index);


};


class CONSTRUCT_LIB Forget : public Model
{
	
public:

	//Beliefs_mask_mode* belief_model=0;

	const Nodeset* agents;
	unsigned int agent_count;
	unsigned int knowledge_count = 0;



	//graph name - "knowledge network"
	//agent x knowledge
	Graph<bool>* _knowledge_net = 0;

	//graph name - "knowledge strength network"
	//agent x knowledge
	Graph<float>* _knowledge_strength = 0;

	//graph name - "knowledge forgetting rate network"
	//agent x knowledge
	const Graph<float>* _knowledge_forget_rate = 0;

	//graph name - "knowledge forgetting prob network"
	//agent x knowledge
	const Graph<float>* _knowledge_forget_prob = 0;

	//graph name - "knowledge trust network"
	//agent x knowledge
	Graph<float>* _ktrust_net = 0;

	//sparse matrix of users who used a piece of knowledge
	//gets reset each time step
	//not intended to get input any input will be cleared at the beginning of each time step
	Graph<bool>* _unused_knowledge = 0;

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
	const Graph<unsigned int>* _belief_message_complexity = 0;

	//graph name - "transactive belief message complexity network"
	//agent x timeperiod
	const Graph<unsigned int>* _btm_message_complexity = 0;

	//graph name - "belief transactive memory network"
	//agent x agent x belief
	Graph<std::map<unsigned int, float> >* _btm = 0;

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
	Graph<bool>* _group_membership = 0;



	GrandInteraction(const dynet::ParameterMap& parameters, Construct* construct);
	void initialize(void);
	void communicate(InteractionMessageQueue::iterator& msg);
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

	KnowledgeLearningDifficulty(Construct* construct);

	//float get_knowledge_difficulty(unsigned int agent_index,unsigned int knowledge_index)const;
	//InteractionMessageQueue& get_interaction_message_queue();

	InteractionMessageQueue* queue;

	//graph name - "knowledge learning difficulty network"
	//agent x knowledge
	const Graph<float> *_knowledge_leanrning_difficulty_net;

};


class CONSTRUCT_LIB Location : public Model
{
public:

	// graph name - "location network"
	// location x location 
	const Graph<bool>* _loc_net;

	// graph name - "agent current location network"
	// agent x location
	Graph<bool>* _location;

	// graph name - "location knowledge network"
	// location x knowledge
	const Graph<bool>* _location_knowledge_net;

	// graph name - "agent location learning rate network"
	// agent x location
	const Graph<float>* _agent_location_learning_rate_net;

	// graph name - "knowledge expertise weight network"
	// agent x timperiod
	const Graph<float>* _knowledge_expertise;

	// graph name - "location preference network"
	// agent x location
	const Graph<float>* _location_preference;

	// graph name - "location medium access network"
	// location x CommunicationMedium
	const Graph<bool>* _location_medium_access;

	// graph name - "knowledge network"
	// agent x knowledge
	Graph<bool>* _knowledge_net;

	//graph name - "interaction knowledge weight network"
	//agent x knowledge
	const Graph<float>* _knowledge_importance;

	//graph name - "agent location learning limit network"
	//agent x location
	const Graph<unsigned int>* _loc_limit;

	//graph name - "agent active timperiod network"
	//agent x timeperiod
	const Graph<bool>* _active;

	//graph name - "knowledge priority network"
	//agent x knowledge
	const Graph<float>* _knowledge_priority_network;

	//graph name - "communication medium access network"
	//agent x CommunicationMedium
	const Graph<bool>* _comm_access;

	//graph name - "communication medium preferences network"
	//agent x CommunicationMedium
	const Graph<float>* _comm_medium_prefs;

	unsigned int agent_count;
	unsigned int location_count;
	unsigned int knowledge_count;
	std::vector<unsigned int> _current_agent_locations;
	std::vector<CommunicationMedium> _communication_mediums;

	Tasks* tasks = 0;

	Location(Construct* construct);
	void initialize(void);
	void think(void);
	void cleanup(void);

	float get_expertise(unsigned int agent, unsigned int location);
	const CommunicationMedium* get_medium(unsigned int agent);
};


class CONSTRUCT_LIB Mail : public Model
{
public:

	//graph name - "agent mail usage by medium"
	//agent x CommunicationMedium
	Graph<float>* _mail_pref;

	//graph name - "mail check probability"
	//agent x timeperiod
	Graph<float>* _mail_check_prob;

	std::vector<InteractionMessageQueue> mailboxes;

	unsigned int agent_count;
	unsigned int medium_count;

	Mail(Construct* construct);
	void update(void);
};


class CONSTRUCT_LIB Subscription : public Model
{

public:
	Subscription(Construct* construct);

	void think();
	void communicate(InteractionMessageQueue::iterator& msg);
	void cleanup();

	InteractionMessageQueue* public_queue;

	//graph name - "public propensity"
	//agent x CommunicationMedium
	const Graph<float>* _public_propensity;
	//graph name - "subscription network"
	//agent x agent
	//if row,column element is true row is subscribed to column
	Graph<bool>* _subscriptions;
	//graph name - "subscription probability"
	//agent x agent
	const Graph<float>* _sub_prob;

	unsigned int agent_count;
	unsigned int communication_count;

};

