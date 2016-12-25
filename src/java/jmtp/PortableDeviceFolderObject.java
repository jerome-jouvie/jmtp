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

import be.derycke.pieter.com.COMException;

/**
 *
 * @author Pieter De Rycke
 */
public class PortableDeviceFolderObject extends AbstractPortableDeviceContainer {

    public PortableDeviceFolderObject(String objectID, PortableDeviceContent content,
            PortableDeviceProperties properties) {
        super(objectID, content, properties);
    }
    
	public void delete(boolean recursive) {
		try {
			PortableDevicePropVariantCollection collection = new PortableDevicePropVariantCollection();
			collection.add(new PropVariant(this.objectID));
			this.content.delete(
					recursive ? Win32WPDDefines.PORTABLE_DEVICE_DELETE_WITH_RECURSION : Win32WPDDefines.PORTABLE_DEVICE_DELETE_NO_RECURSION,
					collection);
		} catch(COMException e) {
			e.printStackTrace();
		}
    }
}
