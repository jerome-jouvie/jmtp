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

#pragma once

#include <objbase.h>
#include <atlbase.h>
#include <jni.h>

struct ClassAndMethods {
	jclass stringClass;
	jclass bigIntClass;
	jclass illegalArgEx;
	jclass npeEx;
	jclass ioobEx;

	jclass comRefClass;
	jclass comExceptionClass;

	jclass guidClass;

	jclass pdClass;
	jclass pdContentClass;
	jclass pdKeyCollectionClass;
	jclass pdPropertiesClass;
	jclass pdPropVariantCollectionClass;
	jclass pdValuesClass;
	jclass propertyKeyClass;
	jclass propVariantClass;

	jmethodID booleanValue;
	jmethodID bigIntInit;
	jmethodID bigIntSignum;
	jmethodID bigIntToByteArray;
	jmethodID fileGetAbsolutePath;
	jmethodID comExceptionInit;
	jmethodID guidInit;
	jmethodID guidGetData1;
	jmethodID guidGetData2;
	jmethodID guidGetData3;
	jmethodID guidGetData4;
	jmethodID oleDateSet;
	jmethodID oleDateGet;
	jmethodID comRefInit;
	jmethodID comRefGetMemoryAddress;
	jmethodID pdContentInit;
	jmethodID pdPropertiesInit;
	jmethodID pdPropVariantCollectionInit;
	jmethodID pdValuesInit;
	jmethodID pdValuesGetReference;
	jmethodID pdKeyCollectionGetReference;
	jmethodID propertyKeyInit;
	jmethodID propertyKeyGetPid;
	jmethodID propertyKeyGetFmtid;
	jmethodID propVariantInit;
	jmethodID propVariantGetVt;
	jmethodID propVariantGetValue;
	jmethodID comReferenceableGetReference;

	jfieldID comRefPIUnknow;
	jfieldID pdDeviceId;
	jfieldID pdDeviceField;
	jfieldID pdDevicePDManager;
	jfieldID pdContentField;
	jfieldID pdKeyCollectionField;
	jfieldID pdManagerField;
	jfieldID pdPropertiesField;
	jfieldID pdValuesField;
	jfieldID pdPropVariantCollectionField;
};
extern ClassAndMethods rt;

void LoadClassAndMethods(JNIEnv* env);
void UnLoadClassAndMethods(JNIEnv* env);

void ThrowCOMException(JNIEnv* env, LPWSTR message, HRESULT hr);

jobject ConvertGuidToJava(JNIEnv* env, GUID guid);
GUID ConvertJavaToGuid(JNIEnv* env, jobject jGuid);

jobject ConvertPropertyKeyToJava(JNIEnv* env, PROPERTYKEY key);
PROPERTYKEY ConvertJavaToPropertyKey(JNIEnv* env, jobject jKey);

jobject ConvertPropVariantToJava(JNIEnv* env, PROPVARIANT pv);
PROPVARIANT ConvertJavaToPropVariant(JNIEnv* env, jobject jobjPropVariant);

ULONGLONG ConvertJavaToUnsignedLongLong(JNIEnv* env, jobject jobjBigInteger);
jobject ConvertUnsignedLongLongToJava(JNIEnv* env, ULONGLONG number);

jobject RetrieveCOMReferenceFromCOMReferenceable(JNIEnv* env, jobject jobjCOMReferenceable);
jlong ConvertComReferenceToPointer(JNIEnv* env, jobject jobjReference);
jlong GetComReferencePointer(JNIEnv* env, jobject obj, jfieldID field);
jlong GetComReferencePointerFromComReferenceable(JNIEnv* env, jobject jobjCOMReferenceable);

inline jstring NewStringFromLPWSTR(JNIEnv* env, const LPWSTR& str) {
	return env->NewString((jchar*)str, wcslen(str));
}