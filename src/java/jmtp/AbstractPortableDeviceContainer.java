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

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import be.derycke.pieter.com.COMException;

abstract class AbstractPortableDeviceContainer extends PortableDeviceObject implements PortableDeviceContainer {
	
	AbstractPortableDeviceContainer(String objectID, PortableDeviceContent content, PortableDeviceProperties properties) {
		super(objectID, content, properties);
	}

	@Override
	public final List<PortableDeviceObject> getChildObjects() {
        try {
            String[] childIDs = content.listChildObjects(objectID);
            List<PortableDeviceObject> objects = new ArrayList<>(childIDs.length);
            for(String childID : childIDs) {
            	PortableDeviceObject portableDeviceObject =
            			WPD.convertToPortableDeviceObject(childID, this.content, this.properties);
				if (portableDeviceObject != null) {
					objects.add(portableDeviceObject);
				}
            }
            return objects;
        } catch (COMException e) {
            return Collections.emptyList();
        }
    }

	@Override
	public PortableDeviceFolderObject createFolderObject(String name) {
		try {
			PortableDeviceValues values = new PortableDeviceValues();
			values.setStringValue(Win32WPDDefines.WPD_OBJECT_PARENT_ID, this.objectID);
			values.setStringValue(Win32WPDDefines.WPD_OBJECT_ORIGINAL_FILE_NAME, name);
			values.setStringValue(Win32WPDDefines.WPD_OBJECT_NAME, name);
			values.setGuidValue(Win32WPDDefines.WPD_OBJECT_CONTENT_TYPE, Win32WPDDefines.WPD_CONTENT_TYPE_FOLDER);
			
			return new PortableDeviceFolderObject(content.createObjectWithPropertiesOnly(values),
	        		this.content, this.properties);
        } catch (COMException e) {
        	e.printStackTrace();
            return null;
        }
	}
}
