#!/usr/bin/env bash
trap "exit" INT

# GPIO C Library
sudo apt-get update
sudo apt-get -y install wiringpi

# Make executable
rm -rf ./build/
cmake -B./build/ -H.
cd build/
make
cd ..

# Move executable to /usr/bin
sudo rm -f /usr/bin/rpifan
sudo mv ./build/rpifan /usr/bin/

# Update cron jobs
crontab -l > mycron
echo "" >> mycron
echo "# RPI Fan controller" >> mycron
echo "*/10 * * * * /usr/bin/rpifan" >> mycron
crontab mycron

# Clean up
rm -f mycron
rm -rf ./build/
