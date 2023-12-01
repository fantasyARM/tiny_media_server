#g++ -c event_engine.cc -I ./ -I ../common -std=c++11
#g++ -c processor_udp.cc -I ./ -I ../common -std=c++11
g++ -g -o tinyserver main.cc event_engine.cc processor_udp.cc -I ./ -I ../common -std=c++11 -lpthread
