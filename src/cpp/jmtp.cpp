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
#include <atlbase.h>
#include <jni.h>

#include "jmtp.h"

#define JNI_CLASS(x) (jclass) env->NewGlobalRef(env->FindClass(x))
#define JNI_CLASS_DELETE(x) env->DeleteGlobalRef(x)

ClassAndMethods rt;

// ON JVM load/unload
void LoadClassAndMethods(JNIEnv* env) {
	jclass booleanClass = env->FindClass("java/lang/Boolean");
	rt.stringClass = JNI_CLASS("java/lang/String");
	rt.bigIntClass = JNI_CLASS("java/math/BigInteger");
	jclass fileClass = env->FindClass("java/io/File");
	rt.illegalArgEx = JNI_CLASS("java/lang/IllegalArgumentException");
	rt.npeEx = JNI_CLASS("java/lang/NullPointerException");
	rt.ioobEx = JNI_CLASS("java/lang/IndexOutOfBoundsException");

	rt.comRefClass = JNI_CLASS("be/derycke/pieter/com/COMReference");
	jclass comReferenceableClass = env->FindClass("be/derycke/pieter/com/COMReferenceable");
	rt.comExceptionClass = JNI_CLASS("be/derycke/pieter/com/COMException");

	rt.guidClass = JNI_CLASS("be/derycke/pieter/com/Guid");
	jclass oleDateClass = env->FindClass("be/derycke/pieter/com/OleDate");

	jclass pdManagerClass = env->FindClass("jmtp/PortableDeviceManager");
	rt.pdClass = JNI_CLASS("jmtp/PortableDevice");
	rt.pdContentClass = JNI_CLASS("jmtp/PortableDeviceContent");
	rt.pdKeyCollectionClass = JNI_CLASS("jmtp/PortableDeviceKeyCollection");
	rt.pdPropertiesClass = JNI_CLASS("jmtp/PortableDeviceProperties");
	rt.pdPropVariantCollectionClass = JNI_CLASS("jmtp/PortableDevicePropVariantCollection");
	rt.pdValuesClass = JNI_CLASS("jmtp/PortableDeviceValues");
	rt.propertyKeyClass = JNI_CLASS("jmtp/PropertyKey");
	rt.propVariantClass = JNI_CLASS("jmtp/PropVariant");
	jclass pdTransfertClass = env->FindClass("jmtp/PortableDeviceManager");

	rt.booleanValue = env->GetMethodID(booleanClass, "booleanValue", "()Z");
	rt.bigIntInit = env->GetMethodID(rt.bigIntClass, "<init>", "([B)V");
	rt.bigIntSignum = env->GetMethodID(rt.bigIntClass, "signum", "()I");
	rt.bigIntToByteArray = env->GetMethodID(rt.bigIntClass, "toByteArray", "()[B");
	rt.fileGetAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
	rt.comExceptionInit = env->GetMethodID(rt.comExceptionClass, "<init>", "(Ljava/lang/String;I)V");
	rt.guidInit = env->GetMethodID(rt.guidClass, "<init>", "(JII[S)V");
	rt.guidGetData1 = env->GetMethodID(rt.guidClass, "getData1", "()J");
	rt.guidGetData2 = env->GetMethodID(rt.guidClass, "getData2", "()I");
	rt.guidGetData3 = env->GetMethodID(rt.guidClass, "getData3", "()I");
	rt.guidGetData4 = env->GetMethodID(rt.guidClass, "getData4", "()[S");
	rt.oleDateSet = env->GetMethodID(oleDateClass, "set", "(IIIIII)V");
	rt.oleDateGet = env->GetMethodID(oleDateClass, "get", "(I)I");
	rt.comRefInit = env->GetMethodID(rt.comRefClass, "<init>", "(J)V");
	rt.comRefGetMemoryAddress = env->GetMethodID(rt.comRefClass, "getMemoryAddress", "()J");
	rt.pdContentInit = env->GetMethodID(rt.pdContentClass, "<init>", "(Lbe/derycke/pieter/com/COMReference;)V");
	rt.pdPropertiesInit = env->GetMethodID(rt.pdPropertiesClass, "<init>", "(Lbe/derycke/pieter/com/COMReference;)V");
	rt.pdPropVariantCollectionInit = env->GetMethodID(rt.pdPropVariantCollectionClass, "<init>", "(Lbe/derycke/pieter/com/COMReference;)V");
	rt.pdValuesInit = env->GetMethodID(rt.pdValuesClass, "<init>", "(Lbe/derycke/pieter/com/COMReference;)V");
	rt.pdValuesGetReference = env->GetMethodID(rt.pdValuesClass, "getReference", "()Lbe/derycke/pieter/com/COMReference;");
	rt.pdKeyCollectionGetReference = env->GetMethodID(rt.pdKeyCollectionClass, "getReference", "()Lbe/derycke/pieter/com/COMReference;");
	rt.propertyKeyInit = env->GetMethodID(rt.propertyKeyClass, "<init>", "(Lbe/derycke/pieter/com/Guid;J)V");
	rt.propertyKeyGetPid = env->GetMethodID(rt.propertyKeyClass, "getPid", "()J");
	rt.propertyKeyGetFmtid = env->GetMethodID(rt.propertyKeyClass, "getFmtid", "()Lbe/derycke/pieter/com/Guid;");
	rt.propVariantInit = env->GetMethodID(rt.propVariantClass, "<init>", "(Ljava/lang/String;)V");
	rt.propVariantGetVt = env->GetMethodID(rt.propVariantClass, "getVt", "()I");
	rt.propVariantGetValue = env->GetMethodID(rt.propVariantClass, "getValue", "()Ljava/lang/Object;");
	rt.comReferenceableGetReference = env->GetMethodID(comReferenceableClass, "getReference", "()Lbe/derycke/pieter/com/COMReference;");

	const char* COM_REFERENCE_FIELD = "Lbe/derycke/pieter/com/COMReference;";
	rt.comRefPIUnknow = env->GetFieldID(rt.comRefClass, "pIUnknown", "J");
	rt.pdDeviceId = env->GetFieldID(rt.pdClass, "deviceID", "Ljava/lang/String;");
	rt.pdDeviceField = env->GetFieldID(rt.pdClass, "pDevice", COM_REFERENCE_FIELD);
	rt.pdDevicePDManager = env->GetFieldID(rt.pdClass, "pDeviceManager", COM_REFERENCE_FIELD);
	rt.pdContentField = env->GetFieldID(rt.pdContentClass, "pDeviceContent", COM_REFERENCE_FIELD);
	rt.pdKeyCollectionField = env->GetFieldID(rt.pdKeyCollectionClass, "pKeyCollection", COM_REFERENCE_FIELD);
	rt.pdManagerField = env->GetFieldID(pdManagerClass, "pDeviceManager", COM_REFERENCE_FIELD);
	rt.pdPropertiesField = env->GetFieldID(rt.pdPropertiesClass, "pDeviceProperties", COM_REFERENCE_FIELD);
	rt.pdValuesField = env->GetFieldID(rt.pdValuesClass, "pDeviceValues", COM_REFERENCE_FIELD);
	rt.pdPropVariantCollectionField = env->GetFieldID(rt.pdPropVariantCollectionClass, "pPropVariantCollection", COM_REFERENCE_FIELD);
}

void UnLoadClassAndMethods(JNIEnv* env) {
	JNI_CLASS_DELETE(rt.stringClass);
	JNI_CLASS_DELETE(rt.bigIntClass);
	JNI_CLASS_DELETE(rt.illegalArgEx);
	JNI_CLASS_DELETE(rt.npeEx);
	JNI_CLASS_DELETE(rt.ioobEx);

	JNI_CLASS_DELETE(rt.comRefClass);
	JNI_CLASS_DELETE(rt.comExceptionClass);

	JNI_CLASS_DELETE(rt.guidClass);

	JNI_CLASS_DELETE(rt.pdClass);
	JNI_CLASS_DELETE(rt.pdContentClass);
	JNI_CLASS_DELETE(rt.pdKeyCollectionClass);
	JNI_CLASS_DELETE(rt.pdPropertiesClass);
	JNI_CLASS_DELETE(rt.pdPropVariantCollectionClass);
	JNI_CLASS_DELETE(rt.pdValuesClass);
	JNI_CLASS_DELETE(rt.propertyKeyClass);
	JNI_CLASS_DELETE(rt.propVariantClass);

	rt = ClassAndMethods();
}

void ThrowCOMException(JNIEnv* env, LPWSTR message, HRESULT hr) {
	jstring jsMessage = NewStringFromLPWSTR(env, message);
	jobject exception = env->NewObject(rt.comExceptionClass, rt.comExceptionInit, jsMessage, (jint)hr);
	env->Throw((jthrowable)exception);
}

jobject ConvertGuidToJava(JNIEnv* env, GUID guid) {
	jshortArray jData4 = env->NewShortArray(8);
	for (int i = 0; i < 8; i++) {
		jshort jsBuffer = guid.Data4[i];
		env->SetShortArrayRegion(jData4, i, 1, &jsBuffer);
	}
	return env->NewObject(rt.guidClass, rt.guidInit, (jlong)guid.Data1, (jint)guid.Data2, (jint)guid.Data3, jData4);
}

GUID ConvertJavaToGuid(JNIEnv* env, jobject jGuid) {
	GUID guid;
	guid.Data1 = static_cast<unsigned long>(env->CallLongMethod(jGuid, rt.guidGetData1));
	guid.Data2 = static_cast<unsigned short>(env->CallIntMethod(jGuid, rt.guidGetData2));
	guid.Data3 = static_cast<unsigned short>(env->CallIntMethod(jGuid, rt.guidGetData3));
	jshort* data4 = env->GetShortArrayElements((jshortArray)env->CallObjectMethod(jGuid, rt.guidGetData4), NULL);
	for (int i = 0; i < 8; i++) {
		guid.Data4[i] = static_cast<unsigned char>(data4[i]);
	}

	return guid;
}

jobject ConvertPropertyKeyToJava(JNIEnv* env, PROPERTYKEY key) {
	return env->NewObject(rt.propertyKeyClass, rt.propertyKeyInit, ConvertGuidToJava(env, key.fmtid), (jlong)key.pid);
}

PROPERTYKEY ConvertJavaToPropertyKey(JNIEnv* env, jobject jKey) {
	PROPERTYKEY key;
	jobject jGuid;

	key.pid = static_cast<DWORD>(env->CallLongMethod(jKey, rt.propertyKeyGetPid));
	jGuid = env->CallObjectMethod(jKey, rt.propertyKeyGetFmtid);
	key.fmtid = ConvertJavaToGuid(env, jGuid);

	return key;
}

jobject ConvertPropVariantToJava(JNIEnv* env, PROPVARIANT pv) {
	if (pv.vt == VT_LPWSTR) {
		jstring jsValue = NewStringFromLPWSTR(env, pv.pwszVal);
		return env->NewObject(rt.propVariantClass, rt.propVariantInit, jsValue);
	} else {
		return NULL;	//andere types dan string in de propvariant worden momenteel niet ondersteunt
	}
}

//nog werk nodig! met de union!!
PROPVARIANT ConvertJavaToPropVariant(JNIEnv* env, jobject jobjPropVariant) {
	PROPVARIANT pv;
	jstring jsValue;
	LPWSTR wszBuffer;
	LPWSTR wszValue;
	jobject jobjObjectValue;

	PropVariantInit(&pv);
	
	pv.vt = static_cast<VARTYPE>(env->CallIntMethod(jobjPropVariant, rt.propVariantGetVt));

	switch(pv.vt) {
		case VT_LPWSTR:
			jsValue = (jstring)env->CallObjectMethod(jobjPropVariant, rt.propVariantGetValue);
			wszBuffer = (WCHAR*)env->GetStringChars(jsValue, NULL);
			wszValue = (WCHAR*)CoTaskMemAlloc(sizeof(WCHAR) * (wcslen(wszBuffer) + 1));
			wcscpy_s(wszValue, wcslen(wszBuffer) + 1, wszBuffer);
			env->ReleaseStringChars(jsValue, (jchar*)wszBuffer);
			pv.pwszVal = wszValue;
			break;
		case VT_BOOL:
			jobjObjectValue = env->CallObjectMethod(jobjPropVariant, rt.propVariantGetValue);
			pv.boolVal = env->CallBooleanMethod(jobjObjectValue, rt.booleanValue);
			break;
	}
	//andere types worden momenteel niet ondersteunt
	
	return pv;
}

ULONGLONG ConvertJavaToUnsignedLongLong(JNIEnv* env, jobject jobjBigInteger) {
	jbyteArray jbaArray;
	jboolean isCopy = JNI_TRUE;
	jbyte* buffer;
	jsize jiLength;
	ULONGLONG result = 0;
	byte* tmp;

	if (env->CallIntMethod(jobjBigInteger, rt.bigIntSignum) >= 0) {
		jbaArray = (jbyteArray)env->CallObjectMethod(jobjBigInteger, rt.bigIntToByteArray);

		//van big endian naar little endian converteren
		jiLength = env->GetArrayLength(jbaArray);
		buffer = env->GetByteArrayElements(jbaArray, &isCopy);

		//i < 8 is omdat we met een 64 bit getal werken
		tmp = new byte[sizeof(ULONGLONG)];
		for(int i = 0; i < sizeof(ULONGLONG); i++) {
			if (i < jiLength) {
				tmp[i] = buffer[jiLength - i - 1];
			} else {
				tmp[i] = 0;
			}
		}
		memcpy_s(&result, sizeof(ULONGLONG), tmp, sizeof(ULONGLONG));
		delete tmp;

		env->ReleaseByteArrayElements(jbaArray, buffer, JNI_ABORT);

		return result;
	} else {
		env->ThrowNew(rt.illegalArgEx, "Only positive integers can be converted.");
		return -1;	//-> wat doen we bij negatieve getallen?
	}
}

jobject ConvertUnsignedLongLongToJava(JNIEnv* env, ULONGLONG number) {
	jbyteArray buffer = env->NewByteArray(9);
	for (int i = 0; i < 8; i++) {
		env->SetByteArrayRegion(buffer, (i + 1), 1, (jbyte*)((byte*)&number + 7 - i));
	}
	return env->NewObject(rt.bigIntClass, rt.bigIntInit, buffer);
}

inline jlong ConvertComReferenceToPointer(JNIEnv* env, jobject jobjReference) {
	return env->CallLongMethod(jobjReference, rt.comRefGetMemoryAddress);
}

//De COMReference opvragen van een COMReferencable object
inline jobject RetrieveCOMReferenceFromCOMReferenceable(JNIEnv* env, jobject jobjCOMReferenceable) {
	return env->CallObjectMethod(jobjCOMReferenceable, rt.comReferenceableGetReference);
}

jlong GetComReferencePointer(JNIEnv* env, jobject obj, jfieldID field) {
	jobject reference = env->GetObjectField(obj, field);
	return ConvertComReferenceToPointer(env, reference);
}

jlong GetComReferencePointerFromComReferenceable(JNIEnv* env, jobject jobjCOMReferenceable) {
	jobject jobjReference = RetrieveCOMReferenceFromCOMReferenceable(env, jobjCOMReferenceable);
	return ConvertComReferenceToPointer(env, jobjReference);
}
