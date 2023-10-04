import os
import subprocess
import sys
f = open('ips.txt' ,'r')
fileToCopy = sys.argv[1]
path = "/".join(fileToCopy.split("/")[:-1])
print(fileToCopy, path)
lines = f.readlines()
for line in lines:
	line = line.strip()
	command = f"/bin/sh -c 'scp -r {fileToCopy} {line}:/home/mpiuser/Mica/{path}/'"
	print(command)
	os.system(command)
f.close()
