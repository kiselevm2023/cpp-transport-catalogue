 #include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <iostream>
#include <fstream>

using namespace transport_catalogue;
using namespace transport_router;
using namespace json_reader;
using namespace renderer;
using namespace std;

int main() {
    TransportCatalogue catalogue;
    MapRenderer map_renderer;
    RoutingSettings route_settings; // Initialize with default values or from a config file
    TransportRouter transport_router(catalogue, route_settings); // Pass the settings here
    RequestHandler request_handler(catalogue, map_renderer, transport_router);

    JsonReader reader(cin);
    reader.ApplyCommands(catalogue);
    reader.ApplyRenderSettingsCommands(map_renderer);
    reader.ApplyRouteSettingsCommands(transport_router);
    reader.PrintJson(request_handler, cout);

    return 0;
}