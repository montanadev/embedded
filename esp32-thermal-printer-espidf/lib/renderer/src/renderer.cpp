#include "renderer.h"
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <cstdarg>

using namespace std;

const string WHITESPACE = " \n\r\t\f\v";

template<typename T>
vector<string> createInstruction(const T& value)
{
    std::ostringstream oss;
    oss << value;
    vector<string> out;
    out.push_back(oss.str());
    return out;
}

template<typename T, typename ... Args >
vector<string> createInstruction(const T& value, const Args& ... args)
{
    vector<string> a = createInstruction(value);
    vector<string> b = createInstruction(args...);
    vector<string> c;
    // add the two lists together
    for (auto i : a) {
        c.push_back(i);
    }
    for (auto i : b) {
        c.push_back(i);
    }
    return c;
}

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

vector<int> calculateCellWidths(vector<string> tableRows) {
    vector<int> maxWidths;
    for (auto row: tableRows) {
        // TODO - the off-by-one is due to split, should it be changed to simplify?
        vector<string> cells = split(row, '|');
        for (int i = 1; i < cells.size(); i++) {
            if (maxWidths.size() < i) {
                maxWidths.push_back(0);
            }

            int cellWidth = trim(cells[i]).length();
            if (cellWidth > maxWidths[i - 1]) {
                maxWidths[i -1] = cellWidth;
            }
        }
    }
    return maxWidths;
}

vector<vector<string> > formatTableRows(vector<string> tableRows) {
    vector<vector<string> > formatted;
    auto maxWidths = calculateCellWidths(tableRows);

    // top row and gutter should be bold
    formatted.push_back(createInstruction("boldOn"));

    // push the first header row
    formatted.push_back(createInstruction("println", formatTableRow(tableRows[0], maxWidths)));
    // push the table gutter
    formatted.push_back(createInstruction("println", formatTableGutterRow(tableRows[1], maxWidths)));

    // turn off bold for start of table rows
    formatted.push_back(createInstruction("boldOff")); 

    for (int i = 2; i < tableRows.size(); i++) {
        formatted.push_back(createInstruction("println", formatTableRow(tableRows[i], maxWidths)));
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

        regex header1pattern("^[ ]{0,}#(?!#)[ ]{0,}(.*)");
        // ex: `# h1`
        regex_match(line, sm, header1pattern);
        if (sm.size()) {
            result.push_back(createInstruction("setSize", "L"));

            vector<vector<string> > addtl = renderer(sm[1]);
            for (auto a: addtl) {
                result.push_back(a);
            }

            // reset to normal text size
            result.push_back(createInstruction("setSize", "S"));
            continue;
        }
        

        regex header2pattern("^[ ]{0,}##(?!#)[ ]{0,}(.*)");
        // ex: `## h2`
        regex_match(line, sm, header2pattern);
        if (sm.size()) {
            result.push_back(createInstruction("setSize", "M"));

            vector<vector<string> > addtl = renderer(sm[1]);
            for (auto a: addtl) {
                result.push_back(a);
            }

            // reset to normal text size
            result.push_back(createInstruction("setSize", "S"));
            continue;
        }

        regex header3pattern("^[ ]{0,}[#]{3,}(?!#)[ ]+(.*)");
        // ex: `### h3` but can't be printed smaller, so strip the ###'s off and set as small text
        regex_match(line, sm, header3pattern);
        if (sm.size()) {
            result.push_back(createInstruction("setSize", "S"));

            vector<vector<string> > addtl = renderer(sm[1]);
            for (auto a: addtl) {
                result.push_back(a);
            }
            continue;
        }

        regex horizontalRule("^[ ]{0,}[-]{3,}[ ]{0,}$");
        regex_match(line, sm, horizontalRule);
         if (sm.size()) {
            result.push_back(createInstruction("setSize", "S"));
            result.push_back(createInstruction("boldOn"));
            result.push_back(createInstruction("println", "--------------------------------"));
            result.push_back(createInstruction("boldOff"));
            continue;
         }

        
        // TODO - this doesnt work for ***triple***, but not sure what the expected outcome would be
        regex boldPattern("\\*\\*(.*)\\*\\*");
        // ex: `**bold**`
        regex_match(line, sm, boldPattern);
        if (sm.size()) {
            result.push_back(createInstruction("boldOn"));

            vector<vector<string> > addtl = renderer(sm[1]);
            for (auto a: addtl) {
                result.push_back(a);
            }

            result.push_back(createInstruction("boldOff"));
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
        result.push_back(createInstruction("println", line));
    }

    if (tableRows.size()) {
        auto formattedTableRows = formatTableRows(tableRows);
        for (auto r: formattedTableRows) {
            result.push_back(r);
        }
    }
    return result;
}
