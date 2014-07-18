

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Thu Jul 17 18:55:45 2014
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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_ieosgearthLib,0xB7B14D89,0xE239,0x451A,0xA8,0x6E,0xFD,0x03,0xD2,0xD3,0x46,0x4F);


MIDL_DEFINE_GUID(IID, DIID__Dieosgearth,0x87F293B3,0x5349,0x4E7E,0xA6,0x81,0x4F,0x30,0x53,0x28,0x7E,0x57);


MIDL_DEFINE_GUID(IID, DIID__DieosgearthEvents,0x1D9360FA,0x1B82,0x462C,0xB3,0x97,0xE9,0xC9,0x0F,0x7A,0x3F,0x3C);


MIDL_DEFINE_GUID(CLSID, CLSID_ieosgearth,0x5F41CFDD,0xD29D,0x4CA8,0xB3,0x7D,0x6F,0x64,0xF2,0x8E,0x12,0xEA);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



