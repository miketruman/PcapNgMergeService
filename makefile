all:
	astyle main.cpp
	g++ main.cpp -lPcap++ -lPacket++ -lCommon++ -lboost_iostreams  -lpcap -lboost_program_options -lboost_regex -lboost_filesystem -lboost_system  -lz -lpthread -lboost_date_time  -std=c++11 -o a.bin
	./a.bin
