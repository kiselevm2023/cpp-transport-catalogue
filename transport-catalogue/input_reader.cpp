#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>

namespace transport_catalogue {

namespace detail {

Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma_first = str.find(',');

    if (comma_first == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma_first + 1);
    auto comma_second = str.find(',', not_space2);

    double lat = std::stod(std::string(str.substr(not_space, comma_first - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2, comma_second - not_space2)));

    return {lat, lng};
}

std::vector<Distance> ParseStopsDistance(std::string_view str) {
    std::vector<Distance> res;

    auto comma_first = str.find(',');
    auto comma_second = str.find(',', comma_first + 1);

    auto not_space = str.find_first_not_of(' ', comma_second + 1);
    while (not_space != str.npos) {
        auto metre = str.find('m', not_space);
        int distance = std::stoi(std::string(str.substr(not_space, metre - not_space)));

        auto not_space2 = str.find_first_not_of(' ', metre + 1);
        auto space = str.find(' ', not_space2);
        auto not_space3 = str.find_first_not_of(' ', space);
        auto comma = str.find(',', not_space3);
        std::string stop_name = std::string(str.substr(not_space3, comma - not_space3));

        res.push_back({distance, stop_name});

        if (comma == str.npos) {
            not_space = comma;
        } else {
            not_space = str.find_first_not_of(' ', comma + 1);
        }
    }

    return res;
}

std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}
} 

void InputReader::ParseLine(std::string_view line) {
    auto command_description = detail::ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

CommandType GetCommandType(std::string_view command) {
    if (command == "Stop"s) {
        return CommandType::STOP;
    } else if (command == "Bus"s) {
        return CommandType::BUS;
    }
    return CommandType::UNKNOWN;
}

void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) const {
    std::vector<CommandDescription> only_stop_commands;
    std::vector<CommandDescription> only_bus_commands;

    for (const auto& command : commands_) {
        auto command_type = GetCommandType(command.command);
        if (command_type == CommandType::STOP) {
            catalogue.AddStop({command.id, detail::ParseCoordinates(command.description)});
            only_stop_commands.push_back(std::move(command));
        } else if (command_type == CommandType::BUS) {
            only_bus_commands.push_back(std::move(command));
        }
    }

    for (const auto& command : only_stop_commands) {
        auto distances = detail::ParseStopsDistance(command.description);
        if (!distances.empty()) {
            for (const auto& distance : distances) {
                catalogue.SetDistanceBetweenStops(
                    catalogue.FindStop(command.id), catalogue.FindStop(distance.stop_name), distance.distance
                );
            }
        }
    }

    for (const auto& command : only_bus_commands) {

        std::vector<const Stop*> stops;

        for (const auto& stop_name : detail::ParseRoute(command.description)) {
        
            const Stop* stop_ptr = catalogue.FindStop(stop_name);
            stops.push_back(stop_ptr);
        }
            
       catalogue.AddBus(Bus{command.id, std::move(stops)});
    }
}
} 