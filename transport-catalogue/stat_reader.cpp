#include "stat_reader.h"
#include <iostream>

void ParseAndPrintStat(const transport::TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output) {

    std::string_view input = request.substr(0, request.find(' '));
    std::string_view in = request.substr(request.find(' ') + 1, request.size());
    std::string out;
    if(input == "Bus"){
        out = tansport_catalogue.PrintBusInfo((in));
    }
    else if(input == "Stop"){
        out = tansport_catalogue.PrintStopBuses(in);
    }

    output << out << std::endl;
}