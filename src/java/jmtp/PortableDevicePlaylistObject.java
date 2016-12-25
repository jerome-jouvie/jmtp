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

public final class PortableDevicePlaylistObject extends PortableDeviceObject {

	PortableDevicePlaylistObject(String objectID, PortableDeviceContent content, PortableDeviceProperties properties) {
		super(objectID, content, properties);
	}

    public List<PortableDeviceObject> getReferences() {
		try {
			keyCollection.clear();
			keyCollection.add(Win32WPDDefines.WPD_OBJECT_REFERENCES);
			PortableDevicePropVariantCollection propVariantCollection = 
					properties.getValues(objectID, keyCollection).
					getPortableDeviceValuesCollectionValue(Win32WPDDefines.WPD_OBJECT_REFERENCES);

			//long naar int? misschien een probleem bij *enorm* grote afspeellijsten, 
			//maar kan normaal wel niet gebeuren
            List<PortableDeviceObject> references = new ArrayList<>((int)propVariantCollection.count());
			for(int i = 0; i < propVariantCollection.count(); i++) {
				PortableDeviceObject portableDeviceObject = WPD.convertToPortableDeviceObject(
						(String)propVariantCollection.getAt(i).getValue(), content, properties);
				if (portableDeviceObject != null) {
					references.add(portableDeviceObject);
				}
			}
			return references;
		} catch(COMException e) {
            return Collections.emptyList();
		}
	}
}