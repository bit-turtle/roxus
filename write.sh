# Build and Write
tup
sudo mkdir /mnt/roxus
sudo mount $1 /mnt/roxus
sudo cp -r root/* /mnt/roxus
sudo umount /mnt/roxus
sudo rm -rf /mnt/roxus
