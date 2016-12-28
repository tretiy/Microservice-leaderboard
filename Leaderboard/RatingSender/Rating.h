#pragma once
#include <string>
struct RatingLine
{
	unsigned int position = 0;
	int id = -1;
	long long rating = 0;
	std::string name = "";

	RatingLine() = default;
	RatingLine(const RatingLine& other) = default;
	RatingLine(unsigned int _pos, int _id, long long _rating):
	position{_pos},
	id{_id},
	rating{_rating}
	{
		
	}
	operator std::string()
	{
		return std::to_string(position) + " " + std::to_string(id) + 
					" " + 	std::to_string(rating) + " " + name;
	};
};