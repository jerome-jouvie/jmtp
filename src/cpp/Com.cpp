/*
 * Copyright 2007 Pieter De Rycke
 * 
 * This file is part of JMTP.
 * 
 * JTMP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation, either version 3 of 
 * the License, or any later version.
 * 
 * JMTP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU LesserGeneral Public 
 * License along with JMTP. If not, see <http://www.gnu.org/licenses/>.
 */

#include <objbase.h>

#include "be_derycke_pieter_com_COM.h"
#include "be_derycke_pieter_com_COMReference.h"

#include "jmtp.h"

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
	CoInitializeEx(NULL, COINIT_MULTITHREADED);
	
	JNIEnv* env;
	if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_OK) {
		LoadClassAndMethods(env);
	}

	return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
	CoUninitialize();

	JNIEnv* env;
	if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) == JNI_OK) {
		UnLoadClassAndMethods(env);
	}
}

JNIEXPORT jobject JNICALL Java_be_derycke_pieter_com_COM_CoCreateInstance
	(JNIEnv* env, jclass guidCls, jobject rclsidObj, jlong pUnkOuter, 
	jlong dwClsContext, jobject riidObj) {
	HRESULT hr;
	IID rclsid;
	IID riid;
	LPVOID reference;

	rclsid = ConvertJavaToGuid(env, rclsidObj);
	riid = ConvertJavaToGuid(env, riidObj);

	//niet 100% want een dword is een unsigned long een een jlong een signed long
	//maar doet het voor nu wel
	hr = CoCreateInstance(rclsid, (LPUNKNOWN)pUnkOuter, (DWORD)dwClsContext, riid, &reference);

	if (SUCCEEDED(hr)) {
		//smart reference object aanmaken
		return env->NewObject(rt.comRefClass, rt.comRefInit, reference);
	} else {
		ThrowCOMException(env, L"Couldn't create the COM-object", hr);
		return NULL;
	}
}

JNIEXPORT jlong JNICALL Java_be_derycke_pieter_com_COMReference_release(JNIEnv* env, jobject obj) {
	LPUNKNOWN pointer = (LPUNKNOWN)env->GetLongField(obj, rt.comRefPIUnknow);
	__try {
		return pointer->Release();
	}
	__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) {
		return 0;
	}
}

JNIEXPORT jlong JNICALL Java_be_derycke_pieter_com_COMReference_addRef(JNIEnv* env, jobject obj) {
	LPUNKNOWN pointer = (LPUNKNOWN)env->GetLongField(obj, rt.comRefPIUnknow);
	__try {
		return pointer->AddRef();
	}
	__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION) {
		return 0;
	}
}