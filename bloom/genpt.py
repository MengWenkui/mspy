#!/usr/local/bin/python

import os

dirs = os.listdir("/")
file_obj = open("partition.conf", "w+")

for i in dirs:
    path = "/" + i
    if (i[0] == "." or i == "proc") :
        continue
    files = os.listdir(path)
    for j in files:
        file = path +  "/" + j
        if(j[0] != "." and os.path.isdir(file)) :
            file_obj.writelines(file)
            file_obj.writelines("\n")

file_obj.close()
    
