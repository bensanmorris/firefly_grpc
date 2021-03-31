# check for go
path_to_go=$(which go)
if [ ! -x "$path_to_go" ] ; then
    echo "go executable not on user PATH - please install go to continue"
    exit 1
fi

# check mariadb (mysql) is running (required for testing)
pgrep -x mysqld >/dev/null
if [ $? -ne 1 ]; then
    echo "Mariadb (mysqld) doesn't appear to be running but is required in order to test. Exiting."
    exit 2
fi

# pull down submodules (grpc, certstrap etc)
rm -rf grpc
git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc

# build grpc
pushd .
cd grpc
git submodule update --init
make
make install
popd
go get -u github.com/grpc-ecosystem/grpc-gateway/protoc-gen-grpc-gateway
go get -u github.com/grpc-ecosystem/grpc-gateway/protoc-gen-swagger
go get -u github.com/golang/protobuf/protoc-gen-go

# build
buildType=Release
ca="ffsrv.fireflytech.org"
if [ -d build ]; then
    rm -rf build
fi
mkdir build
pushd .
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$buildType -DCA_NAME="$ca" -DSERVER_HOST_NAME=$HOSTNAME -DCLIENT_HOST_NAME=$HOSTNAME ..
cmake --build . --config $buildType
if [ $? -ne 0 ]; then
    echo "Build generation failed"
    exit 3
fi

# test
ctest -C $buildType
if [ $? -ne 0 ]; then
    echo "Testing failed"
    exit 4
fi
