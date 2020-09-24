#include <cstdlib>
#include <list>
#include <cstring>
#include <stdint.h>
#include <iterator>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include "boost/filesystem.hpp"
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <new>
#include <set>
#include <time.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <arpa/inet.h>

#include "pcapplusplus/IPv4Layer.h"
#include "pcapplusplus/Packet.h"
#include "pcapplusplus/PcapFileDevice.h"
#include "pcapplusplus/PacketUtils.h"

using namespace boost::posix_time;

int main(int argc, char **argv)
{
    boost::filesystem::create_directory("/input/");
    boost::filesystem::create_directory("/output/");

    while(true)
    {
        bool addDelay(true);
        std::cout << "#### MergePcap ############################" << std::endl;

        size_t mergeCount(50);

        ptime now_2 = microsec_clock::universal_time();
        std::string outFilename(std::string(to_iso_string(now_2)) + ".pcapng");

        std::string tmpFile("/output/." + outFilename);
        std::string closeFile("/output/" + outFilename);
        pcpp::PcapNgFileWriterDevice pcapNgWriter(tmpFile.c_str());
        pcapNgWriter.open();

        boost::filesystem::path p ("/input/");


        boost::filesystem::directory_iterator end_itr;
        size_t count(0);
        for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
        {
            ++count;
            if (is_regular_file(itr->path()) && itr->path().filename().string().substr(0,1) != ".") {
                std::string current_file = itr->path().string();
                std::cout << current_file << std::endl;

                pcpp::PcapNgFileReaderDevice pcapReader(current_file.c_str());
                pcapReader.open();

                pcpp::RawPacket rawPacket;
                std::string comment;
                while (pcapReader.getNextPacket(rawPacket, comment)) {
                    pcapNgWriter.writePacket(rawPacket, comment.c_str());
                }

                pcapReader.close();
                boost::filesystem::remove(current_file);
                addDelay = false;
            }
            if(count == mergeCount)
            {
                std::cout << "BREAK" << std::endl;
                break;
            }
        }
        if(count < mergeCount)
	{
		std::cout << "only(" << count << ") file merged" << std::endl;
            	sleep(1);
	}
        pcapNgWriter.close();
        boost::filesystem::rename(tmpFile,closeFile);
        std::cout << "closeFile=" << closeFile << std::endl;
        if(addDelay)
        {
            sleep(10);
        }
    }
}
