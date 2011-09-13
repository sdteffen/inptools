Name: inptools
Version: 0.2.0
Release: 1
Vendor: epanet.de
License: GPL
Summary: Tools to process files in the EPANET INP and BIN formats
Group: Productivity/Other
Packager: Steffen Macke <sdteffen@sdteffen.de>
BuildRoot:  %{_tmppath}/%{name}-%{version}-build 
Source: %{name}-%{version}.tar.gz
Url: http://inptools.epanet.de

Requires: epanet2 libproj shapelib
BuildRequires: epanet2 libproj-devel shapelib-devel

%description
Tools to process files in the EPANET INP format

%prep
%setup

%build
./configure --prefix=/usr --libdir=%_libdir
make

%install
rm -rf %{buildroot}
%makeinstall

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog NEWS README
/usr/bin/epanet2csv
/usr/bin/inpproj
/usr/bin/inp2shp
/usr/bin/shp2inp
%changelog
* Sat Feb 21 2009 - Steffen Macke <sdteffen@sdteffen.de>
- Added epanet2csv
* Wed Aug 27 2008 - Steffen Macke <sdteffen@sdteffen.de>
- Initial RPM build for version 0.0.2
