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

import java.io.File;
import java.io.IOException;
import be.derycke.pieter.com.COMException;

public class PortableDeviceTransfert {
	
    public static boolean transfertToHost(String objectId, File dstPath, PortableDevice pDevice) throws COMException {
    	return transfertToHost(objectId,
    			dstPath.getAbsolutePath(),
    			pDevice);
    }

    public static boolean transfertFromHost(String objectId, File srcPath, PortableDevice pDevice) throws COMException, IOException {
    	return transfertFromHost(objectId,
    			srcPath.getAbsolutePath(),
    			srcPath.getName(),
    			srcPath.length(),
    			pDevice) != null;
    }
    
    public static native boolean transfertToHost(String objectId, String dstPath, PortableDevice pDevice) throws COMException;
    
    private static native String transfertFromHost(String objectId, String srcPath, String srcName, long srcSize, PortableDevice pDevice) throws COMException;
}
