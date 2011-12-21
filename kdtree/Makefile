all: meta_kdtree test_kdtree skdtree_test point_query range_query write_disk log_update

log_update: log_update.cpp skdtree.cpp  ../partition.c ../myutil.c ../filter.c
	g++ -o $@ $^ -g

write_disk: write_disk.cpp skdtree.cpp  ../partition.c ../myutil.c ../filter.c
	g++ -o $@ $^ -g

range_query: range_query.cpp skdtree.cpp  ../partition.c ../myutil.c ../filter.c
	g++ -o $@ $^ -g

point_query: point_query.cpp skdtree.cpp  ../partition.c ../myutil.c ../filter.c
	g++ -o $@ $^ -g

skdtree_test: skdtree_test.cpp skdtree.cpp  ../partition.c ../myutil.c ../filter.c
	g++ -o $@ $^ -g

meta_kdtree: meta_kdtree.cpp  skdtree.cpp  ../partition.c ../myutil.c ../filter.c
	g++ -o $@ $^ -g

test_kdtree: test_kdtree.cpp 
	g++ -o $@ $^ -g

clean:
	rm meta_kdtree test_kdtree skdtree_test point_query range_query write_disk log_update *.o -rf
