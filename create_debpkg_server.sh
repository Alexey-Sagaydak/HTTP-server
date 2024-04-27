cd server/
mkdir -p ./deb_pakage
mkdir -p ./deb_pakage/bin
mkdir -p ./deb_pakage/DEBIAN
touch  ./deb_pakage/DEBIAN/control

cat << EOF > ./deb_pakage/DEBIAN/control
Package: http-server
Version: 1.0
Architecture: amd64
Maintainer: alexey-sagaidak
Description: http server for DCSA labs
EOF

cp ./build/libhv-http ./deb_pakage/bin/

dpkg-deb --build ./deb_pakage/ http-server.deb