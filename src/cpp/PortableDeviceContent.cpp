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
#include <list>

#include "jmtp.h"
#include "jmtp_PortableDeviceContent.h"

static inline IPortableDeviceContent* GetPortableDeviceContent(JNIEnv* env, jobject obj) {
	return (IPortableDeviceContent*)GetComReferencePointer(env, obj, rt.pdContentField);
}

JNIEXPORT jstring JNICALL Java_jmtp_PortableDeviceContent_createObjectWithPropertiesAndData
	(JNIEnv* env, jobject obj, jobject jobjValues, jobject jobjFile) {
	HRESULT hr;
	IPortableDeviceContent* pDeviceContent;
	IPortableDeviceValues* pDeviceObjectValues;
	jobject jobjValuesReference;
	jstring jsFileLocation;
	LPWSTR wszFileLocation;
	DWORD dwBufferSize;
	CComPtr<IStream> pFileStream;
	CComPtr<IStream> pDeviceStream;
	CComPtr<IPortableDeviceDataStream> pDeviceDataStream;
	STATSTG fileStats;
	BYTE* pBuffer;
	DWORD dwReadFromStream;
	LPWSTR wszObjectID;
	jstring jsObjectID;

	pDeviceContent = GetPortableDeviceContent(env, obj);
	jobjValuesReference = RetrieveCOMReferenceFromCOMReferenceable(env, jobjValues);
	pDeviceObjectValues = (IPortableDeviceValues*)ConvertComReferenceToPointer(env, jobjValuesReference);
	
	//COM stream object aanmaken
	jsFileLocation = (jstring)env->CallObjectMethod(jobjFile, rt.fileGetAbsolutePath);
	wszFileLocation = (WCHAR*)env->GetStringChars(jsFileLocation, NULL);
	hr = SHCreateStreamOnFileW(wszFileLocation, STGM_READ, &pFileStream);
	env->ReleaseStringChars(jsFileLocation, (jchar*)wszFileLocation); //string resources terug vrijgeven

	if (SUCCEEDED(hr)) {
		//groote van het bestand bepalen
		//(door een beperking in java op het gebied van unsigned integers, moeten we het wel in c++ doen)
		pFileStream->Stat(&fileStats, STATFLAG_NONAME);
		pDeviceObjectValues->SetUnsignedLargeIntegerValue(WPD_OBJECT_SIZE, fileStats.cbSize.QuadPart);

		hr = pDeviceContent->CreateObjectWithPropertiesAndData(pDeviceObjectValues, 
			&pDeviceStream, &dwBufferSize, NULL);
		
		if (SUCCEEDED(hr)) {
			pDeviceStream->QueryInterface(IID_IPortableDeviceDataStream, (void**)&pDeviceDataStream);

			//data kopieren
			pBuffer = new BYTE[dwBufferSize];
			dwReadFromStream = 0;
			do {
				pFileStream->Read(pBuffer, dwBufferSize, &dwReadFromStream);
				pDeviceDataStream->Write(pBuffer, dwReadFromStream, NULL);
			}
			while(dwReadFromStream > 0);
			delete[] pBuffer;
			hr = pDeviceDataStream->Commit(STGC_DEFAULT);

			if (SUCCEEDED(hr)) {
				pDeviceDataStream->GetObjectID(&wszObjectID);
				jsObjectID = NewStringFromLPWSTR(env, wszObjectID);
				CoTaskMemFree(wszObjectID);
				return jsObjectID;
			} else {
				ThrowCOMException(env, L"Couldn't commit the data to the portable device", hr);
			}
		} else {
			ThrowCOMException(env, L"Couldn't create a COM stream object to the portable device", hr);
		}
	} else {
		ThrowCOMException(env, L"Couldn't create a COM stream object to the file", hr);
	}

	return NULL;
}

JNIEXPORT jstring JNICALL Java_jmtp_PortableDeviceContent_createObjectWithPropertiesOnly
	(JNIEnv* env, jobject obj, jobject jobjValues) {
	HRESULT hr;
	IPortableDeviceContent* pDeviceContent;
	IPortableDeviceValues* pValues;
	LPWSTR wszObjectID;
	jstring jsObjectID;

	if (jobjValues != NULL) {
		pDeviceContent = GetPortableDeviceContent(env, obj);
		pValues = (IPortableDeviceValues*)GetComReferencePointerFromComReferenceable(env, jobjValues);
		hr = pDeviceContent->CreateObjectWithPropertiesOnly(pValues, &wszObjectID);

		if (SUCCEEDED(hr)) {
			jsObjectID = NewStringFromLPWSTR(env, wszObjectID);
			CoTaskMemFree(wszObjectID);
			return jsObjectID;
		} else {
			ThrowCOMException(env, L"Couldn't create the file", hr);
			return NULL;
		}
	} else {
		env->ThrowNew(rt.npeEx, "values can't be null");
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_jmtp_PortableDeviceContent_delete
	(JNIEnv* env, jobject obj, jint jiOptions, jobject jobjObjectIDs) {
	HRESULT hr;
	IPortableDeviceContent* pDeviceContent;
	IPortableDevicePropVariantCollection* pObjectIDs;

	if (jiOptions == 0 || jiOptions == 1) {
		pDeviceContent = GetPortableDeviceContent(env, obj);
		pObjectIDs = (IPortableDevicePropVariantCollection*)GetComReferencePointerFromComReferenceable(env, jobjObjectIDs);
		hr = pDeviceContent->Delete(jiOptions, pObjectIDs, NULL);

		if (FAILED(hr)) {
			ThrowCOMException(env, L"Failed to delete the files", hr);
			return;
		}
	} else {
		env->ThrowNew(rt.illegalArgEx, "The parameter options has an invalid value.");
		return;
	}
}

JNIEXPORT jobjectArray JNICALL Java_jmtp_PortableDeviceContent_listChildObjects
	(JNIEnv* env, jobject obj, jstring jsParentID) {
	HRESULT hr;
	IPortableDeviceContent* pDeviceContent;
	LPWSTR wszParentID;
	CComPtr<IEnumPortableDeviceObjectIDs> pEnum;
	std::list<LPWSTR> childList;
	std::list<LPWSTR>::iterator iterator;
	LPWSTR* wszObjectID;
	ULONG fetched;


	pDeviceContent = GetPortableDeviceContent(env, obj);
	wszParentID = (WCHAR*)env->GetStringChars(jsParentID, NULL);
	hr = pDeviceContent->EnumObjects(0, wszParentID, NULL, &pEnum);
	env->ReleaseStringChars(jsParentID, (jchar*)wszParentID);
	
	if (SUCCEEDED(hr)) {
		wszObjectID = new LPWSTR[1];
		while(SUCCEEDED(pEnum->Next(1, wszObjectID, &fetched)) && fetched > 0) {
			childList.push_back(wszObjectID[0]);
		}
		delete[] wszObjectID;

		jobjectArray jobjaChildArray = env->NewObjectArray(childList.size(), rt.stringClass, NULL);
		int i = 0;
		for (iterator = childList.begin(); iterator != childList.end(); iterator++) {
			env->SetObjectArrayElement(jobjaChildArray, i, NewStringFromLPWSTR(env, *iterator));
			CoTaskMemFree(*iterator);
			i++;
		}

		return jobjaChildArray;
	} else {
		ThrowCOMException(env, L"Failed to retrieve the enumerator", hr);
		return NULL;
	}
}

JNIEXPORT jobject JNICALL Java_jmtp_PortableDeviceContent_getProperties
	(JNIEnv* env, jobject obj) {
	HRESULT hr;
	IPortableDeviceContent* pContent;
	IPortableDeviceProperties* pProperties;

	pContent = GetPortableDeviceContent(env, obj);
	hr = pContent->Properties(&pProperties);

	if (SUCCEEDED(hr)) {
		//smart reference object aanmaken
		jobject jobjReference = env->NewObject(rt.comRefClass, rt.comRefInit, pProperties);
		return env->NewObject(rt.pdPropertiesClass, rt.pdPropertiesInit, jobjReference);
	} else {
		ThrowCOMException(env, L"Failed to retrieve the property object", hr);
		return NULL;
	}
}

JNIEXPORT jobject JNICALL Java_jmtp_PortableDeviceContent_getObjectIDsFromPersistentUniqueIDs
	(JNIEnv* env, jobject obj, jobject jobjPropVariantCollection) {
	HRESULT hr;
	IPortableDeviceContent* pContent;
	IPortableDevicePropVariantCollection* pPersistentUniqueIDs;
	IPortableDevicePropVariantCollection* pObjectIDs;

	if (jobjPropVariantCollection != NULL) {
		pContent = GetPortableDeviceContent(env, obj);
		pPersistentUniqueIDs =
			(IPortableDevicePropVariantCollection*)GetComReferencePointerFromComReferenceable(env, jobjPropVariantCollection);

		hr = pContent->GetObjectIDsFromPersistentUniqueIDs(pPersistentUniqueIDs, &pObjectIDs);

		if (SUCCEEDED(hr)) {
			//smart reference object aanmaken
			jobject jobjReference = env->NewObject(rt.comRefClass, rt.comRefInit, pObjectIDs);
			return env->NewObject(rt.pdPropVariantCollectionClass, rt.pdPropVariantCollectionInit, jobjReference);
		} else {
			ThrowCOMException(env, L"Failed to retrieve the ObjectIDs", hr);
			return NULL;
		}
	} else {
		env->ThrowNew(rt.npeEx, "persistentUniqueIDs can't be null");
		return NULL;
	}
}