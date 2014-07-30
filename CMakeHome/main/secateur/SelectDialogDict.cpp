// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME SelectDialogDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "SelectDialog.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void SelectDialogEntry_Dictionary();
   static void SelectDialogEntry_TClassManip(TClass*);
   static void delete_SelectDialogEntry(void *p);
   static void deleteArray_SelectDialogEntry(void *p);
   static void destruct_SelectDialogEntry(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SelectDialogEntry*)
   {
      ::SelectDialogEntry *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SelectDialogEntry),0);
      static ::ROOT::TGenericClassInfo 
         instance("SelectDialogEntry", "SelectDialog.h", 24,
                  typeid(::SelectDialogEntry), DefineBehavior(ptr, ptr),
                  &SelectDialogEntry_Dictionary, isa_proxy, 4,
                  sizeof(::SelectDialogEntry) );
      instance.SetDelete(&delete_SelectDialogEntry);
      instance.SetDeleteArray(&deleteArray_SelectDialogEntry);
      instance.SetDestructor(&destruct_SelectDialogEntry);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SelectDialogEntry*)
   {
      return GenerateInitInstanceLocal((::SelectDialogEntry*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::SelectDialogEntry*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static void SelectDialogEntry_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SelectDialogEntry*)0x0)->GetClass();
      SelectDialogEntry_TClassManip(theClass);
   }

   static void SelectDialogEntry_TClassManip(TClass* theClass){
      theClass->CreateAttributeMap();
      TDictAttributeMap* attrMap( theClass->GetAttributeMap() );
      attrMap->AddProperty("file_name","SelectDialog.h");
   }

} // end of namespace ROOT

namespace ROOT {
   static void SelectDialog_Dictionary();
   static void SelectDialog_TClassManip(TClass*);
   static void delete_SelectDialog(void *p);
   static void deleteArray_SelectDialog(void *p);
   static void destruct_SelectDialog(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::SelectDialog*)
   {
      ::SelectDialog *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::SelectDialog),0);
      static ::ROOT::TGenericClassInfo 
         instance("SelectDialog", "SelectDialog.h", 38,
                  typeid(::SelectDialog), DefineBehavior(ptr, ptr),
                  &SelectDialog_Dictionary, isa_proxy, 4,
                  sizeof(::SelectDialog) );
      instance.SetDelete(&delete_SelectDialog);
      instance.SetDeleteArray(&deleteArray_SelectDialog);
      instance.SetDestructor(&destruct_SelectDialog);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::SelectDialog*)
   {
      return GenerateInitInstanceLocal((::SelectDialog*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::SelectDialog*)0x0); R__UseDummy(_R__UNIQUE_(Init));

   // Dictionary for non-ClassDef classes
   static void SelectDialog_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::SelectDialog*)0x0)->GetClass();
      SelectDialog_TClassManip(theClass);
   }

   static void SelectDialog_TClassManip(TClass* theClass){
      theClass->CreateAttributeMap();
      TDictAttributeMap* attrMap( theClass->GetAttributeMap() );
      attrMap->AddProperty("file_name","SelectDialog.h");
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrapper around operator delete
   static void delete_SelectDialogEntry(void *p) {
      delete ((::SelectDialogEntry*)p);
   }
   static void deleteArray_SelectDialogEntry(void *p) {
      delete [] ((::SelectDialogEntry*)p);
   }
   static void destruct_SelectDialogEntry(void *p) {
      typedef ::SelectDialogEntry current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SelectDialogEntry

namespace ROOT {
   // Wrapper around operator delete
   static void delete_SelectDialog(void *p) {
      delete ((::SelectDialog*)p);
   }
   static void deleteArray_SelectDialog(void *p) {
      delete [] ((::SelectDialog*)p);
   }
   static void destruct_SelectDialog(void *p) {
      typedef ::SelectDialog current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::SelectDialog

namespace {
  void TriggerDictionaryInitialization_SelectDialogDict_Impl() {
    static const char* headers[] = {
"SelectDialog.h",
0
    };
    static const char* includePaths[] = {
"/afs/cern.ch/sw/lcg/releases/ROOT/6.00.02-1c3e2/x86_64-slc6-gcc48-opt/include",
"/afs/cern.ch/user/k/kheijhof/decayObserver/CMakeHome/main/secateur/",
0
    };
    static const char* fwdDeclCode = 
"class SelectDialogEntry;"
"class SelectDialog;"
;
    static const char* payloadCode = 
"\n"
"#ifndef G__VECTOR_HAS_CLASS_ITERATOR\n"
"  #define G__VECTOR_HAS_CLASS_ITERATOR\n"
"#endif\n"
"\n"
"#define _BACKWARD_BACKWARD_WARNING_H\n"
"#include \"SelectDialog.h\"\n"
"\n"
"#undef  _BACKWARD_BACKWARD_WARNING_H\n"
;
    static const char* classesHeaders[]={
"", payloadCode, "@",
"SelectDialog", payloadCode, "@",
"SelectDialog::ChangedFilter", payloadCode, "@",
"SelectDialog::Cleanup", payloadCode, "@",
"SelectDialog::Close", payloadCode, "@",
"SelectDialog::DelayedClose", payloadCode, "@",
"SelectDialog::PositionWindow", payloadCode, "@",
"SelectDialog::PressedCancel", payloadCode, "@",
"SelectDialog::PressedClose", payloadCode, "@",
"SelectDialog::PressedInvert", payloadCode, "@",
"SelectDialog::PressedSelAll", payloadCode, "@",
"SelectDialog::PressedSelect", payloadCode, "@",
"SelectDialog::SelectDialog", payloadCode, "@",
"SelectDialog::default_height", payloadCode, "@",
"SelectDialog::default_width", payloadCode, "@",
"SelectDialog::map_offset_x", payloadCode, "@",
"SelectDialog::map_offset_y", payloadCode, "@",
"SelectDialog::prev_flag", payloadCode, "@",
"SelectDialog::prev_height", payloadCode, "@",
"SelectDialog::prev_width", payloadCode, "@",
"SelectDialog::prev_x", payloadCode, "@",
"SelectDialog::prev_y", payloadCode, "@",
"SelectDialog::~SelectDialog", payloadCode, "@",
"SelectDialogEntry", payloadCode, "@",
"SelectDialogEntry::SelectDialogEntry", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("SelectDialogDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_SelectDialogDict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_SelectDialogDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_SelectDialogDict() {
  TriggerDictionaryInitialization_SelectDialogDict_Impl();
}
