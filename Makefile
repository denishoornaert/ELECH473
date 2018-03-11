all: assignmentOne assignmentTwo

clean: clean_assignmentOne clean_assignmentTwo
	
test: test_assignmentOne test_assignmentTwo

assignmentOne:
	cd assignment1 && $(MAKE)

assignmentTwo:
	cd assignment2 && $(MAKE)

clean_assignmentOne:
	cd assignment1 && $(MAKE) clean

clean_assignmentTwo:
	cd assignment2 && $(MAKE) clean

test_assignmentOne:
	cd assignment1 && $(MAKE) test

test_assignmentTwo:
	cd assignment2 && $(MAKE) test


install_requests:
	pip3 install requests