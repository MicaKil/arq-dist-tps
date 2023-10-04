import os
import subprocess

f = open('passwd.txt' ,'r')
ip_out = open('ips.txt', 'w')
lines = f.readlines()

base = lines[0].replace("\n", "")
for line in lines[1:]:
    if line.startswith("#"): continue
    line = line.replace("\n","")
    ip, pas, *_ = (line + " ").split(" ")
    ip = f"10.65.1.{ip}"
    pas = base if pas == '' else pas
    print((ip, pas))
    os.system(f"/bin/sh -c 'sshpass -v -p {pas} ssh-copy-id -i ~/.ssh/id_rsa.pub -o StrictHostKeyChecking=no -f mpiuser@{ip}'")
    ip_out.write(ip+"\n")

f.close()
ip_out.close()
