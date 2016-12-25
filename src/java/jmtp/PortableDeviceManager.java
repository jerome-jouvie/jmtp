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
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import be.derycke.pieter.com.COM;
import be.derycke.pieter.com.COMException;
import be.derycke.pieter.com.COMReference;

/**
 *
 * @author Pieter De Rycke
 */
public class PortableDeviceManager {
	
	public static boolean LOG_LOADING = true;
    
    static {
		final String osArch = System.getProperty("os.arch");
		boolean bits64 = (osArch.indexOf("64") != -1);
		loadLibrary(bits64 ? "jmtp64" : "jmtp");
    }
    
    private static void loadLibrary(String name) {
		try {
			log("Loading: " + name);
			System.loadLibrary(name);
		} catch (Throwable t) {
			log("Failed: " + t.getMessage());
			
			String fileName = name + ".dll";
			List<Path> files = new ArrayList<>();
			for (String property : new String[]{"java.library.path", "org.lwjgl.librarypath"}) {
				findLibrary(fileName, System.getProperty(property, ""), files);
			}
			
			for (Path file : files) {
				String path = file.normalize().toAbsolutePath().toFile().getPath();
				try {
					log("Loading: " + path);
					System.load(path);
					return;
				} catch (Throwable t2) {
					log("Failed: " + t.getMessage());
				}
			}
			
			throw t;
		}
    }

    private static void log(String s) {
    	if (LOG_LOADING) {
    		System.out.println(s);
    	}
    }

    private static void findLibrary(String fileName, String pathString, List<Path> files) {
    	if (pathString.isEmpty()) {
    		return;
    	}
    	for (String dir : pathString.split(File.pathSeparator)) {
    		Path file = Paths.get(dir).resolve(fileName);
    		if (Files.exists(file)) {
    			files.add(file);
    		}
    	}
    }
    
    private COMReference pDeviceManager;
    
    private final Map<String, PortableDevice> deviceMap = new HashMap<>();
    
    public PortableDeviceManager() {
        try {
            pDeviceManager = COM.CoCreateInstance(WPD.CLSID_PortableDeviceManager, 0,
                    COM.CLSCTX_INPROC_SERVER, WPD.IID_IPortableDeviceManager);
        } catch (Exception e) {
            throw new RuntimeException("probleem met de com");
        }
    }
    
	public List<PortableDevice> getDevices() {
        try {
            String[] devices = getDevicesImpl();

            Set<String> deviceSet = new HashSet<>();
            for (String deviceID : devices) {
                if (!deviceMap.containsKey(deviceID)) {
                	deviceMap.put(deviceID, new PortableDevice(pDeviceManager, deviceID));
                }
                deviceSet.add(deviceID);
            }
            
            for (String deviceID : deviceMap.keySet()) {
            	if (!deviceSet.contains(deviceID)) {
            		deviceMap.remove(deviceID);
            	}
            }
            
            return new ArrayList<>(deviceMap.values());
        } catch (COMException e) {
            e.printStackTrace();
            return Collections.emptyList();
        }
    }
	
    public native void refreshDeviceList() throws COMException;
    
    private native String[] getDevicesImpl() throws COMException;
}
