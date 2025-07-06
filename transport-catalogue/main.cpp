 #include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"

#include <iostream>

using namespace transport_catalogue;
using namespace json_reader;
using namespace renderer;
using namespace std;

int main() {
    TransportCatalogue catalogue;
    MapRenderer map_renderer;
    RequestHandler request_handler(catalogue, map_renderer);

    JsonReader reader(cin);
    reader.ApplyCommands(catalogue);
    reader.ApplySettingsCommands(map_renderer);
    reader.PrintJson(request_handler, cout);
}