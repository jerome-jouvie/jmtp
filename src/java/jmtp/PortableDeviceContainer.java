package jmtp;

import java.util.List;

public interface PortableDeviceContainer {
	List<PortableDeviceObject> getChildObjects();
	PortableDeviceFolderObject createFolderObject(String name);
}
