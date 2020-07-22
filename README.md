# plcwatchd

* Observe plc state of a simatic plc using [snap7](http://snap7.sourceforge.net/).
* Send push notifications via pushover-api in case of plc left RUN state.
* Run as a linux-daemon process

# precondition
* sudo add-apt-repository ppa:gijzelaar/snap7
* sudo apt-get update
* sudo apt-get install libsnap7-1 libsnap7-1-dev
* sudo apt-get install libcurl4-gnutls-dev

# install
* git submodule update --init
* cd Release
* make clean all
* sudo cp plcwatchd /usr/local/bin/
* cp ../plcwatchd.service.draft ../plcwatchd.service
* sudo cp /plcwatchd.service /etc/systemd/system
* sudo systemctl enable plcwatchd

