all: meta_kdtree test_kdtree skdtree_test

skdtree_test: skdtree_test.cpp skdtree.cpp  ../partition.c ../myutil.c ../filter.c
	g++ -o $@ $^ -g

meta_kdtree: meta_kdtree.cpp  skdtree.cpp  ../partition.c ../myutil.c ../filter.c
	g++ -o $@ $^ -g

test_kdtree: test_kdtree.cpp 
	g++ -o $@ $^ -g

clean:
	rm meta_kdtree test_kdtree skdtree_test *.o -rf
