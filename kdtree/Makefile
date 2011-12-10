all: meta_kdtree test_kdtree

meta_kdtree: meta_kdtree.cpp 
	g++ -o meta_kdtree skdtree.cpp meta_kdtree.cpp ../partition.c ../myutil.c ../filter.c -g

test_kdtree: test_kdtree.cpp 
	g++ -o test_kdtree test_kdtree.cpp

clean:
	rm meta_kdtree test_kdtree *.o -rf
