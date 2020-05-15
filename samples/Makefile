dirs = basic example mock qt states

.PHONY: default
default:
	for dir in $(dirs); do cd $$dir; make clean && make; cd ..; done

.PHONY: clean
clean:
	for dir in $(dirs); do cd $$dir; make clean; cd ..; done
