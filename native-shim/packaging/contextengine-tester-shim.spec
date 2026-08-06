Name:       contextengine-tester-shim
Summary:    contextengine-tester native shim
Version:    0.1.0
Release:    1
Group:      Application/Service
# TODO: 실제 사내 라이선스 정책에 맞춰 교체
License:    TBD
Source0:    %{name}-%{version}.tar.gz
Source1001: %{name}.manifest

BuildRequires: cmake
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(capi-appfw-app-control)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: nlohmann-json-devel
# TODO: contextengine TIDL 생성 패키지의 -devel 패키지를 BuildRequires에 추가
# 예) BuildRequires: pkgconfig(contextengine-tidl)

%description
contextengine-tester의 디바이스 측 native shim. TIDL 구독, app_control 호출, SQLite DB
직접 조회를 로컬 소켓으로 bridge-server(Node.js)에 중계한다. docs/interface.md 참고.

%prep
%setup -q
cp %{SOURCE1001} .

%build
%cmake . -DCMAKE_BUILD_TYPE=%{?build_type:%build_type}%{!?build_type:RELEASE}
make %{?_smp_mflags}

%install
rm -rf %{buildroot}
%make_install

mkdir -p %{buildroot}%{_datadir}/packages
install -m 0644 tizen-manifest.xml \
    %{buildroot}%{_datadir}/packages/org.tizen.contextengine-tester-shim.xml

mkdir -p %{buildroot}%{_sysconfdir}/contextengine-tester-shim
install -m 0644 db_config.json \
    %{buildroot}%{_sysconfdir}/contextengine-tester-shim/db_config.json

%files
%manifest %{name}.manifest
%{_bindir}/contextengine_tester_shim
%{_datadir}/packages/org.tizen.contextengine-tester-shim.xml
%config %{_sysconfdir}/contextengine-tester-shim/db_config.json
