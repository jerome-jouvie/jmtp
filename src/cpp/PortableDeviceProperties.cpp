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

#include <PortableDeviceApi.h>
#include <PortableDevice.h>

#include "jmtp.h"
#include "jmtp_PortableDeviceProperties.h"

inline IPortableDeviceProperties* GetPortableDeviceProperties(JNIEnv* env, jobject obj) {
	return (IPortableDeviceProperties*)GetComReferencePointer(env, obj, rt.pdPropertiesField);
}

JNIEXPORT jobject JNICALL Java_jmtp_PortableDeviceProperties_getPropertyAttributes
	(JNIEnv* env, jobject obj, jstring jsObjectID, jobject key) {
	HRESULT hr;
	IPortableDeviceProperties* pProperties;
	IPortableDeviceValues* pValues;
	LPWSTR wszObjectID;

	pProperties = GetPortableDeviceProperties(env, obj);
	wszObjectID = (WCHAR*)env->GetStringChars(jsObjectID, NULL);
	hr = pProperties->GetPropertyAttributes(L"DEVICE", WPD_RESOURCE_ATTRIBUTE_CAN_DELETE, &pValues);
	env->ReleaseStringChars(jsObjectID,(jchar*)wszObjectID);

	if (FAILED(hr)) {
		ThrowCOMException(env, L"Failed to retrieve the property", hr);
	}

	//smart reference object aanmaken
	jobject reference = env->NewObject(rt.comRefClass, rt.comRefInit, pValues);
	return env->NewObject(rt.pdValuesClass, rt.pdValuesInit, reference);
}

JNIEXPORT jobject JNICALL Java_jmtp_PortableDeviceProperties_getValues
	(JNIEnv* env, jobject obj, jstring jsObjectID, jobject jobjKeyCollection) {
	HRESULT hr;
	IPortableDeviceProperties* pProperties;
	IPortableDeviceValues* pValues;
	IPortableDeviceKeyCollection* pKeyCollection;
	LPWSTR wszObjectID;

	pProperties = GetPortableDeviceProperties(env, obj);
	jobject jobjKeyCollectionReference = env->CallObjectMethod(jobjKeyCollection, rt.pdKeyCollectionGetReference);
	pKeyCollection = (IPortableDeviceKeyCollection*)ConvertComReferenceToPointer(env, jobjKeyCollectionReference);
	wszObjectID = (WCHAR*)env->GetStringChars(jsObjectID, NULL);

	hr = pProperties->GetValues(wszObjectID, pKeyCollection, &pValues);
	env->ReleaseStringChars(jsObjectID,(jchar*)wszObjectID);

	if (FAILED(hr)) {
		ThrowCOMException(env, L"Failed to retrieve the properties", hr);
		return NULL;
	}

	//smart reference object aanmaken
	jobject jobjReference = env->NewObject(rt.comRefClass, rt.comRefInit, pValues);
	return env->NewObject(rt.pdValuesClass, rt.pdValuesInit, jobjReference);
}

JNIEXPORT jobject JNICALL Java_jmtp_PortableDeviceProperties_setValues
	(JNIEnv* env, jobject obj, jstring jsObjectID, jobject jobjValues) {
	HRESULT hr;
	IPortableDeviceProperties* pProperties;
	IPortableDeviceValues* pValues;
	IPortableDeviceValues* pResults;
	LPWSTR wszObjectID;
	jobject jobjReference;

	if (jsObjectID != NULL && jobjValues != NULL) {
		pProperties = GetPortableDeviceProperties(env, obj);

		jobjReference = RetrieveCOMReferenceFromCOMReferenceable(env, jobjValues);
		pValues = (IPortableDeviceValues*)ConvertComReferenceToPointer(env, jobjReference);
		
		wszObjectID = (WCHAR*)env->GetStringChars(jsObjectID, NULL);
		hr = pProperties->SetValues(wszObjectID, pValues, &pResults);
		env->ReleaseStringChars(jsObjectID, (jchar*)wszObjectID);

		if (SUCCEEDED(hr)) {
			//smart reference object aanmaken
			jobjReference = env->NewObject(rt.comRefClass, rt.comRefInit, pResults);
			return env->NewObject(rt.pdValuesClass, rt.pdValuesInit, jobjReference);
		} else {
			ThrowCOMException(env, L"Couldn't change the properties of the element", hr);
			return NULL;
		}
	} else {
		env->ThrowNew(rt.npeEx, "arguments can't be null");
		return NULL;
	}
}