
pushd .
cd $(dirname "$0") # $0 is the script's path
# Build
tup
# Write
sudo mkdir /mnt/roxus
sudo mount $1 /mnt/roxus
sudo cp -r root/* /mnt/roxus
sudo umount /mnt/roxus
sudo rm -rf /mnt/roxus
# Return
popd
