# -*- mode: ruby -*-
# vi: set ft=ruby :

VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  config.vm.box = "ubuntu/trusty64"

  # Provision
  config.vm.provision :shell, :path => "bootstrap-ubuntu.sh"

  # To access a website running on port 8000, we can open a web browser
  # on our workstation and go to http://localhost:8000
  config.vm.network "forwarded_port", guest: 8000, host: 8000

  # Shared folder
  config.vm.synced_folder "vagrant/", "/home/vagrant", create: true
end
