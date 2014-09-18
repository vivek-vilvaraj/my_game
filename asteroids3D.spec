
Name:		asteroids3D
Version:	0.5.1
Release:	0
Group:		Amusements/Games/Action/First Person Shootes
Summary:	First-person shooter blowing up asteroids
License:	GPL
URL:		http://jengelh.hopto.org/projects/a3d/

Source:		http://jengelh.hopto.org/files/a3d/asteroids3D-%version.tar.bz2
BuildRoot:	%_tmppath/%name-%version-build
BuildRequires:	Mesa-devel freeglut-devel
Prefix:		%_prefix/games

%description

%debug_package
%prep
%setup

%build
%configure --with-gamesdir=%_prefix/games --with-gamedatadir=%_datadir/games/%name
make %{?jobs:-j%jobs};

%install
b="%buildroot";
rm -Rf "$b";
mkdir "$b";
make install DESTDIR="$b";

%clean
rm -Rf "%buildroot";

%files
%defattr(-,root,root)
%prefix/%name
%_datadir/games/%name
