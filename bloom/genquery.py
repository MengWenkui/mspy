#!/usr/local/bin/python

import time

uid_querys = range(0, 40)  # query uid from 0-39, see /etc/passwd
size_querys = []           # size query
atime_querys = []
mtime_querys = []

#querys below 1KB
for i in range(0, 1000, 100) : 
    query = (i, i + 100)
    size_querys.append(query)


#querys between 1K ~ 1M
for i in range(0, 1000, 10) : 
    query = (i * 1024, (i + 10) * 1024)
    size_querys.append(query)


#querys between 1M - 1G
for i in range(0, 1000, 10) :
    query = (i * 1024 * 1024, (i + 10) * 1024 * 1024)
    size_querys.append(query)

#query by time
daylong = 24 * 60 * 60
now = time.time()
today = now - now % daylong + daylong     # today

days = []
days.append(("20111223", "2011124"))      # today 
days.append(("20111222", "2011124"))      # yesterday
days.append(("20111221", "2011124"))      # the day before yesterday
days.append(("20111217", "2011124"))      # rencent week
days.append(("20111124", "2011124"))      # rencent month

for day in days :
    atime_querys.append(day)
    mtime_querys.append(day)

#print uid_querys
#print size_querys
#print atime_querys
#print mtime_querys

all_querys = []

# just query uid
for i in uid_querys :
    query = "query" + " -u " + str(i)
    all_querys.append(query)

# just query size
for i in size_querys :
    query = "query" + " -s " + str(i[0]) + " " + str(i[1])
    all_querys.append(query)

# just query atime
for i in atime_querys :
    query = "query" + " -a " + i[0] + " " + i[1]
    all_querys.append(query)

# just query mtime
for i in mtime_querys :
    query = "query" + " -m " + i[0] + " " + i[1]
    all_querys.append(query)


# mix uid with size
for i in  uid_querys :
    for j in size_querys :
        query = "query" + " -u " + str(i) + " -s " + str(j[0]) + " " + str(j[1]) 
        all_querys.append(query)

# mix uid with atime
for i in uid_querys :
    for j in atime_querys: 
        query = "query" + " -u " + str(i) + " -a " + j[0] + " " + j[1]
        all_querys.append(query)

# mix uid with mtime
for i in uid_querys :
    for j in mtime_querys: 
        query = "query" + " -u " + str(i) + " -m " + j[0] + " " + j[1]
        all_querys.append(query)

# mix size with atime
for i in size_querys :
    for j in atime_querys :
        query = "query" + " -s " + str(i[0]) + " " + str(i[1]) + " -a " + j[0] + " " + j[1]
        all_querys.append(query)
 
# mix size with mtime
for i in size_querys :
    for j in mtime_querys :
        query = "query" + " -s " + str(i[0]) + " " + str(i[1]) + " -m " + j[0] + " " + j[1]
        all_querys.append(query)

# print all_querys
print len(all_querys)

# write querys to file
fileobj = open("querys.conf", "w+")
for i in all_querys :
    fileobj.writelines(i)
    fileobj.writelines("\n")
fileobj.close()
