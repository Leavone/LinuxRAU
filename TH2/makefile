all: demo

demo: demo.cpp parallel_scheduler.so
	g++ -o demo demo.cpp parallel_scheduler.so

parallel_scheduler.so: parallel_scheduler.cpp
	g++ -fPIC -shared parallel_scheduler.cpp -o parallel_scheduler.so

clean:
	rm demo parallel_scheduler.so
