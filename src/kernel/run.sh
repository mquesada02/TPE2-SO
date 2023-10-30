#!/bin/bash
sudo docker start SO
sudo docker exec -it SO make clean -C/root/Toolchain
sudo docker exec -it SO make clean -C/root/
sudo docker exec -it SO make -C/root/Toolchain
sudo docker exec -it SO make -C/root/
sudo docker stop SO
sudo qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0
