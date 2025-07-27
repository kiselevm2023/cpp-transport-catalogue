#pragma once

#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"

#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>

namespace transport_router {

using namespace transport_catalogue;

struct RouteSettings {
    double bus_wait_time;
    double bus_velocity;
};

struct Item {
    std::string type;
    std::string name;
    double time;
    int span;
};

struct RouteItems {
    double total_time;
    std::vector<Item> items;
};

class TransportRouter {
public:
    TransportRouter(const TransportCatalogue& catalogue);

    void SetSettingsAndBuild(RouteSettings settings);

    std::optional<RouteItems> GetRouteInfo(const std::string_view from, const std::string_view to) const;

private:
    const TransportCatalogue& catalogue_;
    RouteSettings settings_;

    std::unique_ptr<graph::DirectedWeightedGraph<double>> transport_graph_;
    std::unique_ptr<graph::Router<double>> transport_router_;

    std::unordered_map<const Stop*, std::pair<graph::VertexId, graph::VertexId>> stop_to_vertex_;
    std::unordered_map<graph::EdgeId, Item> edge_to_item_;

    double DistanceIntoTime(double distance) const;

    const std::pair<graph::VertexId, graph::VertexId>& GetVertexFromStop(const Stop* stop) const;

    void AddStopsIntoGraph();

    void AddBusEdgeIntoGraph(
        const Stop* from, 
        const Stop* to,
        const std::string_view bus_name,
        int span_count,
        double distance
    );

    void AddBusIntoGraph(const Bus& bus);

    void BuildRoute();

};

} // namespace transport_router