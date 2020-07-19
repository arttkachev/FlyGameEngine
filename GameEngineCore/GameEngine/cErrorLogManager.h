#pragma once
#include "cEngineObject.h"

// Class inherits from the engine base object type
class cErrorLogManager : public cEngineObject
{
public:

	// Returns a pointer to a single instance
	static cErrorLogManager * GetErrorManager();

protected:
	cErrorLogManager();

	// Static instance member
	static cErrorLogManager m_ErrorManager;


};

