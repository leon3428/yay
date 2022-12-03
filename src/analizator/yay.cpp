#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <map>

#include "../Grammar.hpp"
#include "../TableBuilder.hpp"

struct Node {
    Node *par;
    std::string symbol, value;
    int line, state;
    std::vector<Node*> child;
    Node() { par = NULL; }
    Node(int _state) { state = _state; par = NULL; symbol = value = ""; line = -1; } 
    Node(std::string symbol) : symbol(symbol) {};
    Node(int _state, std::string _symbol, int _line, std::string _value) {state = _state; symbol = _symbol; line = _line; value = _value; par = NULL; }

    void addChild(Node *newChild) {
        child.push_back(newChild);
    }
};

void printTable(std::vector<std::vector<TableElement> > &table, std::vector<std::string> &tableTop) {
    
    for(auto it : tableTop)
        std::cerr << it << " ";
    std::cerr << "\n";

    int i = 0;

    for(auto it : table){
        std::cerr << i++ << " ";
        for(auto it1 : it)
            std::cerr << it1.action << " ";
        std::cerr << "\n";
    }
    std::cerr << "\n\n";
}

void print(Node *node, int depth = 0) {
    for(int i = 0; i < depth; ++i)
        std::cout << " ";

    if(node -> symbol[0] == '<'){
        std::cout << node -> symbol << "\n";
        for(auto it = node -> child.rbegin(); it != node -> child.rend(); ++it)
            print(*it, depth + 1);
    } else{
        if(node->symbol != "$")
            std::cout << node -> symbol << " " << node -> line << " " << node -> value << "\n";
        else
            std::cout << node -> symbol << "\n";
    }
}


std::set<std::string> sync;
std::vector<std::string> tableTop;
std::vector<std::vector<TableElement> > table;
std::map<std::string, int> mapTo;
int startState, terminalSize;

void parseInput() {
	std::ifstream inp("src/analizator/in.txt");

    int syncSize, tableN, tableM;
    inp >> terminalSize;
    inp >> syncSize;

    for(int i = 0; i < syncSize; ++i){
        std::string s;
        inp >> s;
        sync.insert(s);
    }

    inp >> tableN >> tableM;

    tableTop.resize(tableM);
    for(int i = 0; i < tableM; ++i){
        inp >> tableTop[i];
        mapTo[tableTop[i]] = i;
    }

    table.resize(tableN);
    for(int i = 0; i < tableN; ++i) {
        table[i].resize(tableM);
        for(int j = 0; j < tableM; ++j){
            char a;
            int b, c, d, e;
            inp >> a >> b >> c >> d >> e;
            table[i][j] = TableElement(a, b, c, d, e);
        }
    }
    inp >> startState;
    inp.close();
}

std::stack<Node*> st;
bool errorState;

void simulate(std::string symbol, int line, std::string text) {
	while(!st.empty()){
		Node *top = st.top();
		int i = top -> state;
		int j = mapTo[symbol];
		TableElement nxt = table[i][j];

		if(errorState){
			if(sync.find(symbol) == sync.end()) // go to next symbol
				break;
			while(!st.empty() && nxt.action == 'E') {
				st.pop();
				top = st.top();
				i = top -> state;
				j = mapTo[symbol];
				nxt = table[i][j];
			}
			errorState = false;
		}

		if(nxt.action == 'S'){
			std::cerr << "SHIFT " << i << " " << symbol << " to " << nxt.shiftTo << "\n"; 
			st.push(new Node(nxt.shiftTo, symbol, line, text));
			return;
		} else if(nxt.action == 'R'){
			std::cerr << "REDUCE ";
			std::cerr << tableTop[Grammar::decodeNonTerminalId(nxt.leftSide) + terminalSize] << " by " << nxt.elementCnt << "\n";
			Node *tmp = new Node(-1);
				if(nxt.elementCnt != 0){
					for(int k = 0; k < nxt.elementCnt; ++k) {
						tmp -> addChild(st.top());
						st.pop();
					}
				} else {
					tmp -> addChild(new Node("$"));
				}

			tmp -> symbol = tableTop[Grammar::decodeNonTerminalId(nxt.leftSide) + terminalSize];
			
			top = st.top();
			i = top -> state;
			j = mapTo[tmp -> symbol];
			nxt = table[i][j];

			if(nxt.action == 'S'){
				tmp -> state = nxt.shiftTo;
				st.push(tmp);
			}
			else {
				std::cerr << "Error\n";
				errorState = true;
				continue;
			}
			continue;
		} else if(nxt.action == 'E'){
			std::cerr << "Error\n";
			errorState = true;
			continue;
		} else {
			std::cerr << "KRAJ " << st.top() -> symbol << "\n";
			print(st.top());
			exit(0);
		}
	}
}

int main(int argc, char const *argv[])
{
	parseInput();

    std::string symbol, text;
    int line;
    
    st.push(new Node(startState));
    
    while(std::cin >> symbol >> line >> text){
        if(mapTo.find(symbol) == mapTo.end())
            symbol = "<" + symbol + ">";

		simulate(symbol, line, text);
    }

	simulate("$", line, "");

    return 0;
}
