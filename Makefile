# Build:  make        Run:  make run        Test:  make test
waste_classifier: src/waste_classifier.c
	gcc -Wall -Wextra src/waste_classifier.c -o waste_classifier

run: waste_classifier
	./waste_classifier

test:
	bash tests/run_tests.sh

clean:
	rm -f waste_classifier
