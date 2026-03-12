%.msix: %/AppxManifest.xml
	makeappx.exe pack /d $(<D) /p $@ /nv /o

default: \
	basic.msix
