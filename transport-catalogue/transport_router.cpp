 #include "transport_router.h"

#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>

namespace transport_router {

    TransportRouter::TransportRouter(const TransportCatalogue& catalogue) :
        catalogue_(catalogue)
    {}

    void TransportRouter::SetSettingsAndBuild(RouteSettings settings) {
        settings_ = std::move(settings);
        BuildRoute();
    }

    std::optional<RouteItems> TransportRouter::GetRouteInfo(const std::string_view from, const std::string_view to) const {
        RouteItems items_info;

        auto from_stop = catalogue_.FindStop(from);
        auto to_stop = catalogue_.FindStop(to);

        auto from_vertex = GetVertexFromStop(from_stop);
        auto to_vertex = GetVertexFromStop(to_stop);

        auto router_info = transport_router_->BuildRoute(
            from_vertex.first,
            to_vertex.first
        );

        if (router_info.has_value()) {
            items_info.total_time = router_info.value().weight;
            for (const auto& edge : router_info.value().edges) {
                items_info.items.push_back(edge_to_item_.at(edge));
            }
            return items_info;
        }

        return std::nullopt;
    }

    double TransportRouter::DistanceIntoTime(double distance) const {
        return (distance * 60) / (settings_.bus_velocity * 1000);
    }

    const std::pair<graph::VertexId, graph::VertexId>& TransportRouter::GetVertexFromStop(const Stop* stop) const {
        auto vertex = stop_to_vertex_.find(stop);
        return vertex->second;
    }

    void TransportRouter::AddStopsIntoGraph() {
        std::size_t v = 0;
        auto stops = catalogue_.GetStops();

        for (const Stop& stop : stops) {
            auto find_stop = catalogue_.FindStop(stop.name);
            stop_to_vertex_.insert({find_stop, {v, v + 1}});

            graph::EdgeId edge = transport_graph_->AddEdge({v, v + 1, settings_.bus_wait_time});
            Item item({"Wait", stop.name, settings_.bus_wait_time, 1});
            edge_to_item_.insert({edge, item});

            v += 2;
        }
    }

    void TransportRouter::AddBusEdgeIntoGraph(
        const Stop* from, 
        const Stop* to,
        const std::string_view bus_name,
        int span_count,
        double distance
    ) {
        Item item({"Bus", std::string(bus_name), DistanceIntoTime(distance), span_count});

        auto from_vertex = GetVertexFromStop(from);
        auto to_vertex = GetVertexFromStop(to);

        graph::EdgeId edge = transport_graph_->AddEdge({from_vertex.second, to_vertex.first, DistanceIntoTime(distance)});
        edge_to_item_.insert({edge, item});
    }

    void TransportRouter::AddBusIntoGraph(const Bus& bus) {
        for (std::size_t i = 0; i < (bus.stops.size() - 1); ++i) {
            double from_to_distance = 0.0;
            double to_from_distance = 0.0;

            const Stop* i_from = bus.stops[i];

            for (std::size_t j = i; j < (bus.stops.size() - 1); ++j) {
                const Stop* from = bus.stops[j];
                const Stop* to = bus.stops[j + 1];

                from_to_distance += catalogue_.GetDistanceBetweenStops(from, to);
                AddBusEdgeIntoGraph(i_from, to, bus.name, j + 1 - i, from_to_distance);

                if (!bus.is_roundtrip) {
                    to_from_distance += catalogue_.GetDistanceBetweenStops(to, from);
                    AddBusEdgeIntoGraph(to, i_from, bus.name, j + 1 - i, to_from_distance);
                }

            }
        }
    }

    void TransportRouter::BuildRoute() {
        transport_graph_ = std::make_unique<graph::DirectedWeightedGraph<double>>(
            catalogue_.GetStops().size() * 2
        );
        AddStopsIntoGraph();

        auto buses = catalogue_.GetBuses();
        for (const Bus& bus : buses) {
            AddBusIntoGraph(bus);
        }

        transport_router_ = std::make_unique<graph::Router<double>>(*transport_graph_);
    }

} // namespace transport_router