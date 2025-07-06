#include "json_reader.h"

#include <string>
#include <sstream>
#include <utility>

namespace json_reader {

    using namespace std::literals;
    using namespace domain;
    using namespace geo;

namespace {

    std::string GetColor(const json::Node& value) {
        std::stringstream res;
        if (value.IsString()) {
            res << value.AsString();
        } else {
            const auto& color = value.AsArray();
            if (color.size() == 3) {
                res << "rgb("
                    << color[0].AsInt() << ','
                    << color[1].AsInt() << ','
                    << color[2].AsInt() << ')';
            } else {
                res << "rgba("
                    << color[0].AsInt() << ','
                    << color[1].AsInt() << ','
                    << color[2].AsInt() << ','
                    << color[3].AsDouble() << ')';
            }
        }
        return res.str();
    }

}  // namespace

    JsonReader::JsonReader (std::istream& input) :
        doc_(json::Load(input))
    {
        const auto& root = doc_.GetRoot().AsMap();
        request_commands_ = &root.at("base_requests").AsArray();
        stat_commands_ = &root.at("stat_requests").AsArray();
        render_settings_ = &root.at("render_settings").AsMap();
    }

    void JsonReader::ApplyCommands(TransportCatalogue& catalogue) const {
        json::Array only_stop_commands;
        json::Array only_bus_commands;

        CommandDistribution(only_stop_commands, only_bus_commands, catalogue);
        StopsHandle(only_stop_commands, catalogue);
        BusesHandle(only_bus_commands, catalogue);
    }

    void JsonReader::CommandDistribution(json::Array& only_stop_commands, json::Array& only_bus_commands, TransportCatalogue& catalogue) const {
        for (const auto& command : *request_commands_) {
            const auto& description = command.AsMap();
            const auto& type = description.at("type").AsString();
            if (type == "Stop") {
                catalogue.AddStop({description.at("name").AsString(),
                                  Coordinates{description.at("latitude").AsDouble(), 
                                              description.at("longitude").AsDouble()}});
                only_stop_commands.push_back(std::move(command));
            } else if (type == "Bus") {
                only_bus_commands.push_back(std::move(command));
            }
        }
    }

    void JsonReader::StopsHandle(json::Array& only_stop_commands, TransportCatalogue& catalogue) const {
        for (const auto& command : only_stop_commands) {
            const auto& description = command.AsMap();
            const auto& name = description.at("name").AsString();
            const auto& road_distances = description.at("road_distances").AsMap();
            if (!road_distances.empty()) {
                for (const auto& distance : road_distances) {
                    catalogue.SetDistanceBetweenStops(
                        catalogue.FindStop(name), catalogue.FindStop(distance.first), distance.second.AsInt()
                    );
                }
            }
        }
    }

    void JsonReader::BusesHandle(json::Array& only_bus_commands, TransportCatalogue& catalogue) const {
        for (const auto& command : only_bus_commands) {
            const auto& description = command.AsMap();
            const auto& route = description.at("stops").AsArray();
            std::vector<const Stop*> stops;
            for (const auto& stop : route) {
                stops.push_back(catalogue.FindStop(stop.AsString()));
            }
            bool is_roundtrip = description.at("is_roundtrip").AsBool();
            if (!is_roundtrip && route.size() > 1) {
                for (auto it = route.rbegin() + 1; it != route.rend(); ++it) {
                    const auto& stop = *it;
                    stops.push_back(catalogue.FindStop(stop.AsString()));
                }
            }
            catalogue.AddBus({description.at("name").AsString(),
                              std::move(stops),
                              is_roundtrip});
        }
    }

    void JsonReader::ApplySettingsCommands(renderer::MapRenderer& renderer) const {
        renderer::RenderSettings settings;
        for (const auto& [key, value] : *render_settings_) {
            if (key == "width") {
                settings.width = value.AsDouble();
            } else if (key == "height") {
                settings.height = value.AsDouble();
            } else if (key == "padding") {
                settings.padding = value.AsDouble();
            } else if (key == "line_width") {
                settings.line_width = value.AsDouble();
            } else if (key == "stop_radius") {
                settings.stop_radius = value.AsDouble();
            } else if (key == "bus_label_font_size") {
                settings.bus_label_font_size = value.AsInt();
            } else if (key == "bus_label_offset") {
                const auto& offset = value.AsArray();
                if (offset.size() == 2) {
                    settings.bus_label_offset[0] = offset[0].AsDouble();
                    settings.bus_label_offset[1] = offset[1].AsDouble();
                }
            } else if (key == "stop_label_font_size") {
                settings.stop_label_font_size = value.AsInt();
            } else if (key == "stop_label_offset") {
                auto offset = value.AsArray();
                if (offset.size() == 2) {
                    settings.stop_label_offset[0] = offset[0].AsDouble();
                    settings.stop_label_offset[1] = offset[1].AsDouble();
                }
            } else if (key == "underlayer_color") {
                settings.underlayer_color = GetColor(value);
            } else if (key == "underlayer_width") {
                settings.underlayer_width = value.AsDouble();
            } else if (key == "color_palette") {
                const auto& colors = value.AsArray();
                for (const auto& color : colors) {
                    settings.color_palette.push_back(GetColor(color));
                }
            }
        }
        renderer.SetSettings(settings);
    }

    void JsonReader::PrintJson(const RequestHandler& request_handler, std::ostream& out) const {
        json::Builder builder;
        builder.StartArray();
        for (const auto& command : *stat_commands_) {
            const auto& description = command.AsMap();
            const auto& type = description.at("type").AsString();
            if (type == "Bus") {
                const auto& bus_info = request_handler.GetBusStat(description.at("name").AsString());
                PrintBusInfo(builder, bus_info, description.at("id").AsInt());
            } else if (type == "Stop") {
                const auto& stop_info = request_handler.GetBusesByStop(description.at("name").AsString());
                PrintStopInfo(builder, stop_info, description.at("id").AsInt());
            } else if (type == "Map") {
                std::ostringstream map_out;
                request_handler.RenderMap().Render(map_out);
                builder.StartDict()
                            .Key("map").Value(map_out.str())
                            .Key("request_id").Value(description.at("id").AsInt())
                        .EndDict();
            }
        }
        builder.EndArray();
        json::Print(json::Document{builder.Build()}, out);
    }

    void JsonReader::PrintBusInfo(json::Builder& builder, const std::optional<BusInfo>& bus_info, int id) const {
        builder.StartDict()
                    .Key("request_id").Value(id);
        if (bus_info.has_value()) {
            builder.Key("curvature").Value(bus_info->curvature)
                    .Key("route_length").Value(double(bus_info->route_length))
                    .Key("stop_count").Value(static_cast<int>(bus_info->stops_on_route))
                    .Key("unique_stop_count").Value(static_cast<int>(bus_info->unique_stops));
        } else {
            builder.Key("error_message").Value(std::string("not found"));
        }
        builder.EndDict();
    }

    void JsonReader::PrintStopInfo(json::Builder& builder, const std::optional<std::unordered_set<std::string_view>>& stop_info, int id) const {
        builder.StartDict()
                    .Key("request_id").Value(id);
        if (stop_info.has_value()) {
            if (!stop_info->empty()) {
                builder.Key("buses").StartArray();
                std::set<std::string_view> sorted_stop_info((*stop_info).begin(), (*stop_info).end());
                for (const auto& bus_name : sorted_stop_info) {
                    builder.Value(std::string(bus_name));
                }
                builder.EndArray();
            } else {
                builder.Key("buses").StartArray().EndArray();
            }
        } else {
            builder.Key("error_message").Value(std::string("not found"));
        }
        builder.EndDict();
    }

}  // namespace json_reader