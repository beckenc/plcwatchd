# plcwatchd

* Observe plc state of a simatic plc using [snap7](http://snap7.sourceforge.net/).
* Send push notifications via pushover-api in case of plc left RUN state.
* Run as a linux-daemon process

# use docker
    docker build . -t beckenc/plcwatchd:latest
    docker run -d -it --rm --name plcwatchd beckenc/plcwatchd -i <ip-address of the plc> -k <pushover.net key> -t <pushover.net token> -c 600 -e 3600 -v

# use docker-compose
    echo "CMD=-i <ip-address of the plc> -k <pushover.net key> -t <pushover.net token> -c 600 -e 3600 -v" > .env
    docker-compose build
    docker-compose up -d
