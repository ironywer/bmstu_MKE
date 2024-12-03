all: test bench

test: 
	./run_test.sh

bench:
	./run_becnhmarks.sh

clean:
	rm -rf results
	rm -rf program