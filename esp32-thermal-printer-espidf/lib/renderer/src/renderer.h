#ifndef ESP32_THERMAL_PRINTER_RENDERER_H
#define ESP32_THERMAL_PRINTER_RENDERER_H

#include <string>
#include <vector>

using namespace std;

string debug(vector<vector<string> > data);

vector<vector<string> > renderer(string);

vector<string> splitToLines(const string &data);

#endif //ESP32_THERMAL_PRINTER_RENDERER_H
