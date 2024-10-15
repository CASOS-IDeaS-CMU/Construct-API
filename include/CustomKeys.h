#pragma once

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
	emotion,
	banned,
	upvotes,
	downvotes,
	subreddit,
	prev_banned,
	reddit_event
	//ordering of the above items shall not be modified
	//new items can be added after the above list
	//added items should be added to InteractionItem::item_names data structure

	//,your_custom_key


	//item_key list should not exceed 100 as this is reserved for the emotion nodeset
};

enum class actions : char {
	post,
	read,
	reply,
	quote,
	repost,
	upvote,
	downvote,
	expand,
	tree
	//ordering of the above actions shall not be modified
	//new actions can be added after the above list

	//,your_custom_key
};