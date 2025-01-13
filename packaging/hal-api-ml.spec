Name:       hal-api-ml
Summary:    hal-api-ml interface
Version:    0.0.1
Release:    0
Group:      Development/Libraries
License:    Apache-2.0
Source0:    hal-api-ml-%{version}.tar.gz
Source1:    hal-api-ml.manifest
Source2:    hal-api-ml-manifest.xml
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(hal-api-common)

%description
hal-api-ml interface


### devel package #########
%package    devel
Summary:    hal-api-ml interface
Group:      Development/Libraries
Requires:   hal-api-ml = %{version}-%{release}

%description devel
hal-api-ml interface development package


### build and install #########
%prep
%setup -q

%cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix} -DCMAKE_LIBDIR_PREFIX=%{_libdir}

%build
cp %{SOURCE1} .
make %{?jobs:-j%jobs}

%check
# (cd tests/unittest && LD_LIBRARY_PATH=../../ ctest -V)

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}%{_sysconfdir}/hal
cp %{SOURCE2} %{buildroot}%{_sysconfdir}/hal/
%make_install

%clean
rm -rf %{buildroot}

%post
/sbin/ldconfig
/usr/bin/hal-compatibility-checker --reset

%postun
/sbin/ldconfig
/usr/bin/hal-compatibility-checker --reset

%files
%manifest hal-api-ml.manifest
%license LICENSE
%defattr(-,root,root,-)
%{_libdir}/hal/*.so*
%{_sysconfdir}/hal/hal-api-ml-manifest.xml

%files devel
%defattr(-,root,root,-)
%{_includedir}/hal/*.*
%{_libdir}/pkgconfig/*.pc
