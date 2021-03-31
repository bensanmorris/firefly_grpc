# About

A small (grpc-based) server for transmitting / recording (in a mariadb database) the 3D location of a game tester in a game to aid in analysing test coverage of 3D game environments.

# Licensing

The software is free for non-commercial use. For a commercial license please get in touch.

# Build

Build server image:
    
    sudo docker build -t ffsrv:latest .

Generate the server keys (that will be used to secure the server over TLS):

    pushd .
    cd keys
    ./create_certs.sh MyCA
    popd

Build db image:

    cd db
    sudo docker build -t ffsrv_db:latest .

Run db container:

    sudo docker rm ffsrv_db && sudo docker run --network=host --name ffsrv_db ffsrv_db:latest

Verify the db (via mysql client):

    mysql -h 127.0.0.1 -u ff_user -p

    > Test123!

Run server container (with syslog logging, by default write to localhost:514):

    sudo docker rm ffsrv && sudo docker run --log-driver=syslog --network=host -v `pwd`/keys:/keys --env-file=./devel_env-file --name ffsrv ffsrv:latest

Run the test client:

    sudo docker run -v `pwd`/keys:/keys --network=host --entrypoint /opt/ffsrv.fireflytech.org/bin/test_client ffsrv:latest

Shell into the container:

    sudo docker run -it --entrypoint bash ffsrv:latest

Shell into a container layer:

    sudo docker run --rm -it [layer_id]

# AWS deployment

    https://docs.aws.amazon.com/AmazonECR/latest/userguide/ECR_AWSCLI.html
    
    repos:
	    ben@ben-LabTop:~/dev/firefly_grpc$ aws ecr create-repository --repository-name ffsrv
        {
            "repository": {
                "repositoryArn": "arn:aws:ecr:us-west-2:062866975314:repository/ffsrv",
                "registryId": "062866975314",
                "repositoryName": "ffsrv",
                "repositoryUri": "062866975314.dkr.ecr.us-west-2.amazonaws.com/ffsrv",
                "createdAt": 1578763760.0
            }
        }

        {
            "repository": {
                "repositoryUri": "062866975314.dkr.ecr.us-west-2.amazonaws.com/ffsrv_db", 
                "imageScanningConfiguration": {
                    "scanOnPush": false
                }, 
                "registryId": "062866975314", 
                "imageTagMutability": "MUTABLE", 
                "repositoryArn": "arn:aws:ecr:us-west-2:062866975314:repository/ffsrv_db", 
                "repositoryName": "ffsrv_db", 
                "createdAt": 1578903150.0
            }
        }


    steps:
        create an aws user with appropriate IAM privs
        aws configure using aws user / access key
        create docker repo
        tag docker image:
            sudo docker tag a3232ae459c5 062866975314.dkr.ecr.us-west-2.amazonaws.com/ffsrv:latest
        push docker image:
            aws ecr get-login --registry-ids 062866975314 --no-include-email
            sudo docker push 062866975314.dkr.ecr.us-west-2.amazonaws.com/ffsrv:latest
 
