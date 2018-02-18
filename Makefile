all: assignmentOne

assignmentOne:
	cd assignment1 && $(MAKE)

clean:
	cd assignment1 && $(MAKE) clean

test:
	cd assignment1 && $(MAKE) test

install_requests:
	pip3 install requests