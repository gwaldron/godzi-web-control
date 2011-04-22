#pragma once

// osgearthPpg.h : Declaration of the CosgearthPropPage property page class.


// CosgearthPropPage : See osgearthPpg.cpp for implementation.

class CosgearthPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CosgearthPropPage)
	DECLARE_OLECREATE_EX(CosgearthPropPage)

// Constructor
public:
	CosgearthPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_IEOSGEARTH };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

