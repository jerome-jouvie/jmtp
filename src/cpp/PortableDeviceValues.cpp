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

#include "jmtp.h"
#include "jmtp_PortableDeviceValues.h"

inline IPortableDeviceValues* GetPortableDeviceValues(JNIEnv* env, jobject obj) {
	return (IPortableDeviceValues*)GetComReferencePointer(env, obj, rt.pdValuesField);
}

JNIEXPORT void JNICALL Java_jmtp_PortableDeviceValues_clear
	(JNIEnv* env, jobject obj) {
	HRESULT hr;
	IPortableDeviceValues* pValues;


	pValues = GetPortableDeviceValues(env, obj);
	hr = pValues->Clear();

	if (FAILED(hr)) {
		ThrowCOMException(env, L"Failed to clear the collection", hr);
	}
}

JNIEXPORT jlong JNICALL Java_jmtp_PortableDeviceValues_count
	(JNIEnv* env, jobject obj) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	DWORD dwCount;

	pValues = GetPortableDeviceValues(env, obj);
	hr = pValues->GetCount(&dwCount);

	if (SUCCEEDED(hr)) {
		return dwCount;
	}
	else {
		ThrowCOMException(env, L"Failed to count the collection", hr);
		return -1;
	}
}

JNIEXPORT void JNICALL Java_jmtp_PortableDeviceValues_setStringValue
	(JNIEnv* env, jobject obj, jobject key, jstring value) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	LPWSTR wszValue;


	if (key != NULL) {
		if (value != NULL) {
			pValues = GetPortableDeviceValues(env, obj);
			wszValue = (WCHAR*)env->GetStringChars(value, NULL);
			hr = pValues->SetStringValue(ConvertJavaToPropertyKey(env, key), wszValue);
			env->ReleaseStringChars(value, (jchar*)wszValue);
			if (FAILED(hr)) {
				ThrowCOMException(env, L"Failed to set the string value", hr);
			}
		} else {
			env->ThrowNew(rt.npeEx, "value can't be null.");
		}
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null.");
	}
}

JNIEXPORT jstring JNICALL Java_jmtp_PortableDeviceValues_getStringValue
	(JNIEnv* env, jobject obj, jobject key) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	LPWSTR wszValue;
	jstring jsValue;

	if (key != NULL) {
		pValues = GetPortableDeviceValues(env, obj);
		hr = pValues->GetStringValue(ConvertJavaToPropertyKey(env, key), &wszValue);

		if (SUCCEEDED(hr)) {
			jsValue = NewStringFromLPWSTR(env, wszValue);
			CoTaskMemFree(wszValue);
			return jsValue;
		} else {
			ThrowCOMException(env, L"Failed to get the string value", hr);
			return NULL;
		}
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null.");
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_jmtp_PortableDeviceValues_setGuidValue
	(JNIEnv* env, jobject obj, jobject key, jobject guid) {
	HRESULT hr;
	IPortableDeviceValues* pValues;

	if (key != NULL) {
		if (guid != NULL) {
			pValues = GetPortableDeviceValues(env, obj);
			hr = pValues->SetGuidValue(ConvertJavaToPropertyKey(env, key), ConvertJavaToGuid(env, guid));

			if (FAILED(hr)) {
				ThrowCOMException(env, L"Failed to set the Guid value", hr);
				return;
			}
		} else {
			env->ThrowNew(rt.npeEx, "guid can't be null.");
		}
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null.");
	}
}

JNIEXPORT jobject JNICALL Java_jmtp_PortableDeviceValues_getGuidValue
	(JNIEnv* env, jobject obj, jobject jobjKey) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	GUID guid;

	if (jobjKey != NULL) {
		pValues = GetPortableDeviceValues(env, obj);
		hr = pValues->GetGuidValue(ConvertJavaToPropertyKey(env, jobjKey), &guid);

		if (SUCCEEDED(hr)) {
			return ConvertGuidToJava(env, guid);
		} else {
			ThrowCOMException(env, L"Failed to retrieve the Guid value", hr);
			return NULL;
		}		
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null.");
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_jmtp_PortableDeviceValues_setUnsignedIntegerValue
	(JNIEnv* env, jobject obj, jobject key, jlong value) {
	HRESULT hr;
	IPortableDeviceValues* pValues;

	if (key != NULL) {
		if (value >= 0) {
			pValues = GetPortableDeviceValues(env, obj);
			hr = pValues->SetUnsignedIntegerValue(ConvertJavaToPropertyKey(env, key), static_cast<ULONG>(value));
			if (FAILED(hr)) {
				ThrowCOMException(env, L"Failed to set the integer value", hr);
			}
		} else {
			env->ThrowNew(rt.illegalArgEx, "The integer must be possitive");
		}
	} else {
		env->ThrowNew(rt.npeEx, "PropertyKey can't be null");
	}
}

JNIEXPORT jlong JNICALL Java_jmtp_PortableDeviceValues_getUnsignedIntegerValue
	(JNIEnv* env, jobject obj, jobject key) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	ULONG value;

	if (key != NULL) {
		pValues = GetPortableDeviceValues(env, obj);
		hr = pValues->GetUnsignedIntegerValue(ConvertJavaToPropertyKey(env, key), &value);
		if (SUCCEEDED(hr)) {
			return value;
		} else {
			ThrowCOMException(env, L"Failed to retrieve the integer value", hr);
			return -1;
		}
	} else {
		env->ThrowNew(rt.npeEx, "PropertyKey can't be null");
		return -1;
	}
}

JNIEXPORT void JNICALL Java_jmtp_PortableDeviceValues_setPortableDeviceValuesCollectionValue
	(JNIEnv* env, jobject obj, jobject jobjKey, jobject jobjValue) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	IPortableDevicePropVariantCollection* pPropVariantCollection;
	jobject jobjReference;

	if (jobjKey != NULL && jobjValue != NULL) {
		pValues = GetPortableDeviceValues(env, obj);
		jobjReference = RetrieveCOMReferenceFromCOMReferenceable(env, jobjValue);
		pPropVariantCollection = 
			(IPortableDevicePropVariantCollection*)ConvertComReferenceToPointer(env, jobjReference);
		hr = pValues->SetIPortableDevicePropVariantCollectionValue(
			ConvertJavaToPropertyKey(env, jobjKey), pPropVariantCollection);
		if (FAILED(hr)) {
			ThrowCOMException(env, L"Failed to set the propvariant collection", hr);
		}
	} else {
		env->ThrowNew(rt.npeEx, "parameters can't be null");
	}
}

JNIEXPORT jobject JNICALL Java_jmtp_PortableDeviceValues_getPortableDeviceValuesCollectionValue
	(JNIEnv* env, jobject obj, jobject jobjKey) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	IPortableDevicePropVariantCollection* pPropVariantCollection;

	if (jobjKey != NULL) {
		pValues = GetPortableDeviceValues(env, obj);
		hr = pValues->GetIPortableDevicePropVariantCollectionValue(
			ConvertJavaToPropertyKey(env, jobjKey), &pPropVariantCollection);
		if (SUCCEEDED(hr)) {
			jobject jobjReference = env->NewObject(rt.comRefClass, rt.comRefInit, pPropVariantCollection);
			return env->NewObject(rt.pdPropVariantCollectionClass, rt.pdPropVariantCollectionInit, jobjReference);
		} else {
			ThrowCOMException(env, L"Failed to retrieve the propvariant collection", hr);
			return NULL;
		}
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null");
		return NULL;
	}
}

JNIEXPORT jboolean JNICALL Java_jmtp_PortableDeviceValues_getBoolValue
	(JNIEnv* env, jobject obj, jobject jobjKey)
{
	HRESULT hr;
	IPortableDeviceValues* pValues;
	BOOL bValue;

	if (jobjKey != NULL) {
		pValues = GetPortableDeviceValues(env, obj);
		hr = pValues->GetBoolValue(ConvertJavaToPropertyKey(env, jobjKey), &bValue);
		if (SUCCEEDED(hr)) {
			return bValue;
		} else {
			ThrowCOMException(env, L"Failed to retrieve the boolean", hr);
			return NULL;
		}
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null");
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_jmtp_PortableDeviceValues_setFloateValue
	(JNIEnv* env, jobject obj, jobject jobjKey, jfloat jfValue) {
	HRESULT hr;
	IPortableDeviceValues* pValues;

	if (jobjKey != NULL) {
		pValues = GetPortableDeviceValues(env, obj);
		hr = pValues->SetFloatValue(ConvertJavaToPropertyKey(env, jobjKey), jfValue);
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null");
	}
}

JNIEXPORT jfloat JNICALL Java_jmtp_PortableDeviceValues_getFloatValue
	(JNIEnv* env, jobject obj, jobject jobjKey) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	FLOAT value;

	if (jobjKey != NULL) {
		pValues = GetPortableDeviceValues(env, obj);
		hr = pValues->GetFloatValue(ConvertJavaToPropertyKey(env, jobjKey), &value);
		if (SUCCEEDED(hr)) {
			return value;
		} else {
			ThrowCOMException(env, L"Failed to retrieve the float", hr);
			return NULL;
		}
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null");
		return NULL;
	}
}

JNIEXPORT jthrowable JNICALL Java_jmtp_PortableDeviceValues_getErrorValue
	(JNIEnv* env, jobject obj, jobject jobjKey) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	HRESULT error;
	jstring jsMessage;

	if (jobjKey != NULL) {
		pValues = GetPortableDeviceValues(env, obj);
		hr = pValues->GetErrorValue(ConvertJavaToPropertyKey(env, jobjKey), &error);

		if (SUCCEEDED(hr)) {
			jsMessage = NewStringFromLPWSTR(env, L"The request is not supported.");
			return (jthrowable)env->NewObject(rt.comExceptionClass, rt.comExceptionInit, jsMessage, (jint)error);
		} else {
			ThrowCOMException(env, L"Failed to retrieve the error", hr);
			return NULL;
		}
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null");
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_jmtp_PortableDeviceValues_setUnsignedLargeIntegerValue
	(JNIEnv* env, jobject obj, jobject jobjKey, jobject jobjValue) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	ULONGLONG value;

	if (jobjKey != NULL) {
		if (jobjValue != NULL) {
			pValues = GetPortableDeviceValues(env, obj);
			value = ConvertJavaToUnsignedLongLong(env, jobjValue);
			hr = pValues->SetUnsignedLargeIntegerValue(ConvertJavaToPropertyKey(env, jobjKey), value);

			if (FAILED(hr)) {
				ThrowCOMException(env, L"Failed to set the unsigned large integer value.", hr);
			}
		} else {
			env->ThrowNew(rt.npeEx, "value can't be null");
		}
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null");
		return;
	}
}

JNIEXPORT jobject JNICALL Java_jmtp_PortableDeviceValues_getUnsignedLargeIntegerValue
	(JNIEnv* env, jobject obj, jobject jobjKey) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	ULONGLONG value;

	if (jobjKey != NULL) {
		pValues = GetPortableDeviceValues(env, obj);
		hr = pValues->GetUnsignedLargeIntegerValue(ConvertJavaToPropertyKey(env, jobjKey), &value);
		if (SUCCEEDED(hr)) {
			return ConvertUnsignedLongLongToJava(env, value);
		} else {
			ThrowCOMException(env, L"Failed to retrieve the unsigned large integer value.", hr);
			return NULL;
		}
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null");
		return NULL;
	}
}

JNIEXPORT void JNICALL Java_jmtp_PortableDeviceValues_setBufferValue
  (JNIEnv* env, jobject obj, jobject jobjKey, jbyteArray jobjValue) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	jboolean isCopy;
	jsize size;
	jbyte* buffer;

	if (jobjKey != NULL) {
		if (jobjValue != NULL) {
			pValues = GetPortableDeviceValues(env, obj);
			size = env->GetArrayLength(jobjValue);
			buffer = env->GetByteArrayElements(jobjValue, &isCopy);
			hr = pValues->SetBufferValue(ConvertJavaToPropertyKey(env, jobjKey), (BYTE*)buffer, size);
			env->ReleaseByteArrayElements(jobjValue, buffer, JNI_ABORT);	//release java array buffer without copying changes back to java

			if (FAILED(hr)) {
				ThrowCOMException(env, L"Failed to set the buffer value.", hr);
			}
		} else {
			env->ThrowNew(rt.npeEx, "value can't be null");
		}
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null");
	}
}

JNIEXPORT jbyteArray JNICALL Java_jmtp_PortableDeviceValues_getBufferValue
  (JNIEnv* env, jobject obj, jobject jobjKey) {
	HRESULT hr;
	IPortableDeviceValues* pValues;
	DWORD size;
	jbyteArray value;
	BYTE* buffer;
	jbyte* jbuffer;

	if (jobjKey != NULL) {
		pValues = GetPortableDeviceValues(env, obj);
		hr = pValues->GetBufferValue(ConvertJavaToPropertyKey(env, jobjKey), (BYTE**) &buffer, &size);
		if (SUCCEEDED(hr)) {
			value = env->NewByteArray(size);
			jbuffer = env->GetByteArrayElements(value, NULL);
			memcpy(jbuffer, buffer, size);

			//release resources
			env->ReleaseByteArrayElements(value, jbuffer, 0);	//copy back the content to the JVM and free the buffer 
			CoTaskMemFree(buffer);

			return value;
		} else {
			ThrowCOMException(env, L"Failed to retrieve the buffer value.", hr);
			return NULL;
		}
	} else {
		env->ThrowNew(rt.npeEx, "key can't be null");
		return NULL;
	}
}
