// osgearthCtrl.cpp : Implementation of the CosgearthCtrl ActiveX Control class.

#include "stdafx.h"
#include "windows.h"
#include "ieosgearth.h"
#include "osgearthCtrl.h"
#include "osgearthPpg.h"
#include <osg/Notify>
#include <osgDB/FileUtils>

#include <objsafe.h>
#include <Mshtml.h>

#include <sstream>

using namespace ReadyMapWebPlugin;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CosgearthCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CosgearthCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CosgearthCtrl, COleControl)
	DISP_FUNCTION_ID(CosgearthCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
    DISP_FUNCTION_ID(CosgearthCtrl, "sendCommand", dispidsendCommand, sendCommand, VT_BSTR, VTS_BSTR VTS_BSTR VTS_BOOL )    
    DISP_STOCKFUNC_DOCLICK()
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CosgearthCtrl, COleControl)
    EVENT_CUSTOM_ID("readymapevent", eventidreadymapevent, readymapevent, VTS_BSTR VTS_BSTR VTS_BSTR)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CosgearthCtrl, 1)
	PROPPAGEID(CosgearthPropPage::guid)
END_PROPPAGEIDS(CosgearthCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CosgearthCtrl, "IEOSGEARTH.osgearthCtrl.1",
	0x5f41cfdd, 0xd29d, 0x4ca8, 0xb3, 0x7d, 0x6f, 0x64, 0xf2, 0x8e, 0x12, 0xea)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CosgearthCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID BASED_CODE IID_Dieosgearth =
		{ 0x87F293B3, 0x5349, 0x4E7E, { 0xA6, 0x81, 0x4F, 0x30, 0x53, 0x28, 0x7E, 0x57 } };
const IID BASED_CODE IID_DieosgearthEvents =
		{ 0x1D9360FA, 0x1B82, 0x462C, { 0xB3, 0x97, 0xE9, 0xC9, 0xF, 0x7A, 0x3F, 0x3C } };



// Control type information

static const DWORD BASED_CODE _dwieosgearthOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CosgearthCtrl, IDS_IEOSGEARTH, _dwieosgearthOleMisc)



// CosgearthCtrl::CosgearthCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CosgearthCtrl

BOOL CosgearthCtrl::CosgearthCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_IEOSGEARTH,
			IDB_IEOSGEARTH,
			afxRegApartmentThreading,
			_dwieosgearthOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CosgearthCtrl::CosgearthCtrl - Constructor

CosgearthCtrl::CosgearthCtrl()
{
	InitializeIIDs(&IID_Dieosgearth, &IID_DieosgearthEvents);
	// TODO: Initialize your control's instance data here.
}



// CosgearthCtrl::~CosgearthCtrl - Destructor

CosgearthCtrl::~CosgearthCtrl()
{
    //MessageBox(_T("~CosgearthCtrl"), _T("Info"), MB_OK);
}



// CosgearthCtrl::OnDraw - Drawing function

void CosgearthCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

    if (!IsOptimizedDraw())
	{
		// The container does not support optimized drawing.

		// TODO: if you selected any GDI objects into the device context *pdc,
		//		restore the previously-selected objects here.
	}
}



// CosgearthCtrl::DoPropExchange - Persistence support

void CosgearthCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CosgearthCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
DWORD CosgearthCtrl::GetControlFlags()
{
	DWORD dwFlags = COleControl::GetControlFlags();


	// The control will not be redrawn when making the transition
	// between the active and inactivate state.
	dwFlags |= noFlickerActivate;

	// The control can optimize its OnDraw method, by not restoring
	// the original GDI objects in the device context.
	dwFlags |= canOptimizeDraw;
	return dwFlags;
}



// CosgearthCtrl::OnResetState - Reset control to default state

void CosgearthCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CosgearthCtrl::AboutBox - Display an "About" box to the user

void CosgearthCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_IEOSGEARTH);
	dlgAbout.DoModal();
}



// CosgearthCtrl message handlers
int
CosgearthCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    int result = COleControl::OnCreate(lpCreateStruct);

    //Initialize the map
    _map = new ReadyMapWebPlugin::MapControl;
    _map->init( GetSafeHwnd() );
    _map->setEventCallback(this);
    _map->startThread();

    while (!_map->isRunning())
    {
        ::Sleep(10);
    }

    return result;
}

void
CosgearthCtrl::OnDestroy()
{
    //MessageBox(_T("OnDestroy"), _T("Info"), MB_OK);
    if (_map.valid()) _map->cancel();
    _map = NULL;
}

void
CosgearthCtrl::OnPaint()
{

}

BSTR CosgearthCtrl::sendCommand(LPCTSTR command, LPCTSTR args, VARIANT_BOOL blocking)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CString strResult;
    strResult = _map->handleCommand( std::string(CT2CA(command)), std::string(CT2CA(args)), blocking ).c_str();
    return strResult.AllocSysString();
}

void
CosgearthCtrl::handleEvent(const std::string &target, const std::string &eventName, const std::string &data)
{
    CString targetStr, eventNameStr, dataStr;
    targetStr = target.c_str();
    eventNameStr = eventName.c_str();
    dataStr = data.c_str();
    readymapevent(targetStr.AllocSysString(), eventNameStr.AllocSysString(), dataStr.AllocSysString());
}