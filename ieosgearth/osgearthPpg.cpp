// osgearthPpg.cpp : Implementation of the CosgearthPropPage property page class.

#include "stdafx.h"
#include "ieosgearth.h"
#include "osgearthPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CosgearthPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CosgearthPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CosgearthPropPage, "IEOSGEARTH.osgearthPropPage.1",
	0x99c2e21b, 0x6973, 0x4f14, 0x8f, 0xc4, 0x15, 0xdc, 0xdf, 0x9, 0x80, 0x60)



// CosgearthPropPage::CosgearthPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CosgearthPropPage

BOOL CosgearthPropPage::CosgearthPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_IEOSGEARTH_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CosgearthPropPage::CosgearthPropPage - Constructor

CosgearthPropPage::CosgearthPropPage() :
	COlePropertyPage(IDD, IDS_IEOSGEARTH_PPG_CAPTION)
{
}



// CosgearthPropPage::DoDataExchange - Moves data between page and properties

void CosgearthPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CosgearthPropPage message handlers
