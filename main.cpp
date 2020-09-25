#include <boost/program_options.hpp>
#include "boost/filesystem.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "pcapplusplus/Packet.h"
#include "pcapplusplus/PcapFileDevice.h"

int main(int argc, char **argv)
{
    boost::filesystem::create_directory("/input/");
    boost::filesystem::create_directory("/input/failed/");
    boost::filesystem::create_directory("/output/");

    while(true)
    {
        bool addDelay(true);
        std::cout << "#### MergePcap ############################" << std::endl;

        size_t mergeCount(50);

        boost::posix_time::ptime now_2 = boost::posix_time::microsec_clock::universal_time();
        std::string outFilename(boost::posix_time::to_iso_string(now_2) + ".pcapng");

        std::string tmpFile("/output/." + outFilename);
        std::string closeFile("/output/" + outFilename);
        pcpp::PcapNgFileWriterDevice pcapNgWriter(tmpFile.c_str());
        pcapNgWriter.open();

        boost::filesystem::path p ("/input/");


        boost::filesystem::directory_iterator end_itr;
        size_t count(0);
        for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
        {
            if (is_regular_file(itr->path()) && itr->path().filename().string().substr(0,1) != ".")
            {
                std::string current_file = itr->path().string();
                std::cout << current_file << std::endl;

                pcpp::IFileReaderDevice* reader = pcpp::IFileReaderDevice::getReader(current_file.c_str());
                if(reader && reader->open())
                {
                    ++count;
                    pcpp::PcapNgFileReaderDevice* pcapNgReader(dynamic_cast<pcpp::PcapNgFileReaderDevice*>(reader));
                    pcpp::PcapFileReaderDevice* pcapReader(dynamic_cast<pcpp::PcapFileReaderDevice*>(reader));
                    if(pcapNgReader)
                    {
                        pcpp::RawPacket rawPacket;
                        std::string comment;
                        while (pcapNgReader->getNextPacket(rawPacket, comment)) {
                            pcapNgWriter.writePacket(rawPacket, comment.c_str());
                        }
                    }
                    else if(pcapReader)
                    {
                        pcpp::RawPacket rawPacket;
                        while (pcapReader->getNextPacket(rawPacket)) {
                            pcapNgWriter.writePacket(rawPacket);
                        }
                    }
                    boost::filesystem::remove(current_file);
                    addDelay = false;
                    reader->close();
                }
                else
                {
                    std::string failedFile("/input/failed/" + itr->path().filename().string());
                    std::cout << "Failed:" << current_file << " file moved to:" << failedFile << std::endl;
                    boost::filesystem::rename(current_file, failedFile);
                }
            }
            if(count == mergeCount)
            {
                std::cout << "BREAK" << std::endl;
                break;
            }
        }
        pcapNgWriter.close();
        boost::filesystem::rename(tmpFile,closeFile);
        if(count < mergeCount)
        {
            std::cout << "only(" << count << ") file merged" << std::endl;
            sleep(1);
        }
        std::cout << "closeFile=" << closeFile << std::endl;
        if(addDelay)
        {
            sleep(10);
        }
    }
}
