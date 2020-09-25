#include <boost/program_options.hpp>
#include "boost/filesystem.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "pcapplusplus/Packet.h"
#include "pcapplusplus/PcapFileDevice.h"

//using namespace boost::program_options;

int main(int argc, char **argv)
{
    size_t mergeCount(50);
    size_t penalty(1);
    size_t yield(10);

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
    ("help,h", "print usage message")
    ("mergecount,m", boost::program_options::value(&mergeCount), "number of files to merge before creating new file")
    ("penalty,p", boost::program_options::value(&penalty), "Penalty duration when less then mergecount was found")
    ("yield,y", boost::program_options::value(&yield), "yield duration when no files where found")
    ;

    boost::program_options::variables_map vm;
    boost::program_options::store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    if (vm.count("mergecount")) {
        mergeCount =  vm["mergecount"].as<size_t>();
    }
    if (vm.count("penalty")) {
        penalty =  vm["penalty"].as<size_t>();
    }
    if (vm.count("yield")) {
        yield =  vm["yield"].as<size_t>();
    }
    std::cout << "STARTING #########################################" << std::endl;
    std::cout << "mergeCount=" << mergeCount << std::endl;
    std::cout << "penalty=" << penalty << std::endl;
    std::cout << "yield=" << yield << std::endl;

    boost::filesystem::create_directory("/input/");
    boost::filesystem::create_directory("/input/failed/");
    boost::filesystem::create_directory("/output/");

    while(true)
    {
        bool addDelay(true);
        std::cout << "#### MergePcap ############################" << std::endl;


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
        if(count && count < mergeCount)
        {
            std::cout << "only(" << count << ") file merged. Penalty time:" << penalty << std::endl;
            sleep(penalty);
        }
        std::cout << "closeFile=" << closeFile << std::endl;
        if(addDelay)
        {
            std::cout << "No input files found. yield for:" << yield << "sec" << std::endl;
            sleep(yield);
        }
    }
}
