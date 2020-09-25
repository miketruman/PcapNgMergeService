all:
	g++ main.cpp --static -lPcap++ -lPacket++ -lCommon++  -lpcap -lboost_program_options -lboost_filesystem -lboost_system  -lboost_date_time  -std=c++11 -o a.bin
