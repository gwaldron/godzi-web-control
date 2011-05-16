/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is 
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or 
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

//////////////////////////////////////////////////
//
// CPlugin class implementation
//
#ifdef XP_WIN
#include <windows.h>
#include <windowsx.h>
#endif

#ifdef XP_MAC
#include <TextEdit.h>
#endif

#ifdef XP_UNIX
#include <string.h>
#endif

#include "plugin.h"
#include "npupp.h"

using namespace ReadyMapWebPlugin;

static NPIdentifier sSendCommand_id;
static NPIdentifier sReadyMapEventHandler_id; 
static NPIdentifier sDocument_id;
static NPIdentifier sCreateEvent_id;
static NPIdentifier sDispatchEvent_id;
static NPIdentifier sInitEvent_id;


// Helper class that can be used to map calls to the NPObject hooks
// into virtual methods on instances of classes that derive from this
// class.
class ScriptablePluginObjectBase : public NPObject
{
public:
  ScriptablePluginObjectBase(NPP npp)
    : mNpp(npp)
  {
  }

  virtual ~ScriptablePluginObjectBase()
  {
  }

  // Virtual NPObject hooks called through this base class. Override
  // as you see fit.
  virtual void Invalidate();
  virtual bool HasMethod(NPIdentifier name);
  virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                      uint32_t argCount, NPVariant *result);
  virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
                             NPVariant *result);
  virtual bool HasProperty(NPIdentifier name);
  virtual bool GetProperty(NPIdentifier name, NPVariant *result);
  virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
  virtual bool RemoveProperty(NPIdentifier name);

public:
  static void _Deallocate(NPObject *npobj);
  static void _Invalidate(NPObject *npobj);
  static bool _HasMethod(NPObject *npobj, NPIdentifier name);
  static bool _Invoke(NPObject *npobj, NPIdentifier name,
                      const NPVariant *args, uint32_t argCount,
                      NPVariant *result);
  static bool _InvokeDefault(NPObject *npobj, const NPVariant *args,
                             uint32_t argCount, NPVariant *result);
  static bool _HasProperty(NPObject * npobj, NPIdentifier name);
  static bool _GetProperty(NPObject *npobj, NPIdentifier name,
                           NPVariant *result);
  static bool _SetProperty(NPObject *npobj, NPIdentifier name,
                           const NPVariant *value);
  static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);

protected:
  NPP mNpp;
};

#define DECLARE_NPOBJECT_CLASS_WITH_BASE(_class, ctor)                        \
static NPClass s##_class##_NPClass = {                                        \
  NP_CLASS_STRUCT_VERSION,                                                    \
  ctor,                                                                       \
  ScriptablePluginObjectBase::_Deallocate,                                    \
  ScriptablePluginObjectBase::_Invalidate,                                    \
  ScriptablePluginObjectBase::_HasMethod,                                     \
  ScriptablePluginObjectBase::_Invoke,                                        \
  ScriptablePluginObjectBase::_InvokeDefault,                                 \
  ScriptablePluginObjectBase::_HasProperty,                                   \
  ScriptablePluginObjectBase::_GetProperty,                                   \
  ScriptablePluginObjectBase::_SetProperty,                                   \
  ScriptablePluginObjectBase::_RemoveProperty                                 \
}

#define GET_NPOBJECT_CLASS(_class) &s##_class##_NPClass

void
ScriptablePluginObjectBase::Invalidate()
{
}

bool
ScriptablePluginObjectBase::HasMethod(NPIdentifier name)
{
  return false;
}

bool
ScriptablePluginObjectBase::Invoke(NPIdentifier name, const NPVariant *args,
                                   uint32_t argCount, NPVariant *result)
{
  return false;
}

bool
ScriptablePluginObjectBase::InvokeDefault(const NPVariant *args,
                                          uint32_t argCount, NPVariant *result)
{
  return false;
}

bool
ScriptablePluginObjectBase::HasProperty(NPIdentifier name)
{
    return false;
}

bool
ScriptablePluginObjectBase::GetProperty(NPIdentifier name, NPVariant *result)
{
    return false;
}

bool
ScriptablePluginObjectBase::SetProperty(NPIdentifier name,
                                        const NPVariant *value)
{
  return false;
}

bool
ScriptablePluginObjectBase::RemoveProperty(NPIdentifier name)
{
  return false;
}

// static
void
ScriptablePluginObjectBase::_Deallocate(NPObject *npobj)
{
  // Call the virtual destructor.
  delete (ScriptablePluginObjectBase *)npobj;
}

// static
void
ScriptablePluginObjectBase::_Invalidate(NPObject *npobj)
{
  ((ScriptablePluginObjectBase *)npobj)->Invalidate();
}

// static
bool
ScriptablePluginObjectBase::_HasMethod(NPObject *npobj, NPIdentifier name)
{
  return ((ScriptablePluginObjectBase *)npobj)->HasMethod(name);
}

// static
bool
ScriptablePluginObjectBase::_Invoke(NPObject *npobj, NPIdentifier name,
                                    const NPVariant *args, uint32_t argCount,
                                    NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->Invoke(name, args, argCount,
                                                       result);
}

// static
bool
ScriptablePluginObjectBase::_InvokeDefault(NPObject *npobj,
                                           const NPVariant *args,
                                           uint32_t argCount,
                                           NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->InvokeDefault(args, argCount,
                                                              result);
}

// static
bool
ScriptablePluginObjectBase::_HasProperty(NPObject * npobj, NPIdentifier name)
{
  return ((ScriptablePluginObjectBase *)npobj)->HasProperty(name);
}

// static
bool
ScriptablePluginObjectBase::_GetProperty(NPObject *npobj, NPIdentifier name,
                                         NPVariant *result)
{
  return ((ScriptablePluginObjectBase *)npobj)->GetProperty(name, result);
}

// static
bool
ScriptablePluginObjectBase::_SetProperty(NPObject *npobj, NPIdentifier name,
                                         const NPVariant *value)
{
  return ((ScriptablePluginObjectBase *)npobj)->SetProperty(name, value);
}

// static
bool
ScriptablePluginObjectBase::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
  return ((ScriptablePluginObjectBase *)npobj)->RemoveProperty(name);
}



class ScriptablePluginObject : public ScriptablePluginObjectBase
{
public:
  ScriptablePluginObject(NPP npp)
      : 
  ScriptablePluginObjectBase(npp),
  _plugin(0),
  _readyMapEventHandler(0)
  {
  }

  virtual bool HasMethod(NPIdentifier name);
  virtual bool Invoke(NPIdentifier name, const NPVariant *args,
                      uint32_t argCount, NPVariant *result);
  virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
                             NPVariant *result);
  virtual bool HasProperty(NPIdentifier name);
  virtual bool GetProperty(NPIdentifier name, NPVariant *result);
  virtual bool SetProperty(NPIdentifier name, const NPVariant *value);

  CPlugin* _plugin;
  NPObject* _readyMapEventHandler;
};

static NPObject *
AllocateScriptablePluginObject(NPP npp, NPClass *aClass)
{
  return new ScriptablePluginObject(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(ScriptablePluginObject,
                                 AllocateScriptablePluginObject);

bool
ScriptablePluginObject::HasMethod(NPIdentifier name)
{
  /*NPUTF8* name2 = NPN_UTF8FromIdentifier(name);
  osg::notify(osg::NOTICE) << "HasMethod " << name2 << std::endl;*/
  if (name == sSendCommand_id)
  {
      return true;
  }
  return false;
}

bool
ScriptablePluginObject::HasProperty(NPIdentifier name)
{
    /*NPUTF8* name2 = NPN_UTF8FromIdentifier(name);
    osg::notify(osg::NOTICE) << "HasProperty " << name2 << std::endl;*/
    if (name == sReadyMapEventHandler_id) return true;
    return false;
}

bool
ScriptablePluginObject::GetProperty(NPIdentifier name, NPVariant *result)
{
    /*NPUTF8* name2 = NPN_UTF8FromIdentifier(name);
    osg::notify(osg::NOTICE) << "GetProperty " << name2 << std::endl;*/

    if ((name == sReadyMapEventHandler_id) && (_readyMapEventHandler))
    {
        OBJECT_TO_NPVARIANT(_readyMapEventHandler, *result);
        return true;
    }
    //No properties
    return false;
}

bool
ScriptablePluginObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
    /*NPUTF8* name2 = NPN_UTF8FromIdentifier(name);
    osg::notify(osg::NOTICE) << "SetProperty " << name2 << std::endl;*/

    if (name == sReadyMapEventHandler_id)
    {
        //Store the reference to the incoming object
        _readyMapEventHandler = NPN_RetainObject(NPVARIANT_TO_OBJECT(*value));

        //Try to invoke it
        /*NPVariant result;
        NPN_InvokeDefault(mNpp, _myFunction, NULL, 0, &result);*/
        return true;
    }
    //No properties
    return false;
}

bool
ScriptablePluginObject::Invoke(NPIdentifier name, const NPVariant *args,
                               uint32_t argCount, NPVariant *result)
{
    /*NPUTF8* name2 = NPN_UTF8FromIdentifier(name);
    osg::notify(osg::NOTICE) << "Invoke " << name2 << std::endl;*/

    if (name == sSendCommand_id)
    {
        std::string command;
        std::string arguments;
        bool block;

        if (argCount != 3 || !NPVARIANT_IS_STRING(args[0]) || !NPVARIANT_IS_STRING(args[1]) || !NPVARIANT_IS_BOOLEAN(args[2]))
        {
            STRINGZ_TO_NPVARIANT(strdup("Invalid arguments"), *result);
            return FALSE;
        }

        command   = std::string(NPVARIANT_TO_STRING(args[0]).utf8characters, NPVARIANT_TO_STRING(args[0]).utf8length);
        arguments = std::string(NPVARIANT_TO_STRING(args[1]).utf8characters, NPVARIANT_TO_STRING(args[1]).utf8length);
        block = NPVARIANT_TO_BOOLEAN(args[2]);

        osg::notify(osg::NOTICE) << "Command=" << command << ", " << "Args=" << arguments << ", block=" << block << std::endl;
        if (_plugin)
        {
            std::string res = _plugin->_map->handleCommand(command, arguments, block);

            char *resString = (char*)NPN_MemAlloc(res.length() + 1);
            strcpy(resString,res.c_str());
            STRINGZ_TO_NPVARIANT(resString, *result);
        }
        return TRUE;
    }
  return PR_FALSE;
}

bool
ScriptablePluginObject::InvokeDefault(const NPVariant *args, uint32_t argCount,
                                      NPVariant *result)
{
  printf ("ScriptablePluginObject default method called!\n");

  STRINGZ_TO_NPVARIANT(strdup("default method return val"), *result);

  return PR_TRUE;
}

CPlugin::CPlugin(NPP pNPInstance) :
  m_pNPInstance(pNPInstance),
  m_pNPStream(NULL),
  m_bInitialized(FALSE),
  m_pScriptableObject(NULL)
{
#ifdef XP_WIN
  m_hWnd = NULL;
#endif

  sSendCommand_id = NPN_GetStringIdentifier("sendCommand");  
  sReadyMapEventHandler_id = NPN_GetStringIdentifier("readyMapEventHandler");
  sDocument_id = NPN_GetStringIdentifier("document");
  sCreateEvent_id = NPN_GetStringIdentifier("createEvent");
  sDispatchEvent_id = NPN_GetStringIdentifier("dispatchEvent");
  sInitEvent_id = NPN_GetStringIdentifier("initEvent");
}

CPlugin::~CPlugin()
{
  if (m_pScriptableObject)
    NPN_ReleaseObject(m_pScriptableObject);
}

NPBool CPlugin::init(NPWindow* pNPWindow)
{
  osg::notify(osg::NOTICE) << "CPlugin::init" << std::endl;
	if(pNPWindow == NULL)
    {
		return FALSE;
    }

	m_hWnd = (HWND)pNPWindow->window;
	if(m_hWnd == NULL)
    {
		return FALSE;
    }

    _map = new ReadyMapWebPlugin::MapControl;
    _map->setEventCallback(this);
    _map->init( m_hWnd );
    _map->startThread();

    while (!_map->isRunning())
    {
        ::Sleep(10);
    }

    m_bInitialized = TRUE;

	return TRUE;
}

void CPlugin::shut()
{
    osg::notify(osg::NOTICE) << "CPlugin::shut" << std::endl;
    if (_map.valid()) _map->cancel();
    //Delete the map
    _map = 0;
    m_bInitialized = FALSE;    
}

NPBool CPlugin::isInitialized()
{
  return m_bInitialized;
}

int16 CPlugin::handleEvent(void* event)
{
  return 0;
}

NPObject *
CPlugin::GetScriptableObject()
{
  if (!m_pScriptableObject) {
    m_pScriptableObject =
      NPN_CreateObject(m_pNPInstance,
                       GET_NPOBJECT_CLASS(ScriptablePluginObject));
  }

  if (m_pScriptableObject) {
    NPN_RetainObject(m_pScriptableObject);

  ((ScriptablePluginObject*)m_pScriptableObject)->_plugin = this;
  }

  return m_pScriptableObject;
}

void
CPlugin::handleEvent(const std::string &target, const std::string &eventName, const std::string &data)
{
    if (m_pScriptableObject)
    {
        osg::notify(osg::INFO) << "CPlugin::handleEvent: " << target << ", " << eventName << ", " << data << std::endl;

        //Get the window object
        /*NPObject* windowObj;
        NPN_GetValue(m_pNPInstance, NPNVWindowNPObject, &windowObj);
        osg::notify(osg::INFO) << "Got window obj" << std::endl;

        //Get the document
        NPVariant docv;
        NPN_GetProperty(m_pNPInstance, windowObj, sDocument_id, &docv);
        
        NPObject* doc = NPVARIANT_TO_OBJECT(docv);
        if (doc) osg::notify(osg::INFO) << "Got document" << std::endl;
        
        //Create the event
        NPVariant strv;
        STRINGZ_TO_NPVARIANT("Event", strv);
        NPVariant eventv;
        NPN_Invoke(m_pNPInstance, doc, sCreateEvent_id, &strv, 1, &eventv);

        NPObject* evt = NPVARIANT_TO_OBJECT(eventv);
        //if (evt) osg::notify(osg::NOTICE) << "Created event " << std::endl;

        //Initialize the event
        {
            NPVariant resultv;
            NPVariant args[3];
            STRINGZ_TO_NPVARIANT("onreadymapevent", args[0]);
            BOOLEAN_TO_NPVARIANT(true, args[1]);
            BOOLEAN_TO_NPVARIANT(true, args[2]);
            //Initialize the event
            NPN_Invoke(m_pNPInstance, evt, sInitEvent_id, args,3,&resultv);
            NPN_ReleaseVariantValue(&resultv);
            osg::notify(osg::INFO) << "Initialized event" << std::endl;
        }

        //Dispatch the event
        {
            NPVariant resultv;
            NPN_Invoke(m_pNPInstance, doc, sDispatchEvent_id, &eventv, 1, &resultv);
            NPN_ReleaseVariantValue(&resultv);
            osg::notify(osg::INFO) << "Dispatched event" << std::endl;
        }

        NPN_ReleaseVariantValue(&eventv);              
        NPN_ReleaseVariantValue(&docv);*/

        //Get the document so we can create an event


        NPVariant args[3];
        char* targetStr = (char*)NPN_MemAlloc(target.length() + 1);
        char* eventNameStr = (char*)NPN_MemAlloc(eventName.length()) + 1;
        char* dataStr = (char*)NPN_MemAlloc(data.length() + 1);

        strcpy(targetStr, target.c_str());
        strcpy(eventNameStr, eventName.c_str());
        strcpy(dataStr, data.c_str());

        
        STRINGZ_TO_NPVARIANT(targetStr, args[0]);
        STRINGZ_TO_NPVARIANT(eventNameStr, args[1]);
        STRINGZ_TO_NPVARIANT(dataStr, args[2]);
        
        //osg::notify(osg::NOTICE) << "Calling NPN_InvokeDefault" << std::endl;
        NPVariant result;
        NPN_InvokeDefault(m_pNPInstance, ((ScriptablePluginObject*)m_pScriptableObject)->_readyMapEventHandler,
                          args, 3, &result);
        //osg::notify(osg::NOTICE) << "Calling NPN_InvokeDefault" << std::endl;

        //osg::notify(osg::NOTICE) << "Releasing Variant Values" << std::endl;
        NPN_ReleaseVariantValue(&result);

        NPN_ReleaseVariantValue(&args[0]);
        NPN_ReleaseVariantValue(&args[1]);
        NPN_ReleaseVariantValue(&args[2]);
        //osg::notify(osg::NOTICE) << "Finished Releasing Variant Values" << std::endl;
    }
}
