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

package jmtp;

import java.io.Closeable;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import be.derycke.pieter.com.COM;
import be.derycke.pieter.com.COMException;
import be.derycke.pieter.com.COMReference;
import jmtp.exception.DeviceAlreadyOpenedException;
import jmtp.exception.DeviceClosedException;

/**
 *
 * @author Pieter De Rycke
 */
public class PortableDevice implements PortableDeviceContainer, Closeable {
    
    private String deviceID;
	private COMReference pDeviceManager;
	private COMReference pDevice;
    
    private PortableDeviceContent content;
    private PortableDeviceProperties properties;
    private PortableDeviceKeyCollection keyCollection;
    
    PortableDevice(COMReference pDeviceManager, String deviceID) {
        this.pDeviceManager = pDeviceManager;
        this.deviceID = deviceID;
        try {
            pDevice = COM.CoCreateInstance(WPD.CLSID_PortableDevice, 0, 
                    COM.CLSCTX_INPROC_SERVER, WPD.IID_IPortableDevice);
        } catch (COMException e) {
            e.printStackTrace();
        }
    }
    
    private native String getDeviceFriendlyName(String deviceID) throws COMException;
    private native String getDeviceManufacturer(String deviceID) throws COMException;
    private native String getDeviceDescription(String deviceID) throws COMException;
    private native void openImpl(PortableDeviceValues values) throws COMException;
    private native void closeImpl() throws COMException;
    native PortableDeviceContent getDeviceContent() throws COMException;
    public native PortableDeviceValues sendCommand(PortableDeviceValues values) throws COMException;
    
    private void createStructures() throws COMException {
    	if (content == null)
    		content = getDeviceContent();
    	
    	if (properties == null)
            properties = content.getProperties();
    	
    	if (keyCollection == null)
    		keyCollection = new PortableDeviceKeyCollection();
    	else		
    		keyCollection.clear();
    }
    
    private String retrieveStringValue(PropertyKey key) throws COMException {
    	try {
    		createStructures();
	    	
	        keyCollection.add(key);
	        return properties.getValues(Win32WPDDefines.WPD_DEVICE_OBJECT_ID, keyCollection).
	        	getStringValue(key);
    	} catch(COMException e) {
    		if (e.getHresult() == COMException.E_POINTER) {
    			//there is no connection to the device
    			throw new DeviceClosedException("The device connection is closed.");
    		} else {
	    		throw e;
    		}
    	}
    }
    
    private long retrieveUnsignedIntegerValue(PropertyKey key) throws COMException {
    	try {
    		createStructures();
	    	
	        keyCollection.add(key);
	        return properties.getValues(Win32WPDDefines.WPD_DEVICE_OBJECT_ID, keyCollection).
	        	getUnsignedIntegerValue(key);
    	} catch(COMException e) {
    		if (e.getHresult() == COMException.E_POINTER) {
    			//there is no connection to the device
    			throw new DeviceClosedException("The device connection is closed.");
    		} else {
	    		throw e;
    		}
    	}
    }
    
    private boolean retrieveBooleanValue(PropertyKey key) throws COMException {
    	try {
    		createStructures();
	    	
	        keyCollection.add(key);
	        return properties.getValues(Win32WPDDefines.WPD_DEVICE_OBJECT_ID, keyCollection).
	        	getBoolValue(key);
    	} catch(COMException e) {
    		if (e.getHresult() == COMException.E_POINTER)
    			throw new DeviceClosedException("The device connection is closed.");
    		else
	    		throw e;
    	}
    }
    
	public String getFriendlyName() {
        try {
            return getDeviceFriendlyName(deviceID);
        }
        catch(COMException e) {
            return null;
        }
    }
    
	public String getManufacturer()  {
        try {
            return getDeviceManufacturer(deviceID);
        } catch(COMException e) {
            return null;
        }
    }
    
	public String getDescription()  {
        try {
            return getDeviceDescription(deviceID);
        } catch(COMException e) {
            return null;
        }
    }

	public void open(String appName, int appMajor, int appMinor, int appRevision) {
        try {
            PortableDeviceValues values = new PortableDeviceValues();
            values.setStringValue(Win32WPDDefines.WPD_CLIENT_NAME, appName);
            values.setUnsignedIntegerValue(Win32WPDDefines.WPD_CLIENT_MAJOR_VERSION, appMajor);
            values.setUnsignedIntegerValue(Win32WPDDefines.WPD_CLIENT_MINOR_VERSION, appMinor);
            values.setUnsignedIntegerValue(Win32WPDDefines.WPD_CLIENT_REVISION, appRevision);
            
            openImpl(values);
        } catch(COMException e) {
            e.printStackTrace();
        }
    }
    
	public void open() {
        try {
            openImpl(new PortableDeviceValues());
        } catch(COMException e) {
        	if (e.getHresult() == Win32WPDDefines.E_WPD_DEVICE_ALREADY_OPENED) {
        		throw new DeviceAlreadyOpenedException("The device connection has already been opened.");
        	} else {
        		e.printStackTrace();
        	}
        }
    }
    
    @Override
	public void close() {
        try {
            closeImpl();
        } catch(COMException e) {
            e.printStackTrace();
        }
    }

    @Override
    public String toString() {
        return deviceID;
    }

    @Override
    public List<PortableDeviceObject> getChildObjects() {
        try {
            PortableDeviceContent content = getDeviceContent();
            PortableDeviceProperties properties = content.getProperties();

            String[] childIDs = content.listChildObjects(Win32WPDDefines.WPD_DEVICE_OBJECT_ID);
            List<PortableDeviceObject> objects = new ArrayList<>(childIDs.length);
            for(String childID : childIDs) {
            	PortableDeviceObject portableDeviceObject =
            			WPD.convertToPortableDeviceObject(childID, content, properties);
            	if (portableDeviceObject != null) {
            		objects.add(portableDeviceObject);
            	}
            }
            
            return objects;
        } catch (COMException e) {
        	if (e.getHresult() == COMException.E_POINTER) {
    			//there is no connection to the device
    			throw new DeviceClosedException("The device connection is closed.");
    		} else {
	    		e.printStackTrace();
	    		return Collections.emptyList();
    		}
        }
    }
    
    @Override
    public PortableDeviceFolderObject createFolderObject(String name) {
    	throw new UnsupportedOperationException();
    }
    
	public List<PortableDeviceObject> getPortableDeviceObjectsFromPersistentUniqueIDs(String[] persistentUniqueIDs) {
    	try {
	    	PortableDeviceContent content = getDeviceContent();
            PortableDeviceProperties properties = content.getProperties();
	    	
	    	PortableDevicePropVariantCollection persistentUniqueIDCollection =
	    			new PortableDevicePropVariantCollection();
	    	for (String persistentUniqueID : persistentUniqueIDs) {
	    		persistentUniqueIDCollection.add(new PropVariant(persistentUniqueID));
	    	}
	    	
	    	PortableDevicePropVariantCollection objectIDCollection = content
	    			.getObjectIDsFromPersistentUniqueIDs(persistentUniqueIDCollection);
	    	
	    	List<PortableDeviceObject> result = new ArrayList<>((int)objectIDCollection.count());
	    	for(int i = 0; i < objectIDCollection.count(); i++) {
	    		PortableDeviceObject portableDeviceObject = WPD.convertToPortableDeviceObject(
	    				(String)objectIDCollection.getAt(i).getValue(), content, properties);
	    		if (portableDeviceObject != null) {
	    			result.add(portableDeviceObject);
	    		}
	    	}
	    	
	    	return result;
    	} catch(COMException e) {
    		if (e.getHresult() == COMException.E_POINTER) {
    			//there is no connection to the device
    			throw new DeviceClosedException("The device connection is closed.");
    		} else {
	    		e.printStackTrace();
	    		return Collections.emptyList();
    		}
    	}
    }
    
	public PortableDeviceObject getPortableDeviceObjectsFromPersistentUniqueIDs(String persistentUniqueID) {
		List<PortableDeviceObject> list = getPortableDeviceObjectsFromPersistentUniqueIDs(new String[] {persistentUniqueID});
		return list.get(0);
    }
    
	public String getSerialNumber() {
    	try {
    		return retrieveStringValue(Win32WPDDefines.WPD_DEVICE_SERIAL_NUMBER);
    	}
    	catch(COMException e) {
	    	return null;
    	}
    }
    
	public String getFirmwareVersion() {
    	try {
    		return retrieveStringValue(Win32WPDDefines.WPD_DEVICE_FIRMWARE_VERSION);
    	}
    	catch(COMException e) {
	    	return null;
    	}
    }
    
	public String getModel() {
    	try {
    		return retrieveStringValue(Win32WPDDefines.WPD_DEVICE_MODEL);
    	} catch(COMException e) {
	    	return null;
    	}
    }
    
	public String getProtocol() {
    	try {
    		return retrieveStringValue(Win32WPDDefines.WPD_DEVICE_PROTOCOL);
    	} catch(COMException e) {
	    	return null;
    	}
    }
    
	public String getSyncPartner() {
    	try {
    		return retrieveStringValue(Win32WPDDefines.WPD_DEVICE_SYNC_PARTNER);
    	} catch(COMException e) {
	    	return null;
    	}
    }
    
	public int getPowerLevel() {
    	try {
    		//we can cast to an int because only values in the range [0-100]
    		return (int)retrieveUnsignedIntegerValue(Win32WPDDefines.WPD_DEVICE_POWER_LEVEL);
    	} catch(COMException e) {
	    	return -1;
    	}
    }
    
	public boolean isNonConsumableSupported() {
    	try {
    		return retrieveBooleanValue(Win32WPDDefines.WPD_DEVICE_SUPPORTS_NON_CONSUMABLE);
    	} catch(COMException e) {
    		return false;
    	}
    }
}
