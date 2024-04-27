cd client/
mkdir -p ./deb_pakage
mkdir -p ./deb_pakage/bin
mkdir -p ./deb_pakage/DEBIAN
touch  ./deb_pakage/DEBIAN/control

cat << EOF > ./deb_pakage/DEBIAN/control
Package: http-client
Version: 1.0
Architecture: amd64
Maintainer: alexey-sagaidak
Description: http client for DCSA labs
EOF

cp ./build/libhv-client ./deb_pakage/bin/

dpkg-deb --build ./deb_pakage/ http-client.deb