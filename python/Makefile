.PHONY: default
default:
	pip3.7 install --user -r requirements.txt
	python3.7 -m unittest discover tests/
	python3.7 setup.py bdist_wheel

.PHONY: clean
clean:
	rm -fr build
	rm -fr DrMockGenerator.egg-info
	rm -fr dist
