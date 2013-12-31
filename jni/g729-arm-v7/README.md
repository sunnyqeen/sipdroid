
This code is a port of ITU g729a codec implementation for armv6 / armv7. It has been tested on iOS. People reported this code works on Android.

Compilation
-----------

- Download the source from Github
- Modify some variables:
   - INSTALLDIR,
   - DEST,
   - maybe DEVPATH, SDKPATH, IPHONEOS_DEPLOYMENT_TARGET,
- Run 'build_ios.sh', the library is compiled for armv6/armv7 into ${INSTALLDIR}/lib

Assessment / Check
------------------

- Download ITU g729a codec implementation.
- Copy into 'tests/data', the files *.pst, *.bit and *.in
- Run 'verify.sh' on your device
- Shell will stop if the generated file is different from the reference

License
-------

This code is licensed under GNU General Public License. See LICENSE for more details.

Patent information
------------------

Despite this implementation is open-source, the use of G729 remains governed by a patent license to be acquired from G729 patent holder representatives.
