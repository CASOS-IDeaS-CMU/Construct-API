#pragma once
#if defined WIN32 || defined _WIN32 || defined __CYGWIN__ || defined __MINGW32__
#define CONSTRUCT_API __declspec(dllexport)
#else
#define CONSTRUCT_API
#endif

#include "pch.h"
//Do not edit these functions

namespace dynet {

	//Construct will call this function if it doesn't recongize a model_name
	//All the param elements in the input xml file are dumped in the ParameterMap
	//If the model_name is not reconginized by this function a NULL pointer should be returned
	extern "C" CONSTRUCT_API Model * create_model(const std::string & model_name, const ParameterMap & parameters, Construct * construct);

	//Construct will call this function if it doesn't recongize an output_name
	//All the param elements in the input xml file are dumped in the ParameterMap
	//If the output_name is not reconginized by this function a NULL pointer should be returned
	extern "C" CONSTRUCT_API Output* create_output(const std::string& output_name, const ParameterMap& parameters, Construct* construct);
}
