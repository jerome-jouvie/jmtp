/*
 * Copyright 2010 Pieter De Rycke
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
#include <PortableDeviceApi.h>

#include "jmtp.h"
#include "jmtp_PortableDevice.h"

static inline IPortableDevice* GetPortableDevice(JNIEnv* env, jobject obj) {
	return (IPortableDevice*)GetComReferencePointer(env, obj, rt.pdDeviceField);
}

static inline IPortableDeviceManager* GetPortableDeviceManager(JNIEnv* env, jobject obj) {
	return (IPortableDeviceManager*)GetComReferencePointer(env, obj, rt.pdDevicePDManager);
}

JNIEXPORT jstring JNICALL Java_jmtp_PortableDevice_getDeviceFriendlyName
	(JNIEnv* env, jobject obj, jstring deviceID) {
	IPortableDeviceManager* pDeviceManager;
	LPWSTR wszDeviceID;
	LPWSTR wszDeviceFriendlyName;
	DWORD length;
	jstring friendlyName;

	pDeviceManager = GetPortableDeviceManager(env, obj);
	wszDeviceID = (WCHAR*)env->GetStringChars(deviceID, NULL);

	pDeviceManager->GetDeviceFriendlyName(wszDeviceID, NULL, &length);
	wszDeviceFriendlyName = new WCHAR[length + 1];
	pDeviceManager->GetDeviceFriendlyName(wszDeviceID, wszDeviceFriendlyName, &length);	
	friendlyName = NewStringFromLPWSTR(env, wszDeviceFriendlyName);

	env->ReleaseStringChars(deviceID, (jchar*)wszDeviceID);
	delete wszDeviceFriendlyName;

	return friendlyName;
}

JNIEXPORT jstring JNICALL Java_jmtp_PortableDevice_getDeviceManufacturer
	(JNIEnv* env, jobject obj, jstring deviceID) {
	IPortableDeviceManager* pDeviceManager;
	LPWSTR wszDeviceID;
	LPWSTR wszDeviceManufacturer;
	DWORD length;
	jstring manufacturer;

	pDeviceManager = GetPortableDeviceManager(env, obj);
	wszDeviceID = (WCHAR*)env->GetStringChars(deviceID, NULL);

	pDeviceManager->GetDeviceManufacturer(wszDeviceID, NULL, &length);
	wszDeviceManufacturer = new WCHAR[length + 1];
	pDeviceManager->GetDeviceManufacturer(wszDeviceID, wszDeviceManufacturer, &length);	
	manufacturer = NewStringFromLPWSTR(env, wszDeviceManufacturer);

	env->ReleaseStringChars(deviceID, (jchar*)wszDeviceID);
	delete wszDeviceManufacturer;

	return manufacturer;
}

JNIEXPORT jstring JNICALL Java_jmtp_PortableDevice_getDeviceDescription
	(JNIEnv* env, jobject obj, jstring deviceID) {
	IPortableDeviceManager* pDeviceManager;
	LPWSTR wszDeviceID;
	LPWSTR wszDeviceDescription;
	DWORD length;
	jstring description;

	pDeviceManager = GetPortableDeviceManager(env, obj);
	wszDeviceID = (WCHAR*)env->GetStringChars(deviceID, NULL);

	pDeviceManager->GetDeviceDescription(wszDeviceID, NULL, &length);
	wszDeviceDescription = new WCHAR[length + 1];
	pDeviceManager->GetDeviceDescription(wszDeviceID, wszDeviceDescription, &length);	
	description = NewStringFromLPWSTR(env, wszDeviceDescription);

	env->ReleaseStringChars(deviceID, (jchar*)wszDeviceID);
	delete wszDeviceDescription;

	return description;
}

JNIEXPORT void JNICALL Java_jmtp_PortableDevice_openImpl
	(JNIEnv* env, jobject obj, jobject values) {
	HRESULT hr;
	IPortableDevice* pDevice;
	IPortableDeviceValues* pClientInfo;
	LPWSTR wszDeviceID;
	jstring jsDeviceID;

	pDevice = GetPortableDevice(env, obj);

	//clientinfo value object opvragen
	jobject reference = env->CallObjectMethod(values, rt.pdValuesGetReference);
	pClientInfo = (IPortableDeviceValues*)ConvertComReferenceToPointer(env, reference);

	//deviceID opvragen
	jsDeviceID = (jstring)env->GetObjectField(obj, rt.pdDeviceId);
	wszDeviceID = (WCHAR*)env->GetStringChars(jsDeviceID, NULL);

	hr = pDevice->Open(wszDeviceID, pClientInfo);
	env->ReleaseStringChars(jsDeviceID, (jchar*)wszDeviceID);

	if (FAILED(hr)) {
		ThrowCOMException(env, L"Couldn't open the device", hr);
		return;
	}
}

JNIEXPORT void JNICALL Java_jmtp_PortableDevice_closeImpl
	(JNIEnv* env, jobject obj) {
	HRESULT hr;
	IPortableDevice* pDevice;

	pDevice = GetPortableDevice(env, obj);

	hr = pDevice->Close();

	if (FAILED(hr)) {
		ThrowCOMException(env, L"Couldn't close the device", hr);
		return;
	}
}

JNIEXPORT jobject JNICALL Java_jmtp_PortableDevice_getDeviceContent
	(JNIEnv* env, jobject obj) {
	HRESULT hr;
	IPortableDevice* pDevice;
	IPortableDeviceContent* pContent;

	pDevice = GetPortableDevice(env, obj);
	hr = pDevice->Content(&pContent);
	if (FAILED(hr)) {
		ThrowCOMException(env, L"Couldn't retrieve the device content", hr);
		return NULL;
	}

	//smart reference object aanmaken
	jobject reference = env->NewObject(rt.comRefClass, rt.comRefInit, pContent);
	return env->NewObject(rt.pdContentClass, rt.pdContentInit, reference);
}

JNIEXPORT jobject JNICALL Java_jmtp_PortableDevice_sendCommand
	(JNIEnv* env, jobject obj, jobject values) {
	HRESULT hr;
	IPortableDevice* pDevice;
	IPortableDeviceValues* pValuesIn;
	IPortableDeviceValues* pValuesOut;
	jobject reference;

	pDevice = GetPortableDevice(env, obj);

	//clientinfo value object opvragen
	reference = env->CallObjectMethod(values, rt.pdValuesGetReference);
	pValuesIn = (IPortableDeviceValues*)ConvertComReferenceToPointer(env, reference);

	hr = pDevice->SendCommand(0, pValuesIn, &pValuesOut);

	if (FAILED(hr)) {
		ThrowCOMException(env, L"The custom command failed.", hr);
		return NULL;
	}

	//smart reference object aanmaken
	reference = env->NewObject(rt.comRefClass, rt.comRefInit, pValuesOut);
	return env->NewObject(rt.pdValuesClass, rt.pdValuesInit, reference);
}
