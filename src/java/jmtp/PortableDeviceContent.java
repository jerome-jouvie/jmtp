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
import be.derycke.pieter.com.COMReference;
import java.io.File;

/**
 *
 * @author Pieter De Rycke
 */
class PortableDeviceContent {
    
	private COMReference pDeviceContent;
    
    PortableDeviceContent(COMReference pDeviceContent) {
        this.pDeviceContent = pDeviceContent;
    }
    
    /**
     * Door een probleem met unsigned longs (ontbreken in java, regelt de c++ dll de file groote, moet dus niet
     * ingesteld worden vanuit Java!)
     * 
     * @param values
     * @param file
     * @return objectId van het nieuw aangemaakte object
     * @throws COMException
     */
    native String createObjectWithPropertiesAndData(PortableDeviceValues values,
            File file) throws COMException;
    
    native String createObjectWithPropertiesOnly(PortableDeviceValues values) 
            throws COMException;
    
    native void delete(int options, PortableDevicePropVariantCollection objectIDs);
    
    native String[] listChildObjects(String parentID) throws COMException;
    
    native PortableDeviceProperties getProperties() throws COMException;
    
    native PortableDevicePropVariantCollection getObjectIDsFromPersistentUniqueIDs(
    		PortableDevicePropVariantCollection persistentUniqueIDs) throws COMException;
    
    /**
     * The Cancel method cancels a pending operation called on this interface
     * @throws be.derycke.pieter.com.COMException
     */
    native void cancel() throws COMException;
}
