Summary: Netopeer CLI client for NETCONF protocol.
Name: netopeer-cli
Version: 0.8.0
Release: 1
URL: http://www.liberouter.org/
Source: https://www.liberouter.org/repo/SOURCES/%{name}-%{version}-%{release}.tar.gz
Group: Liberouter
License: BSD
Vendor: CESNET, z.s.p.o.
Packager: Rohan-Kadam <rkadam202@gmail.com>
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}

BuildRequires: gcc make pkgconfig  libnetconf-devel readline-devel
Requires: libxml2  libnetconf readline 
Provides: netopeer-cli

%description
CLI client connecting operator to a NETCONF capable device. This application
is built on the libnetconf library.

%prep
%setup

%build
./configure --prefix=%{_prefix} ;
make

%install
make DESTDIR=$RPM_BUILD_ROOT install

%post

%files
%{_bindir}/netopeer-cli
%{_mandir}/man1/*
%{_datadir}/netopeer/*.html
