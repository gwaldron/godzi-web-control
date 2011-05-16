#pragma once

#include <GodziWebControl/MapControl>

// osgearthCtrl.h : Declaration of the CosgearthCtrl ActiveX Control class.


// CosgearthCtrl : See osgearthCtrl.cpp for implementation.

class CosgearthCtrl : public COleControl, public GodziWebControl::MapControl::EventCallback
{
	DECLARE_DYNCREATE(CosgearthCtrl)

// Constructor
public:
	CosgearthCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
    virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
    virtual void OnDestroy();
    virtual void OnPaint();
	virtual DWORD GetControlFlags();

    bool SendEvent(CString& jsFunctionName);

    virtual void handleEvent(const std::string &target, const std::string &eventName, const std::string &data);

// Implementation
protected:
	~CosgearthCtrl();

	DECLARE_OLECREATE_EX(CosgearthCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CosgearthCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CosgearthCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CosgearthCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:

	enum DispIDEnum {
        eventidgodzievent = 1L,
        dispidsendCommand = 1L,
	};

protected:
    osg::ref_ptr< GodziWebControl::MapControl > _map;
    BSTR sendCommand(LPCTSTR command, LPCTSTR args, VARIANT_BOOL blocking);

    void godzievent(LPCTSTR target, LPCTSTR eventName, LPCTSTR data)
    {
        FireEvent(eventidgodzievent, EVENT_PARAM(VTS_BSTR VTS_BSTR VTS_BSTR), target, eventName, data);
    }
};

