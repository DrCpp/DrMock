.PHONY: default
default:
	pip install --user -r requirements.txt
	python -m unittest discover tests/
	python setup.py bdist_wheel

.PHONY: clean
clean:
	rm -fr build
	rm -fr DrMockGenerator.egg-info
	rm -fr dist
	rm -fr mocker/__pycache__
	rm -f mocker/mocker.cfg
