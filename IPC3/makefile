all: first second

first: first.cpp shared-array.cpp
	g++ -o first first.cpp shared-array.cpp

second: second.cpp shared-array.cpp
	g++ -o second second.cpp shared-array.cpp

clean:
	rm first second
