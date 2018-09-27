Basic guide to deploying a simple 4-node Lustre cluster on Centos 7 using VMs from Virtualbox.  

Updated and adapted fork of [dleske/Three-Node-LustreFS-Cluster-Quickstart.md](https://gist.github.com/dleske/743f9dafc212b7bb0edce370e961b99e), which was based on [joshuar/Three-Node-LustreFS-Cluster-Quickstart.md](https://gist.github.com/joshuar/4e283308c932ec62fc05), which was based on Intel's [How to Create and Mount a Lustre Filesystem](https://wiki.hpdd.intel.com/display/PUB/Create+and+Mount+a+Lustre+Filesystem). I've taken a lot of inspiration from [Oak Ridge National Laboratory slides about Lustre](https://lustre.ornl.gov/lustre101-courses/content/C1/L4/LustreTestVMs.pdf) and [the official documentation, especially the network configuring section](http://doc.lustre.org/lustre_manual.xhtml#lnet_config).

The document in its current form has not much to do with those ones but I definitely got them as a starting point.

This is an offline version included in OSS repo but maybe the online version in gist is a bit more updated. To check it, see the README.md of this repo.

## Overview

Note: I am a newbie to Lustre so this overview is extremely light and specific, and probably I'm omitting a lot of important stuff.

The basic architecture we are aiming for, with the smallest number of different nodes possible, is made up of 4 machines:
* MGT, which manages the MDT and OSS nodes
* MDT, which stores file metadata
* OSS, which stores file objects
* Client, which mounts and access the whole filesystem.

### Note on changes

The original document:

* was based on Centos 6; this is for Centos 7
* assumed use of ZFS; this one uses Ext
* used three different nodes; I'm trying to use four Virtualbox VMs here
* Isn't really extensive and easy; this one hopefully is for noobs

### Note on instructions

The commands are presented in shell-script form, so that they can be easily copied and pasted into a terminal window.  *You should never blindly copy strings of commands into a terminal, especially as a privileged user.*  They are presented here because many of the commands must be replicated in multiple nodes.

## Instructions

 * Create a VM with a 5GB VHD, dynamically allocated, with 1024M RAM and install the latest (at the time of writing, 7.5) CentOS minimal ISO. The partition table can be set to automatic in the installation process.
 
 * Log in as root.
 
 * Enable internet connection by running ```dhclient -v```

 * Enable lustre repos:
```bash
# Update the system
yum update
# Install wget
yum -y install wget
# Download the .repo file from this Gist
wget https://git.io/fN2S7
# Due to the link shortener functionality, the file is downloaded as fN2S7, so we have to rename it
mv fN2S7 lustre.repo
# Now we can move it to its home
mv lustre.repo /etc/yum.repos.d/
```

* Install Lustre packages
```bash
# Update the repositories
yum update
# If e2fsprogs was not automatically updated in the previous step, update it manually
yum upgrade -y e2fsprogs

# Install lustre-tests
yum install -y lustre-tests
```

* create lnet module configuration (use appropriate interconnect in place of
'tcp0' and appropriate interface in place of 'eth0')

```bash
# Open the lnet.conf file
vi /etc/modprobe.d/lnet.conf
# it should look like:
# options lnet networks=tcp0(eth0)
```

* Disable SELinux
```bash
# Open the file to edit
vi /etc/sysconfig/selinux
# Set SELINUX=disabled
```

* There is now a Lustre kernel installed; reboot to activate running `reboot`

Now we need to set up several different nodes on separate VMs.

* Clone the VM three times. Call the four VMs *client*, *MGT* (management), MDT (metadata)
and *OSS* (storage). Make sure that MAC addresses are changed each time you clone.

* For each VM, go to configuration and activate the network adapter 2, in Bridge Adapter mode. Also, for the first network adapter of each VM, go to _Advanced_ - _Port forwarding_ and create a rule for TCP port 988, since
it's the one Lustre LNET uses. You may need to reactivate the Internet connection running ```dhclient -v```

* After booting again, use ```ip addr show``` to get the IP address of each VM. There should be three
network devices, one of them with a IPv4 number (probably enp0s8). Write down these IPs and the machine they
correspond to.

* In each VM open the port 988 in the CentOS firewall, by running
```firewall-cmd --zone=public --add-port=998/tcp --permanent```. You can check if your zone is different with
```firewall-cmd --get-active-zones```. Apply the changes with ```firewall-cmd --reload```.

* In each VM run ```lnet start``` and add a new network with ```lnetctl net add --net tcp0 --if enp0s8```. If your
interface is not called enp0s8, just use the name of your bridge adapter (you can check it with ```ip addr```). Now
check that it's running with ```lnetctl net show -v```, its status should be ```up```.

* Add the VMs to the /etc/hosts file of each VM. Of course the same machine should not have itself because
Lustre does not accept loopback connections.

* On the MDT, MGT and OSS only, have the `lnet` module auto-load on boot:
```bash
# Create a lnet.conf file in /etc/modules-load.d so the loader
# daemon knows which module to load
cd /etc/modules-load.d
vi lnet.conf
# The file should contain just lnet
# Reboot the machine and check that the module is loaded: lsmod | grep lnet
```

 * On the MGT:
 
   * Create another SATA virtual drive to use it as MGT storage with 10GB.
   * Check the name of the storage drive with ```lsblk```. It usually is /dev/sdb.
   * Create a lustre MGT:
   ```mkfs.lustre --mgs /dev/sdX```
   Or
   ```mkfs.lustre --reformat --mgs /dev/sdX```
   * Mount the disk:
   ```bash
   # Create the directory
   mkdir /mnt/mgt
   # Run lnet module
   lnet start
   # And mount
   mount -t lustre /dev/sdb /mnt/mgt
   ```
   
 * On the MDT:
 
   * Create another SATA virtual drive to use it as MDT storage with 10GB.
   * Check the name of the storage drive with ```lsblk```. It usually is /dev/sdb.
   * Create a lustre MDT:
   ```mkfs.lustre --fsname=lustre --mgsnode=mgsmds@tcp --mdt --index=0 /dev/sdb```
   Or
   ```mkfs.lustre --reformat --fsname=lustre --mgsnode=mgt@tcp --mdt --index=0 /dev/sdb```
   * Mount the disk:
   ```bash
   # Create the directory
   mkdir /mnt/mdt
   # Run lnet module
   lnet start
   # And mount
   mount -t lustre /dev/sdb /mnt/mdt
   ```
     
 * On the OSS:
 
   * Create another SATA virtual drive to use it as MDT storage with 10GB.
   * Check the name of the storage drive with ```lsblk```. It usually is /dev/sdb.
   * Create a lustre OST:
   ```mkfs.lustre --ost --fsname=lustre --mgsnode=mgt@tcp --index=0 /dev/sdb```
   Or
   ```mkfs.lustre --reformat --ost --fsname=lustre --mgsnode=mgt@tcp --index=0 /dev/sdb```
   * Mount the disk:
   ```bash
   # Create the directory
   mkdir /mnt/oss
   # Run lnet module
   lnet start
   # And mount
   mount -t lustre /dev/sdb /mnt/oss
   ```
   
 * On the client:
 
   ```bash
   # Create the directory to mount the filesystem
   mkdir /mnt/lustre
   # Run lnet module
   lnet start
   # Mount the thing
   mount -t lustre mgt@tcp:/lustre /mnt/lustre
   # After that, you should be able to access the filesystem from /mnt/lustre
   ```
   
 ## After rebooting the VMs
   When starting the machines after a shutdown, only two things have to be done.
   * Set up the network again:
   ```bash
   # Run the lnet module
   lnet start
   # Set up the network
   lnetctl net add --net tcp0 --if enp0s8
   # Check that the network is up
   lnetctl net show
   # The status should be up
   ```
   * Mount the device. The order is important and the first node to be mounted should be
   the MGT. It should be:
   ```bash
   # MGT
   mount -t lustre /dev/sdb /mnt/mgt
   # MDT
   mount -t lustre /dev/sdb /mnt/mdt
   # OSS
   mount -t lustre /dev/sdb /mnt/oss
   # Client
   mount -t lustre mgt@tcp:/lustre /mnt/lustre
   ```
   
# Testing
I use IOR to test the system. WIP
