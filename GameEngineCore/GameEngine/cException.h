#pragma once
#ifndef ERRORLOGMANAGER_H_INCLUDED
#define ERRORLOGMANAGER_H_INCLUDED


#include<exception>

class cException : public std::exception
{
public:

	// Override std::exception::what();
	const char* what();

};
#endif // !ERRORLOGMANAGER_H_INCLUDED
