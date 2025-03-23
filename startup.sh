make clean && make
sudo insmod ldd.ko
lsmod | grep ldd
ls /proc | grep pyjama
sudo dmesg

# here is how to read and write to the proc file
# echo "hi pyjama driver" > /proc/pyjama_driver # write
# cat /proc/pyjama_driver # read

