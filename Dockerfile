FROM centos:latest

# dependencies
RUN yum update  -y
RUN yum install -y make
RUN yum install -y cmake3
RUN yum install -y go
RUN yum install -y gcc
RUN yum install -y gcc-c++
RUN yum install -y mariadb-devel
RUN yum install -y git
RUN yum install -y which
RUN yum install -y autoconf automake libtool
RUN yum install -y openssh-server openssh-clients
RUN cmake --version

# ssh keys
ARG ssh_prv_key
ARG ssh_pub_key

# authorize SSH Host
#RUN mkdir -p /root/.ssh && \
#    chmod 0700 /root/.ssh && \
#    ssh-keyscan github.com > /root/.ssh/known_hosts

# add the keys and set permissions
#RUN echo "$ssh_prv_key" > /root/.ssh/id_rsa && \
#    echo "$ssh_pub_key" > /root/.ssh/id_rsa.pub && \
#    chmod 600 /root/.ssh/id_rsa && \
#    chmod 600 /root/.ssh/id_rsa.pub

# paths
ARG install_dir=/opt/ffsrv.fireflytech.org/bin
ENV install_dir_env=$install_dir

# server - args
# nb. ARG values are only accessible at build (and not run) time hence use of ENV needed by ENTRYPOINT
ARG port=50051
ENV port_env=$port

ARG ca=.$install_dir/ffsrv.fireflytech.org.pem
ENV ca_env=$ca

ARG key=.$install_dir/server-key.pem
ENV key_env=$key

ARG cert=.$install_dir/server.pem
ENV cert_env=$cert

ARG db_hostname=localhost
ENV db_hostname_env=$db_hostname

ARG db_port=3306
ENV db_port_env=$db_port

ARG db_name=_test_db
ENV db_name_env=$db_name

ARG db_username=ha_user
ENV db_username_env=$db_username

ARG db_password=Test123!
ENV db_password_env=$db_password

# build grpc
RUN cd /tmp && git clone https://github.com/bensanmorris/firefly_grpc.git ffsrv && cd ffsrv && git clone -b v1.26.0 https://github.com/grpc/grpc
RUN rm -rf /root/.ssh/
RUN cd /tmp/ffsrv/grpc && git submodule update --init && sed -i 's/-Werror/ /g' Makefile && make && make install
#RUN cd /tmp/ffsrv && go get -u github.com/grpc-ecosystem/grpc-gateway/protoc-gen-grpc-gateway && go get -u github.com/grpc-ecosystem/grpc-gateway/protoc-gen-swagger && go get -u github.com/golang/protobuf/protoc-gen-go

# build deps
ARG BUILD_DEPS
RUN cd /tmp/ffsrv && git pull && buildType=Release && ca="ffsrv.fireflytech.org" && mkdir build && cd build && cmake3 -G "Unix Makefiles" -DBUILD_DEPS_ONLY=TRUE -DCMAKE_BUILD_TYPE=$buildType -DCA_NAME="$ca" -DSERVER_HOST_NAME=$db_hostname_env -DCLIENT_HOST_NAME=$db_hostname_env .. && cmake3 --build . --config $buildType

# configure dev tools
RUN yum install -y vim
RUN touch ~/.vimrc
RUN echo set tabstop=4 >> ~/.vimrc
RUN echo set shiftwidth=4 >> ~/.vimrc
RUN echo set expandtab >> ~/.vimrc

# build proj
ARG BUILD_PROJ
RUN cd /tmp/ffsrv && rm -f keys/create_certs.sh && git pull && buildType=Release && ca="ffsrv.fireflytech.org" && cd build && cmake3 -G "Unix Makefiles" -DBUILD_DEPS_ONLY=FALSE -DCMAKE_BUILD_TYPE=$buildType -DCA_NAME="$ca" -DSERVER_HOST_NAME=$db_hostname_env -DCLIENT_HOST_NAME=$db_hostname_env .. && make

# server
RUN mkdir -p $install_dir_env
RUN cp -a /tmp/ffsrv/build/bin/*        $install_dir_env
RUN cp -a /tmp/ffsrv/thirdparty/lib64/* $install_dir_env
RUN cp -a /tmp/ffsrv/thirdparty/lib*    $install_dir_env
EXPOSE 50051
ENTRYPOINT exec $install_dir_env/server --port=$port_env --ca=$ca_env --key=$key_env --cert=$cert_env --db_hostname=$db_hostname_env --db_port=$db_port_env --db_name=$db_name_env --db_username=$db_username_env --db_password=$db_password_env
