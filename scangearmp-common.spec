%define VERSION 2.30
%define RELEASE 1

%define _arc  %(getconf LONG_BIT)
%define _is64 %(if [ `getconf LONG_BIT` = "64" ] ; then  printf "64";  fi)

%define _prefix /usr/local
%define _bindir %{_prefix}/bin
%define _libdir /usr/lib%{_is64}

%define PKG1	mx470series
%define PKG2	mx530series
%define PKG3	e560series
%define PKG4	e400series

%define PR1_ID	434
%define PR2_ID	435
%define PR3_ID	437
%define PR4_ID	438
%define BUILD_PR_ID %{PR1_ID} %{PR2_ID} %{PR3_ID} %{PR4_ID}

%define PR1_USBID	1774
%define PR2_USBID	1775
%define PR3_USBID	177b
%define PR4_USBID	177a

%define CNCP_LIBS libcncpmsimg libcncpmslld
%define CNCP_LIBS_COM libcncpmsimg libcncpmslld libcncpcmcm libcncpmsui libcncpnet

%define _gimp2dir1	%{_libdir}/gimp/2.0/plug-ins
%define _gimp2dir2	/opt/gnome/lib/gimp/2.0/plug-ins


Summary: ScanGear MP Ver.%{VERSION} for Linux
Name: scangearmp-common
Version: %{VERSION}
Release: %{RELEASE}
License: See the LICENSE*.txt file.
Vendor: CANON INC.
Group: Applications/Graphics
Source0: scangearmp-source-%{version}-%{release}.tar.gz
BuildRoot: %{_tmppath}/%{name}-root
Requires: gtk2
BuildRequires: gtk2-devel gimp-devel

%package -n scangearmp-%{PKG1}
Summary: ScanGear MP Ver.%{VERSION} for Linux
License: See the LICENSE*.txt file.
Vendor: CANON INC.
Group: Applications/Graphics
Requires: %{name} >= %{version}

%package -n scangearmp-%{PKG2}
Summary: ScanGear MP Ver.%{VERSION} for Linux
License: See the LICENSE*.txt file.
Vendor: CANON INC.
Group: Applications/Graphics
Requires: %{name} >= %{version}

%package -n scangearmp-%{PKG3}
Summary: ScanGear MP Ver.%{VERSION} for Linux
License: See the LICENSE*.txt file.
Vendor: CANON INC.
Group: Applications/Graphics
Requires: %{name} >= %{version}

%package -n scangearmp-%{PKG4}
Summary: ScanGear MP Ver.%{VERSION} for Linux
License: See the LICENSE*.txt file.
Vendor: CANON INC.
Group: Applications/Graphics
Requires: %{name} >= %{version}


%description
ScanGear MP for Linux.
This ScanGear MP provides scanning functions for Canon Multifunction Inkjet Printer.

%description -n scangearmp-%{PKG1}
ScanGear MP for Linux.
This ScanGear MP provides scanning functions for Canon Multifunction Inkjet Printer.

%description -n scangearmp-%{PKG2}
ScanGear MP for Linux.
This ScanGear MP provides scanning functions for Canon Multifunction Inkjet Printer.

%description -n scangearmp-%{PKG3}
ScanGear MP for Linux.
This ScanGear MP provides scanning functions for Canon Multifunction Inkjet Printer.

%description -n scangearmp-%{PKG4}
ScanGear MP for Linux.
This ScanGear MP provides scanning functions for Canon Multifunction Inkjet Printer.


%prep
%setup -q -n scangearmp-source-%{version}-%{release}


%build
#make


%install
# make install directory
mkdir -p ${RPM_BUILD_ROOT}%{_bindir}
mkdir -p ${RPM_BUILD_ROOT}%{_libdir}/bjlib
mkdir -p ${RPM_BUILD_ROOT}/etc/udev/rules.d/
mkdir -p ${RPM_BUILD_ROOT}%{_datadir}/doc/scangearmp-common-%{version}

# copy common libraries
install -c -s -m 755 com/libs_bin%{_arc}/*.so.* ${RPM_BUILD_ROOT}%{_libdir}
install -c -m 666 com/ini/canon_mfp_net.ini ${RPM_BUILD_ROOT}%{_libdir}/bjlib

# copy *.tbl, *.DAT, model-specific libraries
for PR_ID in %{BUILD_PR_ID}
do
	install -c -m 755 ${PR_ID}/*.tbl ${PR_ID}/*.DAT ${RPM_BUILD_ROOT}%{_libdir}/bjlib
	install -c -s -m 755 ${PR_ID}/libs_bin%{_arc}/*.so.* ${RPM_BUILD_ROOT}%{_libdir}
done

# copy rules file
pushd scangearmp
	install -c -m 644 etc/*.rules ${RPM_BUILD_ROOT}/etc/udev/rules.d/
popd

# make install
pushd scangearmp
	./autogen.sh --prefix=%{_prefix} --enable-libpath=%{_libdir} LDFLAGS="-L`pwd`/../com/libs_bin%{_arc}"
	make clean
	make
    make install DESTDIR=${RPM_BUILD_ROOT} 
	# remove .la .a
	rm -f ${RPM_BUILD_ROOT}%{_libdir}/*.la ${RPM_BUILD_ROOT}%{_libdir}/*.a
popd

%clean
rm -rf $RPM_BUILD_ROOT


%post
# make symbolic link for gimp-plug-in
if [ -d %{_gimp2dir1} ]; then
	ln -fs %{_bindir}/scangearmp %{_gimp2dir1}/scangearmp
elif [ -d %{_gimp2dir2} ]; then
	ln -fs %{_bindir}/scangearmp %{_gimp2dir2}/scangearmp
fi
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
#reload udev rules
if [ -x /sbin/udevadm ]; then
	/sbin/udevadm control --reload-rules 2> /dev/null
	/sbin/udevadm trigger --action=add --subsystem-match=usb 2> /dev/null
fi

%postun
# remove symbolic link (common libs)
for LIBS in %{CNCP_LIBS_COM}
do
	if [ -h %{_libdir}/${LIBS}.so ]; then
		rm -f %{_libdir}/${LIBS}.so
	fi	
done

# remove sgmp_setting files
rm -f /var/tmp/canon_sgmp_setting*.*
rm -f /tmp/canon_sgmp_setting*.*

# remove directory
if [ "$1" = 0 ] ; then
	# remove symbolic link for gimp-plug-in
	rm -f %{_gimp2dir1}/scangearmp
	rm -f %{_gimp2dir2}/scangearmp

	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/locale/*/LC_MESSAGES
	rmdir -p --ignore-fail-on-non-empty %{_prefix}/share/scangearmp
	rmdir -p --ignore-fail-on-non-empty %{_libdir}/bjlib
	rmdir -p --ignore-fail-on-non-empty %{_bindir}
fi

if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi


%post -n scangearmp-%{PKG1}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
%postun -n scangearmp-%{PKG1}
# remove symbolic link (model-specific libs)
for LIBS in %{CNCP_LIBS}
do
	if [ -h %{_libdir}/${LIBS}%{PR1_ID}.so ]; then
		rm -f %{_libdir}/${LIBS}%{PR1_ID}.so
	fi
	if [ -h %{_libdir}/${LIBS}%{PR1_ID}c.so ]; then
		rm -f %{_libdir}/${LIBS}%{PR1_ID}c.so
	fi
done
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi

%post -n scangearmp-%{PKG2}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
%postun -n scangearmp-%{PKG2}
# remove symbolic link (model-specific libs)
for LIBS in %{CNCP_LIBS}
do
	if [ -h %{_libdir}/${LIBS}%{PR2_ID}.so ]; then
		rm -f %{_libdir}/${LIBS}%{PR2_ID}.so
	fi
	if [ -h %{_libdir}/${LIBS}%{PR2_ID}c.so ]; then
		rm -f %{_libdir}/${LIBS}%{PR2_ID}c.so
	fi
done
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi

%post -n scangearmp-%{PKG3}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
%postun -n scangearmp-%{PKG3}
# remove symbolic link (model-specific libs)
for LIBS in %{CNCP_LIBS}
do
	if [ -h %{_libdir}/${LIBS}%{PR3_ID}.so ]; then
		rm -f %{_libdir}/${LIBS}%{PR3_ID}.so
	fi
	if [ -h %{_libdir}/${LIBS}%{PR3_ID}c.so ]; then
		rm -f %{_libdir}/${LIBS}%{PR3_ID}c.so
	fi
done
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi

%post -n scangearmp-%{PKG4}
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi
%postun -n scangearmp-%{PKG4}
# remove symbolic link (model-specific libs)
for LIBS in %{CNCP_LIBS}
do
	if [ -h %{_libdir}/${LIBS}%{PR4_ID}.so ]; then
		rm -f %{_libdir}/${LIBS}%{PR4_ID}.so
	fi
	if [ -h %{_libdir}/${LIBS}%{PR4_ID}c.so ]; then
		rm -f %{_libdir}/${LIBS}%{PR4_ID}c.so
	fi
done
if [ -x /sbin/ldconfig ]; then
	/sbin/ldconfig
fi



%files
%defattr(-,root,root)
%{_libdir}/libcncpcmcm.so*
%{_libdir}/libcncpmsimg.so*
%{_libdir}/libcncpmslld.so*
%{_libdir}/libcncpmsui.so*
%{_libdir}/libcncpnet.so*

%{_libdir}/libsane-canon_mfp.so*
%{_bindir}/scangearmp
%{_libdir}/bjlib/canon_mfp.conf
%{_prefix}/share/locale/*/LC_MESSAGES/scangearmp.mo
%{_prefix}/share/scangearmp/*

/etc/udev/rules.d/*.rules

%doc LICENSE-scangearmp-%{VERSION}EN.txt
%doc LICENSE-scangearmp-%{VERSION}JP.txt
%doc LICENSE-scangearmp-%{VERSION}FR.txt
%doc LICENSE-scangearmp-%{VERSION}SC.txt

%attr(666,root,root) %{_libdir}/bjlib/canon_mfp_net.ini


%files -n scangearmp-%{PKG1}
%defattr(-,root,root)
%{_libdir}/libcncpmsimg%{PR1_ID}.so*
%{_libdir}/libcncpmslld%{PR1_ID}.so*
%{_libdir}/libcncpmslld%{PR1_ID}c.so*
%{_libdir}/bjlib/cnc_%{PR1_ID}0.tbl
%{_libdir}/bjlib/CNC_%{PR1_ID}H.DAT
%{_libdir}/bjlib/CNC_%{PR1_ID}P.DAT
%{_libdir}/bjlib/CNC_%{PR1_ID}U.DAT
%{_libdir}/bjlib/cnc%{PR1_USBID}d.tbl
%doc LICENSE-scangearmp-%{VERSION}EN.txt
%doc LICENSE-scangearmp-%{VERSION}JP.txt
%doc LICENSE-scangearmp-%{VERSION}FR.txt
%doc LICENSE-scangearmp-%{VERSION}SC.txt


%files -n scangearmp-%{PKG2}
%defattr(-,root,root)
%{_libdir}/libcncpmsimg%{PR2_ID}.so*
%{_libdir}/libcncpmslld%{PR2_ID}.so*
%{_libdir}/libcncpmslld%{PR2_ID}c.so*
%{_libdir}/bjlib/cnc_%{PR2_ID}0.tbl
%{_libdir}/bjlib/CNC_%{PR2_ID}H.DAT
%{_libdir}/bjlib/CNC_%{PR2_ID}P.DAT
%{_libdir}/bjlib/CNC_%{PR2_ID}U.DAT
%{_libdir}/bjlib/cnc%{PR2_USBID}d.tbl
%doc LICENSE-scangearmp-%{VERSION}EN.txt
%doc LICENSE-scangearmp-%{VERSION}JP.txt
%doc LICENSE-scangearmp-%{VERSION}FR.txt
%doc LICENSE-scangearmp-%{VERSION}SC.txt


%files -n scangearmp-%{PKG3}
%defattr(-,root,root)
%{_libdir}/libcncpmsimg%{PR3_ID}.so*
%{_libdir}/libcncpmslld%{PR3_ID}.so*
%{_libdir}/libcncpmslld%{PR3_ID}c.so*
%{_libdir}/bjlib/cnc_%{PR3_ID}0.tbl
%{_libdir}/bjlib/CNC_%{PR3_ID}H.DAT
%{_libdir}/bjlib/CNC_%{PR3_ID}P.DAT
%{_libdir}/bjlib/CNC_%{PR3_ID}U.DAT
%{_libdir}/bjlib/cnc%{PR3_USBID}d.tbl
%doc LICENSE-scangearmp-%{VERSION}EN.txt
%doc LICENSE-scangearmp-%{VERSION}JP.txt
%doc LICENSE-scangearmp-%{VERSION}FR.txt
%doc LICENSE-scangearmp-%{VERSION}SC.txt


%files -n scangearmp-%{PKG4}
%defattr(-,root,root)
%{_libdir}/libcncpmsimg%{PR4_ID}.so*
%{_libdir}/libcncpmslld%{PR4_ID}.so*
%{_libdir}/libcncpmslld%{PR4_ID}c.so*
%{_libdir}/bjlib/cnc_%{PR4_ID}0.tbl
%{_libdir}/bjlib/CNC_%{PR4_ID}H.DAT
%{_libdir}/bjlib/CNC_%{PR4_ID}P.DAT
%{_libdir}/bjlib/CNC_%{PR4_ID}U.DAT
%{_libdir}/bjlib/cnc%{PR4_USBID}d.tbl
%doc LICENSE-scangearmp-%{VERSION}EN.txt
%doc LICENSE-scangearmp-%{VERSION}JP.txt
%doc LICENSE-scangearmp-%{VERSION}FR.txt
%doc LICENSE-scangearmp-%{VERSION}SC.txt



%ChangeLog

