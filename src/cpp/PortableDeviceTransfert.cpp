#include <string>
#include <atlbase.h>
#include <atlstr.h>

#include <iostream>


#include <PortableDeviceApi.h>
#include <PortableDevice.h>
#include <PortableDeviceApi.h>

#include "jmtp.h"
#include "jmtp_PortableDeviceTransfert.h"

using namespace std;

static inline IPortableDevice* GetPortableDevice(JNIEnv* env, jobject obj) {
	return (IPortableDevice*)GetComReferencePointer(env, obj, rt.pdDeviceField);
}

// Fills out the required properties for ALL content types...
HRESULT FillRequiredPropertiesForFile(
	_In_ IPortableDeviceValues*  objectProperties,
	_In_ PCWSTR                  parentObjectID,
	_In_ PCWSTR                  fileName,
	_In_ ULONGLONG               fileSize)
{
	HRESULT hr;

	// Set the WPD_OBJECT_PARENT_ID
	hr = objectProperties->SetStringValue(WPD_OBJECT_PARENT_ID, parentObjectID);
	if (FAILED(hr)) {
		wprintf(L"! Failed to set WPD_OBJECT_PARENT_ID, hr = 0x%lx\n", hr);
		return hr;
	}

	// Set the WPD_OBJECT_SIZE by requesting the total size of the data stream.
	hr = objectProperties->SetUnsignedLargeIntegerValue(WPD_OBJECT_SIZE, fileSize);
	if (FAILED(hr)) {
		wprintf(L"! Failed to set WPD_OBJECT_SIZE, hr = 0x%lx\n", hr);
		return hr;
	}

	// Set the WPD_OBJECT_ORIGINAL_FILE_NAME by splitting the file path into a separate filename.
	hr = objectProperties->SetStringValue(WPD_OBJECT_ORIGINAL_FILE_NAME, fileName);
	if (FAILED(hr)) {
		wprintf(L"! Failed to set WPD_OBJECT_ORIGINAL_FILE_NAME, hr = 0x%lx\n", hr);
		return hr;
	}

	// Set the WPD_OBJECT_NAME.  We are using the  file name without its file extension in this
	// example for the object's name.  The object name could be a more friendly name like
	// "This Cool Song" or "That Cool Picture".
	hr = objectProperties->SetStringValue(WPD_OBJECT_NAME, fileName);
	if (FAILED(hr)) {
		wprintf(L"! Failed to set WPD_OBJECT_NAME, hr = 0x%lx\n", hr);
		return hr;
	}

	return hr;
}

// Copies data from a source stream to a destination stream using the
// specified cbTransferSize as the temporary buffer size.
HRESULT StreamCopy(IStream *pDestStream, IStream *pSourceStream, DWORD cbTransferSize, DWORD *pcbWritten) {
    // Allocate a temporary buffer (of Optimal transfer size) for the read results to
    // be written to.
	BYTE* pObjectData = new (std::nothrow) BYTE[cbTransferSize];
	if (pObjectData == NULL) {
		printf("! Failed to allocate %d bytes for the temporary transfer buffer.\n", cbTransferSize);
		return E_FAIL;
	}

	HRESULT hr = S_OK;
    DWORD cbTotalBytesRead    = 0;
    DWORD cbTotalBytesWritten = 0;
    DWORD cbBytesRead    = 0;
    DWORD cbBytesWritten = 0;

	// Read until the number of bytes returned from the source stream is 0, or
    // an error occured during transfer.
	do {
        // Read object data from the source stream
        hr = pSourceStream->Read(pObjectData, cbTransferSize, &cbBytesRead);
        if (FAILED(hr)) {
            printf("! Failed to read %d bytes from the source stream, hr = 0x%lx\n", cbTransferSize, hr);
			break;
        }
        // Write object data to the destination stream
		cbTotalBytesRead += cbBytesRead; // Calculating total bytes read from device for debugging purposes only

		hr = pDestStream->Write(pObjectData, cbBytesRead, &cbBytesWritten);
		if (FAILED(hr)) {
			printf("! Failed to write %d bytes of object data to the destination stream, hr = 0x%lx\n", cbBytesRead, hr);
			break;
		}
		cbTotalBytesWritten += cbBytesWritten; // Calculating total bytes written to the file for debugging purposes only
    } while (SUCCEEDED(hr) && (cbBytesRead > 0));

    // If the caller supplied a pcbWritten parameter and we
    // and we are successful, set it to cbTotalBytesWritten
    // before exiting.
    if (SUCCEEDED(hr) && pcbWritten != NULL) {
        *pcbWritten = cbTotalBytesWritten;
    }

    // Remember to delete the temporary transfer buffer
    delete [] pObjectData;
    pObjectData = NULL;

    return hr;
}

std::wstring JavaToWSZ(JNIEnv* env, jstring string) {
    std::wstring value;

    const jchar *raw = env->GetStringChars(string, 0);
    jsize len = env->GetStringLength(string);
    const jchar *temp = raw;

    value.assign(raw, raw + len);

    env->ReleaseStringChars(string, raw);

    return value;
}

JNIEXPORT jboolean JNICALL Java_jmtp_PortableDeviceTransfert_transfertToHost(JNIEnv * env, jclass jcls, jstring jSrcObjectId, jstring jpath, jobject deviceObj) {
	IPortableDevice* pDevice = GetPortableDevice(env, deviceObj);
	if (pDevice == NULL) {
		printf("! A NULL IPortableDevice interface pointer was received\n");
		return false;
	}

	wstring srcObjectId = JavaToWSZ(env, jSrcObjectId);
	wstring dstFile = JavaToWSZ(env, jpath);

	HRESULT hr = S_OK;
	CComPtr<IPortableDeviceContent> pContent;
	CComPtr<IPortableDeviceResources> pResources;
	CComPtr<IStream> pObjectDataStream;
	CComPtr<IStream> pFinalFileStream;
	DWORD cbOptimalTransferSize = 0;

	hr = pDevice->Content(&pContent);
	if (FAILED(hr)) {
		printf("! Failed to get IPortableDeviceContent from IPortableDevice, hr = 0x%lx\n", hr);
		return false;
	}

	hr = pContent->Transfer(&pResources);
	if (FAILED(hr)) {
		printf("! Failed to get IPortableDeviceResources from IPortableDeviceContent, hr = 0x%lx\n", hr);
		return false;
	}

	hr = pResources->GetStream(srcObjectId.c_str(), WPD_RESOURCE_DEFAULT, STGM_READ, &cbOptimalTransferSize, &pObjectDataStream);
	if (FAILED(hr)) {
		printf("! Failed to get IStream (representing object data on the device) from IPortableDeviceResources, hr = 0x%lx\n", hr);
		return false;
	}

	hr = SHCreateStreamOnFileW(dstFile.c_str(), STGM_CREATE | STGM_WRITE, &pFinalFileStream);
	if (FAILED(hr)) {
		printf("! Failed to open a temporary file named (%ws) to transfer object (%ws), hr = 0x%lx\n", dstFile.c_str(), srcObjectId.c_str(), hr);
		return false;
	}

	DWORD cbTotalBytesWritten = 0;
	hr = StreamCopy(pFinalFileStream, pObjectDataStream, cbOptimalTransferSize, &cbTotalBytesWritten);
	
	//pFinalFileStream.Release();

	if (FAILED(hr)) {
		printf("! Failed to transfer object from device, hr = 0x%lx\n", hr);
		return false;
	}

	return true;
}

JNIEXPORT jstring JNICALL Java_jmtp_PortableDeviceTransfert_transfertFromHost
(JNIEnv * env, jclass jcls, jstring jDstParentId, jstring jSrcPath, jstring jFileName, jlong jFileSize, jobject deviceObj) {
	IPortableDevice* pDevice = GetPortableDevice(env, deviceObj);
	if (pDevice == NULL) {
		printf("! A NULL IPortableDevice interface pointer was received\n");
		return NULL;
	}

	wstring parentObjectID = JavaToWSZ(env, jDstParentId);
	wstring srcFile = JavaToWSZ(env, jSrcPath);
	wstring fileName = JavaToWSZ(env, jFileName);
	ULONGLONG fileSize = jFileSize;

	HRESULT hr = S_OK;
	CComPtr<IPortableDeviceContent> pContent;
	CComPtr<IPortableDeviceValues> objectProperties;
	CComPtr<IStream> pDstStream;
	CComPtr<IStream> pSrcStream;
	DWORD cbOptimalTransferSize = 0;
	DWORD totalBytesWritten = 0;

	hr = pDevice->Content(&pContent);
	if (FAILED(hr)) {
		printf("! Failed to get IPortableDeviceContent from IPortableDevice, hr = 0x%lx\n", hr);
		return NULL;
	}

	hr = SHCreateStreamOnFileEx(srcFile.c_str(), STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE, nullptr, &pSrcStream);
	if (FAILED(hr)) {
		printf("! Failed to open a src file (%ws) to transfer to device (%ws), hr = 0x%lx\n", srcFile.c_str(), parentObjectID.c_str(), hr);
		return NULL;
	}

	hr = CoCreateInstance(CLSID_PortableDeviceValues, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&objectProperties));
	if (SUCCEEDED(hr)) {
		hr = FillRequiredPropertiesForFile(objectProperties, parentObjectID.c_str(), fileName.c_str(), fileSize);
	}
	if (FAILED(hr)) {
		wprintf(L"! Failed to create object properties, hr = 0x%lx\n", hr);
		return NULL;
	}

	hr = pContent->CreateObjectWithPropertiesAndData(objectProperties, &pDstStream, &cbOptimalTransferSize, nullptr);
	if (FAILED(hr)) {
		wprintf(L"! Failed to create object in %ws, hr = 0x%lx\n", parentObjectID.c_str(), hr);
		return NULL;
	}

	CComQIPtr<IPortableDeviceDataStream> pDstDataStream(pDstStream);
	if (!pDstDataStream) {
		wprintf(L"! Failed to QueryInterface for IPortableDeviceDataStream, hr = 0x%lx\n", hr);
		return NULL;
	}

	// Since we have IStream-compatible interfaces, call our helper function
	// that copies the contents of a source stream into a destination stream.
	hr = StreamCopy(pDstDataStream, pSrcStream, cbOptimalTransferSize, &totalBytesWritten);
	if (FAILED(hr)) {
		wprintf(L"! Failed to transfer object to device, hr = 0x%lx\n", hr);
		return NULL;
	}
	hr = pDstDataStream->Commit(STGC_DEFAULT);
	if (FAILED(hr)) {
		wprintf(L"! Failed to commit object to device, hr = 0x%lx\n", hr);
		return NULL;
	}

	PWSTR createdObjectId = nullptr;
	hr = pDstDataStream->GetObjectID(&createdObjectId);
	if (FAILED(hr)) {
		wprintf(L"! Failed to get the newly transferred object's identifier from the device, hr = 0x%lx\n", hr);
		return NULL;
	}

	jstring res = NewStringFromLPWSTR(env, createdObjectId);

	CoTaskMemFree(createdObjectId);
	createdObjectId = nullptr;

	return res;
}