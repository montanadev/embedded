#include "renderer.h"
#include <string>
#include <vector>
#include <regex>
#include <sstream>

using namespace std;

const string WHITESPACE = " \n\r\t\f\v";

string debug(vector<vector<string> > data) {
    string output;
    for (auto outer: data) {
        string innerOutput;
        for (auto inner: outer) {
            innerOutput += inner + " ";
        }
        output += innerOutput + "\n";
    }
    return output;
}

string ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const std::string &s) {
    return rtrim(ltrim(s));
}

vector<string> split(const string &s, char delim) {
    vector<string> result;
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}


// splitToLines takes a string and splits it by newline character
vector<string> splitToLines(const string &data) {
    vector<string> lines;

    stringstream ss(data);
    string token;
    while (getline(ss, token, '\n')) {
        lines.push_back(token);
    }

    return lines;
}

string formatTableRow(string tableRow, vector<int> maxWidths) {
    string tableHeaderCellFormatted = "|";
    auto tableHeaderCells = split(tableRow, '|');
    for (int n = 1; n < tableHeaderCells.size(); n++) {
        auto tableHeaderCell = tableHeaderCells[n];

        string trimmed = trim(tableHeaderCell);
        int whitespaceNeeded = maxWidths[n - 1] - trimmed.length();
        string whitespace = "";
        for (int i = 0; i < whitespaceNeeded; i++) {
            whitespace += " ";
        }
        tableHeaderCellFormatted += ' ' + trimmed + whitespace + " |";
    }
    return tableHeaderCellFormatted;
}

string formatTableGutterRow(string tableRow, vector<int> maxWidths) {
    string tableGutterFormatted = "|";
    auto tableHeaderCells = split(tableRow, '|');
    for (int n = 1; n < tableHeaderCells.size(); n++) {
        string gutter = "";
        for (int i = 0; i < maxWidths[n - 1]; i++) {
            gutter += "-";
        }
        tableGutterFormatted += ' ' + gutter + " |";
    }
    return tableGutterFormatted;
}

vector<vector<string> > formatTableRows(vector<string> tableRows) {
    vector<vector<string> > formatted;
    vector<int> maxWidths;

    for (auto row: tableRows) {
        // TODO - the off-by-one is due to split, should it be changed to simplify?
        vector<string> cells = split(row, '|');
        for (int i = 1; i < cells.size(); i++) {
            if (maxWidths.size() <= i - 1) {
                maxWidths.push_back(0);
            }

            string cell = cells[i];
            if (cell.length() > maxWidths[i - 1]) {
                maxWidths[i -1] = trim(cell).length();
            }
        }
    }

    vector<string> boldOn;
    boldOn.push_back("boldOn");
    formatted.push_back(boldOn);

    vector<string> tableHeaderInstruction;
    tableHeaderInstruction.push_back("println");
    // push the first header row
    tableHeaderInstruction.push_back(formatTableRow(tableRows[0], maxWidths));
    formatted.push_back(tableHeaderInstruction);

    vector<string> tableGutterInstruction;
    tableGutterInstruction.push_back("println");
    tableGutterInstruction.push_back(formatTableGutterRow(tableRows[1], maxWidths));
    formatted.push_back(tableGutterInstruction);

    vector<string> boldOff;
    boldOff.push_back("boldOff");
    formatted.push_back(boldOff);

    for (int i = 2; i < tableRows.size(); i++) {
        vector<string> tableRowInstruction;
        tableRowInstruction.push_back("println");
        tableRowInstruction.push_back(formatTableRow(tableRows[i], maxWidths));
        formatted.push_back(tableRowInstruction);
    }

    return formatted;
}


vector<vector<string> > renderer(string data) {

    vector<vector<string> > result;
    

    // tableStart is used for detecting the first header row of a markdown table
    // and confirming it on the second hyphen row
    bool tableStart = false;
    bool tableConfirmed = false;
    vector<string> tableRows;
    
    vector<string> lines = splitToLines(data);
    for (auto line: lines) {
        smatch sm;

        regex header1pattern("^#(?!#)[ ]+(.*)");
        // ex: `# h1`
        regex_match(line, sm, header1pattern);
        if (sm.size()) {
            vector<string> size;
            size.push_back("setSize");
            size.push_back("L");
            result.push_back(size);

            vector<vector<string> > addtl = renderer(sm[1]);
            for (auto a: addtl) {
                result.push_back(a);
            }
            continue;
        }
        

        regex header2pattern("^##(?!#)[ ]+(.*)");
        // ex: `## h2`
        regex_match(line, sm, header2pattern);
        if (sm.size()) {
            vector<string> size;
            size.push_back("setSize");
            size.push_back("M");
            result.push_back(size);

            vector<vector<string> > addtl = renderer(sm[1]);
            for (auto a: addtl) {
                result.push_back(a);
            }
            continue;
        }
        

        regex header3pattern("^###(?!#)[ ]+(.*)");
        // ex: `### h3`
        regex_match(line, sm, header3pattern);
        if (sm.size()) {
            vector<string> size;
            size.push_back("setSize");
            size.push_back("S");
            result.push_back(size);

            vector<vector<string> > addtl = renderer(sm[1]);
            for (auto a: addtl) {
                result.push_back(a);
            }
            continue;
        }

        
        // TODO - this doesnt work for ***triple***, but not sure what the expected outcome would be
        regex boldPattern("\\*\\*(.*)\\*\\*");
        // ex: `**bold**`
        regex_match(line, sm, boldPattern);
        if (sm.size()) {
            vector<string> boldOn;
            boldOn.push_back("boldOn");
            result.push_back(boldOn);

            vector<vector<string> > addtl = renderer(sm[1]);
            for (auto a: addtl) {
                result.push_back(a);
            }

            vector<string> boldOff;
            boldOff.push_back("boldOff");
            result.push_back(boldOff);
            continue;
        }
        

        regex tableHeaderPattern("\\|(?:[^\\-])+\\|");
        // ex: `| cats | dogs |`
        regex_match(line, sm, tableHeaderPattern);
        if (sm.size()) {
            // it might be a table row
            if (tableConfirmed) {
                tableRows.push_back(line);
                continue;
            }
            // or it might be the start of a table
            tableStart = true;
        }

        regex tableHeaderSubPattern("\\|(?:[ \\-\\|]+)\\|");
        // ex: `| -- | -- |`
        regex_match(line, sm, tableHeaderSubPattern);
        if (sm.size() && tableStart) {
            // confirmed that we're in a table
            tableConfirmed = true;
            tableStart = false;
            tableRows.clear();
            // grab the previous println row, now that we've confirmed
            // we're in a table
            vector<string> lastInstruction = result[result.size() - 1];
            // grab the args from the lastInstruction
            tableRows.push_back(lastInstruction[1]);
            // drop the lastInstruction
            result.pop_back();
            tableRows.push_back(line);
            continue;
        }

        
        if (tableConfirmed) {
            // append the formatted rows to result
            auto formattedTableRows = formatTableRows(tableRows);
            for (auto r: formattedTableRows) {
                result.push_back(r);
            }
            tableRows.clear();
            tableConfirmed = false;

        }

        vector<string> println;
        println.push_back("println");
        println.push_back(line);
        result.push_back(println);
    }

    if (tableRows.size()) {
        auto formattedTableRows = formatTableRows(tableRows);
        for (auto r: formattedTableRows) {
            result.push_back(r);
        }
    }
    return result;
}
