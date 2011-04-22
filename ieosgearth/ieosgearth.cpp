// ieosgearth.cpp : Implementation of CieosgearthApp and DLL registration.

#include "stdafx.h"
#include "ieosgearth.h"
#include "ieosgearth_initialization.h"
#include "objsafe.h"
#include "strsafe.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CieosgearthApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xB7B14D89, 0xE239, 0x451A, { 0xA8, 0x6E, 0xFD, 0x3, 0xD2, 0xD3, 0x46, 0x4F } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

// CLSID_SafeItem - Necessary for safe ActiveX control
// Id taken from IMPLEMENT_OLECREATE_EX function in xxxCtrl.cpp

const CATID CLSID_SafeItem =
{ 0x5f41cfdd, 0xd29d, 0x4ca8,{ 0xb3, 0x7d, 0x6f, 0x64, 0xf2, 0x8e, 0x12, 0xea}};



// CieosgearthApp::InitInstance - DLL initialization

BOOL CieosgearthApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}



// CieosgearthApp::ExitInstance - DLL termination

int CieosgearthApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    //::MessageBox(NULL, _T("DllRegisterServer Begin"), _T("Info"), MB_OK);


    HRESULT  hr;    // return for safety functions

    AFX_MANAGE_STATE(_afxModuleAddrThis);

    if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid)) 
        return ResultFromScode(SELFREG_E_TYPELIB);

    if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
        return ResultFromScode(SELFREG_E_CLASS);

    // Mark the control as safe for initializing.

    hr = CreateComponentCategory(CATID_SafeForInitializing, L"Controls safely initializable from persistent data!");
    if (FAILED(hr))
        return hr;

    hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
    if (FAILED(hr))
        return hr;

    // Mark the control as safe for scripting.

    hr = CreateComponentCategory(CATID_SafeForScripting, L"Controls safely scriptable!");
    if (FAILED(hr))
        return hr;

    hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
    if (FAILED(hr))
        return hr;

    //::MessageBox(NULL, _T("DllRegisterServer Complete"), _T("Info"), MB_OK);

    return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    HRESULT hr;    // HResult used by Safety Functions

    AFX_MANAGE_STATE(_afxModuleAddrThis);

    if (!AfxOleUnregisterTypeLib(_tlid)) 
        return ResultFromScode(SELFREG_E_TYPELIB);

    if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
        return ResultFromScode(SELFREG_E_CLASS);

    // Remove entries from the registry.

    hr=UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
    if (FAILED(hr))
        return hr;

    hr=UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
    if (FAILED(hr))
        return hr;

    return NOERROR;

}
