all: assignmentOne

assignmentOne:
	cd assignment1 && $(MAKE)

clean:
	cd assignment1 && $(MAKE) clean