#!/usr/bin/env bash

sudo apt-get update -y

# install deps
sudo apt-get install -y git make

# install golang
wget https://storage.googleapis.com/golang/go1.8.3.linux-amd64.tar.gz
tar -C /usr/local -xzf go1.8.3linux-amd64.tar.gz

echo "export PATH=$PATH:/usr/local/go/bin" >> .bashrc
echo "export GOPATH=$PWD/golang/" >> .bashrc
echo "source ~/.bashrc" >> ~/.bash_profile
