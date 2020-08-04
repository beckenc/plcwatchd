# plcwatchd

* Observe plc state of a simatic plc using [snap7](http://snap7.sourceforge.net/).
* Send push notifications via pushover-api in case of plc left RUN state.
* Run as a linux-daemon process

# precondition
* sudo add-apt-repository ppa:gijzelaar/snap7
* sudo apt-get update
* sudo apt-get install libsnap7-dev libsnap7-1
* sudo apt-get install libcurl4-gnutls-dev
* sudo apt-get install rapidjson-dev

# install
* cd Release
* make clean all
* sudo cp plcwatchd /usr/local/bin/
* cp ../plcwatchd.service.draft ../plcwatchd.service
* vi ../plcwatchd.service
* sudo cp ../plcwatchd.service /etc/systemd/system
* sudo systemctl enable plcwatchd

# start
* sudo systemctl start plcwatchd

# stop
* sudo systemctl stop plcwatchd

# status
* sudo systemctl status plcwatchd
or
* ps aux | grep plcwatchd


