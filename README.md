jmtp
======

Java API to access MTP compatible portable media players

A project aimed at allowing java programs to access MTP compabitble portable media players.
The main target is currently windows, by wrapping the WPD api, but design decisions have been made to support both linux and mac os x in the future with the same API.
For more information, please see the wiki.

My blog: http://pieterderycke.wordpress.com/

#### Fork used in http://reader.bonzaiengine.com
* Added 64 bit support
* Added transfert functionalities (PortableDeviceTransfert replaces PortableDeviceToHostImpl32)
* Cache JNI class and methods references in ClassAndMethods
* Internal refactor
