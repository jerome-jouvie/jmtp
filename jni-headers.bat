@echo off
echo "Creating the header files (*.h)"
javah -d src/cpp -classpath src/java be.derycke.pieter.com.OleDate
javah -d src/cpp -classpath src/java be.derycke.pieter.com.COM
javah -d src/cpp -classpath src/java be.derycke.pieter.com.COMReference
javah -d src/cpp -classpath src/java jmtp.PortableDevice
javah -d src/cpp -classpath src/java jmtp.PortableDeviceContent
javah -d src/cpp -classpath src/java jmtp.PortableDeviceKeyCollection
javah -d src/cpp -classpath src/java jmtp.PortableDeviceManager
javah -d src/cpp -classpath src/java jmtp.PortableDeviceProperties
javah -d src/cpp -classpath src/java jmtp.PortableDevicePropVariantCollection
javah -d src/cpp -classpath src/java jmtp.PortableDeviceTransfert
javah -d src/cpp -classpath src/java jmtp.PortableDeviceValues
pause