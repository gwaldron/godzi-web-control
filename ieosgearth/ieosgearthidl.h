

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Fri Apr 29 11:08:41 2011
 */
/* Compiler settings for .\ieosgearth.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __ieosgearthidl_h__
#define __ieosgearthidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___Dieosgearth_FWD_DEFINED__
#define ___Dieosgearth_FWD_DEFINED__
typedef interface _Dieosgearth _Dieosgearth;
#endif 	/* ___Dieosgearth_FWD_DEFINED__ */


#ifndef ___DieosgearthEvents_FWD_DEFINED__
#define ___DieosgearthEvents_FWD_DEFINED__
typedef interface _DieosgearthEvents _DieosgearthEvents;
#endif 	/* ___DieosgearthEvents_FWD_DEFINED__ */


#ifndef __ieosgearth_FWD_DEFINED__
#define __ieosgearth_FWD_DEFINED__

#ifdef __cplusplus
typedef class ieosgearth ieosgearth;
#else
typedef struct ieosgearth ieosgearth;
#endif /* __cplusplus */

#endif 	/* __ieosgearth_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __ieosgearthLib_LIBRARY_DEFINED__
#define __ieosgearthLib_LIBRARY_DEFINED__

/* library ieosgearthLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_ieosgearthLib;

#ifndef ___Dieosgearth_DISPINTERFACE_DEFINED__
#define ___Dieosgearth_DISPINTERFACE_DEFINED__

/* dispinterface _Dieosgearth */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__Dieosgearth;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("87F293B3-5349-4E7E-A681-4F3053287E57")
    _Dieosgearth : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DieosgearthVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _Dieosgearth * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _Dieosgearth * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _Dieosgearth * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _Dieosgearth * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _Dieosgearth * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _Dieosgearth * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _Dieosgearth * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DieosgearthVtbl;

    interface _Dieosgearth
    {
        CONST_VTBL struct _DieosgearthVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _Dieosgearth_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _Dieosgearth_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _Dieosgearth_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _Dieosgearth_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _Dieosgearth_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _Dieosgearth_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _Dieosgearth_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___Dieosgearth_DISPINTERFACE_DEFINED__ */


#ifndef ___DieosgearthEvents_DISPINTERFACE_DEFINED__
#define ___DieosgearthEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DieosgearthEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DieosgearthEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("1D9360FA-1B82-462C-B397-E9C90F7A3F3C")
    _DieosgearthEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DieosgearthEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DieosgearthEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DieosgearthEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DieosgearthEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DieosgearthEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DieosgearthEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DieosgearthEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DieosgearthEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DieosgearthEventsVtbl;

    interface _DieosgearthEvents
    {
        CONST_VTBL struct _DieosgearthEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DieosgearthEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DieosgearthEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DieosgearthEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DieosgearthEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DieosgearthEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DieosgearthEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DieosgearthEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DieosgearthEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ieosgearth;

#ifdef __cplusplus

class DECLSPEC_UUID("5F41CFDD-D29D-4CA8-B37D-6F64F28E12EA")
ieosgearth;
#endif
#endif /* __ieosgearthLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


