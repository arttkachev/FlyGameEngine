#include "stdafx.h"
#include "cErrorLogManager.h"

// ctr
cErrorLogManager::cErrorLogManager()
{
	// ctr goes here...
}

// Initialize the ErrorLogManager object (it is global because it is static)
cErrorLogManager cErrorLogManager::m_ErrorManager;


// Getter for the ErrorManager
cErrorLogManager* cErrorLogManager::GetErrorManager()
{
	return &m_ErrorManager;
}







