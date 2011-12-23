#!/usr/local/bin/python

import time

uid_querys = range(0, 1)   # just root
size_querys = []           # size query
ctime_querys = []
mtime_querys = []

#querys below 1KB
for i in range(1, 1000, 10) : 
    query = (i, i + 10)
    size_querys.append(query)


#querys between 1K ~ 1M
for i in range(1, 1000, 10) : 
    query = (i * 1024, (i + 10) * 1024)
    size_querys.append(query)


#querys between 1M - 1G
for i in range(1, 1000, 500) :
    query = (i * 1024 * 1024, (i + 200) * 1024 * 1024)
    size_querys.append(query)

#query by time
daylong = 24 * 60 * 60
now = time.time()
today = now - now % daylong + daylong     # today

days = []
days.append(("20111223", "20111225"))      # today 
days.append(("20111222", "20111225"))      # yesterday
days.append(("20111221", "20111225"))      # the day before yesterday
days.append(("20111217", "20111225"))      # rencent week
days.append(("20111124", "20111225"))      # rencent month

for day in days :
    ctime_querys.append(day)
    mtime_querys.append(day)

#print uid_querys
#print size_querys
#print ctime_querys
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

# just query ctime
for i in ctime_querys :
    query = "query" + " -a " + i[0] + " " + i[1]
    all_querys.append(query)

# just query mtime
for i in mtime_querys :
    query = "query" + " -c " + i[0] + " " + i[1]
    all_querys.append(query)


# mix uid with size
for i in  uid_querys :
    for j in size_querys :
        query = "query" + " -u " + str(i) + " -s " + str(j[0]) + " " + str(j[1]) 
        all_querys.append(query)

# mix uid with ctime
for i in uid_querys :
    for j in ctime_querys: 
        query = "query" + " -u " + str(i) + " -a " + j[0] + " " + j[1]
        all_querys.append(query)

# mix uid with mtime
for i in uid_querys :
    for j in mtime_querys: 
        query = "query" + " -u " + str(i) + " -c " + j[0] + " " + j[1]
        all_querys.append(query)

# mix size with ctime
for i in size_querys :
    for j in ctime_querys :
        query = "query" + " -s " + str(i[0]) + " " + str(i[1]) + " -a " + j[0] + " " + j[1]
        all_querys.append(query)
 
# mix size with mtime
for i in size_querys :
    for j in mtime_querys :
        query = "query" + " -s " + str(i[0]) + " " + str(i[1]) + " -c " + j[0] + " " + j[1]
        all_querys.append(query)

# print all_querys
print len(all_querys)

# write querys to file
fileobj = open("querys.conf", "w+")
for i in all_querys :
    fileobj.writelines(i)
    fileobj.writelines("\n")
fileobj.close()
