#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

using namespace std::literals;

namespace transport_catalogue {

enum class CommandType { BUS, STOP, UNKNOWN };

CommandType GetCommandType(const std::string_view& command);

struct CommandDescription {
    explicit operator bool() const {
        return !command.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    std::string command;      // Название команды
    std::string id;           // id маршрута или остановки
    std::string description;  // Параметры команды
};

class InputReader {
public:
    void ParseLine(std::string_view line); // Парсит строку в структуру CommandDescription и сохраняет результат в commands_

    void ApplyCommands(TransportCatalogue& catalogue) const; // Наполняет данными транспортный справочник, используя команды из commands_

private:
    std::vector<CommandDescription> commands_;
};

} 