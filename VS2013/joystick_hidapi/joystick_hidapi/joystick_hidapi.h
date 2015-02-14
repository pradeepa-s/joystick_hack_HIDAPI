// joystick_hidapi.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cjoystick_hidapiApp:
// See joystick_hidapi.cpp for the implementation of this class
//

class Cjoystick_hidapiApp : public CWinApp
{
public:
	Cjoystick_hidapiApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cjoystick_hidapiApp theApp;