#pragma once

#include "transport_catalogue.h"
#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"

namespace json_reader {

    using namespace transport_catalogue;

    class JsonReader {
    public:
        JsonReader(std::istream& input);

        void ApplyCommands(TransportCatalogue& catalogue) const;
        void ApplySettingsCommands(renderer::MapRenderer& renderer) const;
        void PrintJson(const RequestHandler& request_handler, std::ostream& out) const;

    private:
        const json::Document doc_;

        const json::Array* request_commands_;
        const json::Array* stat_commands_;
        const json::Dict* render_settings_;

        void CommandDistribution(json::Array& only_stop_commands, json::Array& only_bus_commands, TransportCatalogue& catalogue) const;
        void StopsHandle(json::Array& only_stop_commands, TransportCatalogue& catalogue) const;
        void BusesHandle(json::Array& only_bus_commands, TransportCatalogue& catalogue) const;

        void PrintBusInfo(json::Array& res, const std::optional<BusInfo>& bus_info, int id) const;
        void PrintStopInfo(json::Array& res, const std::optional<std::unordered_set<std::string_view>>& stop_info, int id) const;
    };

}  // namespace json_reader