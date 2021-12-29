#include <bits/stdc++.h>
using namespace std;

#define endl '\n'

class Symbol {
public:
  bool isTerminal;
  string name;

  Symbol() {
    isTerminal = false;
    name = "#";
  }

  Symbol(string name, bool isTerminal) {
    this->name = name;
    this->isTerminal = isTerminal;
  }

  bool isNull() { return name == "^"; }

  bool operator<(const Symbol &rhs) const { return name < rhs.name; }
};

class Sequence {
public:
  vector<Symbol> seq;

  Sequence() { seq = vector<Symbol>(); }

  Sequence(vector<Symbol> seq) { this->seq = seq; }

  void print() {
    for (int i = 0; i < seq.size(); i++) {
      cout << seq[i].name << " ";
    }
  }
};

class Production {
public:
  Symbol name;
  vector<Sequence> right;

  Production() {
    name = Symbol();
    right = vector<Sequence>();
  }

  Production(Symbol name, vector<Sequence> right) {
    this->name = name;
    this->right = right;
  }

  void print() {
    cout << name.name << " -> ";
    for (int i = 0; i < right.size(); i++) {
      right[i].print();
      if (i != right.size() - 1)
        cout << " | ";
    }
  }
};

class Grammar {
public:
  map<Symbol, vector<Sequence>> productions;
  vector<Symbol> terminals;
  vector<Symbol> nonTerminals;
  Symbol start;

  Grammar() {
    productions = map<Symbol, vector<Sequence>>();
    terminals = vector<Symbol>();
    nonTerminals = vector<Symbol>();
    start = Symbol();
  }

  Grammar(map<Symbol, vector<Sequence>> productions, vector<Symbol> terminals,
          vector<Symbol> nonTerminals, Symbol start) {
    this->productions = productions;
    this->terminals = terminals;
    this->nonTerminals = nonTerminals;
    this->start = start;
  }

  void print() {
    cout << "~~~~~~~~~~ Grammar ~~~~~~~~~~" << endl;
    cout << "Start: " << start.name << endl;
    cout << "Terminals: ";
    for (int i = 0; i < terminals.size(); i++) {
      cout << terminals[i].name;
      if (i != terminals.size() - 1)
        cout << ", ";
    }
    cout << endl;
    cout << "Non-Terminals: ";
    for (int i = 0; i < nonTerminals.size(); i++) {
      cout << nonTerminals[i].name;
      if (i != nonTerminals.size() - 1)
        cout << ", ";
    }
    cout << endl;
    cout << "Productions:" << endl;
    for (auto production : productions) {
      cout << production.first.name << " -> ";
      for (int i = 0; i < production.second.size(); i++) {
        production.second[i].print();
        if (i != production.second.size() - 1)
          cout << "| ";
      }
      cout << endl;
    }
  }

  set<Symbol> first(Sequence s) {
    set<Symbol> firstSet;
    if (s.seq[0].isTerminal) {
      firstSet.insert(s.seq[0]);
      return firstSet;
    }
    for (auto production : productions[s.seq[0]]) {
    }
    return firstSet;
  }
};

vector<string> splitString(string s, string delimiter) {
  vector<string> splitted;
  size_t pos = 0;
  string token;
  while ((pos = s.find(delimiter)) != string::npos) {
    token = s.substr(0, pos);
    splitted.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  splitted.push_back(s);
  return splitted;
}

Grammar parseGrammar() {
  map<Symbol, vector<Sequence>> productions;
  vector<Symbol> terminals;
  vector<Symbol> nonTerminals;
  Symbol start;

  string line;
  getline(cin, line);
  start = Symbol(line, false);

  getline(cin, line);
  vector<string> splitted = splitString(line, " ");
  for (int i = 0; i < splitted.size(); i++) {
    terminals.push_back(Symbol(splitted[i], true));
  }

  getline(cin, line);
  splitted = splitString(line, " ");
  for (int i = 0; i < splitted.size(); i++) {
    nonTerminals.push_back(Symbol(splitted[i], false));
  }

  int cnt = 0;
  while (getline(cin, line)) {
    splitted = splitString(line, " -> ");
    Symbol name = Symbol(splitted[0], false);
    vector<Sequence> right;
    vector<string> rightStrings = splitString(splitted[1], " | ");
    for (int i = 0; i < rightStrings.size(); i++) {
      vector<Symbol> seq;
      vector<string> seqStrings = splitString(rightStrings[i], " ");
      for (int j = 0; j < seqStrings.size(); j++) {
        seq.push_back(Symbol(seqStrings[j], false));
      }
      right.push_back(Sequence(seq));
    }
    productions[name] = right;
    cnt += 1;
    if (cnt > 10)
      break;
  }

  return Grammar(productions, terminals, nonTerminals, start);
}

int main() {
  Grammar grammar = parseGrammar();
  grammar.print();

  return 0;
}