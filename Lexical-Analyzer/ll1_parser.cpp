#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <fstream>
using namespace std;

class Grammar{
public:
    map<char,vector<string>> prod_rules;
    set<char> non_terminals;
    set<char> terminals;
    char startSymbol;
    map<pair<char,char>,string> parsing_table;
    map<char,set<char>> first_set, follow_set;
    string input_string;

    void initialize(){
        startSymbol = '!';
        vector<string> lines;
        string line;

        ifstream inputFile("production1.txt");
        if (!inputFile.is_open()) {
            cerr << "Could not open the file." << endl;
            exit(0);
        }

        while (getline(inputFile, line)){
            string newLine = "";
            for(auto character: line){
                if(character != ' ')
                    newLine += character;
            }
            if(newLine.length() != 0)
                lines.push_back(newLine);
        }

        startSymbol = lines[0][0];
        for (int i = 1; i < lines.size() - 1; i++){
            auto line = lines[i];
            int index = 0, length = line.length();
            non_terminals.insert(line[index++]);
            index += 2;
            prod_rules[line[index - 3]].push_back(line.substr(index));
            while(index < length){
                if(!(line[index] >= 'A' && line[index] <= 'Z'))
                    terminals.insert(line[index++]);
                else
                    non_terminals.insert(line[index++]);
            }
        }

        input_string = lines[lines.size() - 1];

        cout << "Start Symbol: " << startSymbol << endl;
        cout << "Non-terminals: ";
        for(auto nonTerminal : non_terminals)
            cout << nonTerminal << " ";
        cout << endl;
        cout << "Terminals: ";
        for(auto terminal : terminals)
            cout << terminal << " ";
        cout << endl;
        cout << "Production Rules: " << endl;
        for(auto rule : prod_rules){
            cout << rule.first << " -> ";
            for(auto production : rule.second){
                cout << production;
                if(production != rule.second.back())
                    cout << " | ";
            }
            cout << endl;
        }
        inputFile.close();
    }

    map<pair<char,char>,set<char>> visited;

    set<char> findFirst(char variable, char callingVariable){
        auto key = make_pair(callingVariable, variable);
        if(!(variable >= 'A' && variable <= 'Z'))
            return {variable};
        if(visited.size() && visited.find(key) != visited.end()){
            return visited[key];
        }

        visited[key] = {};

        set<char> temp;
        for(auto production : prod_rules[variable]){
            if(production == "@")
                temp.insert('@'), visited[key].insert('@');
        }
        for(auto production : prod_rules[variable]){
            if(production == "@")
                continue;
            if(!(production[0] >= 'A' && production[0] <= 'Z')){
                temp.insert(production[0]);
                visited[key].insert(production[0]);
            }
            else {
                bool isEpsilon = false;
                for(auto character : production){
                    auto tempSet = findFirst(character, variable);
                    isEpsilon = false;
                    for(auto firstSymbol : tempSet){
                        if(firstSymbol == '@')
                            isEpsilon = true;
                        else {
                            temp.insert(firstSymbol);
                            visited[key].insert(firstSymbol);
                        }
                    }
                    if(!isEpsilon)
                        break;
                }
                if(isEpsilon){
                    temp.insert('@');
                    visited[key].insert('@');
                }
            }
        }
        return temp;
    }

    void calculateFirst(){
        for(auto variable : non_terminals){
            visited.clear();
            first_set[variable] = findFirst(variable, '!');
        }

        cout << "\nFirst Set: " << endl;
        for(auto it : first_set){
            cout << it.first << " -> { ";
            for(auto symbol : it.second){
                cout << symbol;
                if(symbol != (*it.second.rbegin()))
                    cout << ", ";
            }
            cout << " }" << endl;
        }
    }

    set<char> findfirst_of_string(string str){
        set<char> resultSet;
        int count = 0;
        for(auto character : str){
            if(!(character >= 'A' && character <= 'Z')){
                resultSet.insert(character);
                break;
            }
            else {
                bool flag = false;
                for(auto symbol : first_set[character]){
                    if(symbol == '@')
                        flag = true;
                    else
                        resultSet.insert(symbol);
                }
                if(!flag)
                    break;
            }
            count++;
        }
        if(count == str.length())
            resultSet.insert('@');
        return resultSet;
    }

    set<char> visited1;

    set<char> findFollow(char variable){
        if(!(variable >= 'A' && variable <= 'Z'))
            return {variable};
        if(visited1.size() && visited1.find(variable) != visited1.end()){
            return {};
        }
        visited1.insert(variable);
        set<char> resultSet;
        if(variable == startSymbol)
            resultSet.insert('$');
        for(auto rule : prod_rules){
            for(auto production : rule.second){
                int index, length = production.length();
                bool checkFollow = false;
                for(index = 0; index < length - 1; index++){
                    if(production[index] == variable){
                        auto nextFirst = findfirst_of_string(production.substr(index + 1));
                        for(auto symbol : nextFirst){
                            if(symbol != '@')
                                resultSet.insert(symbol);
                            else
                                checkFollow = true;
                        }
                        if(checkFollow){
                            auto nextFollow = findFollow(rule.first);
                            for(auto symbol : nextFollow){
                                if(symbol != '@')
                                    resultSet.insert(symbol);
                            }
                            break;
                        }
                    }
                }
                if(production[length - 1] == variable){
                    auto nextFollow = findFollow(rule.first);
                    for(auto symbol : nextFollow){
                        if(symbol != '@')
                            resultSet.insert(symbol);
                    }
                }
            }
        }
        auto itr = visited1.find(variable);
        if(itr != visited1.end())
            visited1.erase(itr);
        return resultSet;
    }

    void calculateFollow(){
        follow_set[startSymbol].insert('$');
        for(auto variable : non_terminals){
            visited.clear();
            auto resultSet = findFollow(variable);
            for(auto symbol : resultSet)
                follow_set[variable].insert(symbol);
        }

        cout << "\nFollow Set: " << endl;
        for(auto it : follow_set){
            cout << it.first << " -> { ";
            for(auto symbol : it.second){
                cout << symbol;
                if(symbol != (*it.second.rbegin()))
                    cout << ", ";
            }
            cout << " }" << endl;
        }
    }

    void createParseTable(){
        bool isll1 = false;
        for(auto rule : prod_rules){
            for(auto production : rule.second){
                auto first_of_string = findfirst_of_string(production);
                bool isEpsilon = false;
                for(auto symbol : first_of_string){
                    if(symbol != '@'){
                        auto key = make_pair(rule.first, symbol);
                        auto it = parsing_table.find(key);
                        if(it != parsing_table.end()){
                            if(parsing_table[key] == production)
                                continue;
                            isll1 = true;
                            break;
                        }
                        parsing_table[key] = production;
                    }
                    else
                        isEpsilon = true;
                }
                if(isEpsilon){
                    for(auto symbol : follow_set[rule.first]){
                        auto key = make_pair(rule.first, symbol);
                        auto it = parsing_table.find(key);
                        if(it != parsing_table.end()){
                            if(parsing_table[key] == production)
                                continue;
                            isll1 = true;
                            break;
                        }
                        parsing_table[key] = production;
                    }
                }
                if(isll1)
                    break;
            }
            if(isll1)
                break;
        }
        if(isll1){
            cout << "The given grammar is not LL(1)." << endl;
            exit(1);
        }

        cout << "\nParsing Table: " << endl;
        for(auto rule : parsing_table){
            cout << "Variable: " << rule.first.first << " Terminal: " << rule.first.second << " Production: " << rule.second << endl;
        }
    }

    void parseString(){
        cout << "\nParsing Started......." << endl;
        input_string += "$";
        string stack = "$";
        stack += char(startSymbol);
        int ip = 0, length = input_string.length();
        cout << "\tSTACK\t\t\tINPUT\t\t\tACTION" << endl;
        for(ip = 0; ip < length; ip++){
            char ch = input_string[ip];
            char sch = stack.back();
            cout << "\t" << stack << "\t\t\t" << input_string.substr(ip) << "\t\t\t";
            if(ch == '$'){
                cout << "Done" << endl;
                if(ip != length - 1)
                    cout << "\nResult: Invalid Sentence" << endl;
                else
                    cout << "\nResult: Accepted" << endl;
                exit(1);
            }
            auto it = terminals.find(ch);
            if(it == terminals.end()){
                cout << "Done" << endl;
                cout << "\nResult: Invalid Sentence" << endl;
                exit(1);
            }
            if(ch == sch){
                cout << "Substitute" << endl;
                stack.pop_back();
            }
            else if(!(sch >= 'A' && sch <= 'Z')){
                cout << "Done" << endl;
                cout << "\nResult: Invalid Sentence" << endl;
                exit(1);
            }
            else{
                cout << "Next Symbol: ";
                auto production = parsing_table[{sch, ch}];
                cout << sch << " -> " << production << endl;
                stack.pop_back();
                stack.append(production.rbegin(), production.rend());
                ip--;
            }
        }
    }
};

int main(){
    Grammar* g = new Grammar();
    g->initialize();
    g->calculateFirst();
    g->calculateFollow();
    g->createParseTable();
    g->parseString();
    
    return 0;
}

