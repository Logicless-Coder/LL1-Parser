#include <bits/stdc++.h>
#include <boost/algorithm/string.hpp>

using namespace std;

#define MAX_WIDTH 16
#define NULL_SYMBOL "^"
#define DOLLAR_SYMBOL "$"

#define MAX_DEPTH 5
#define endl "\n"

#define all(x) (x).begin(), (x).end()

int WIDTH = 12;

class Symbol {
private:
  string m_value;
  bool m_isTerminal;

public:
  Symbol() { m_value = "!", m_isTerminal = false; }

  Symbol(string _value, bool _isTerminal) {
    m_value = _value;
    m_isTerminal = _isTerminal;
  }

  string getValue() { return m_value; }

  bool isTerminal() { return m_isTerminal; }

  bool isNull() { return m_value == NULL_SYMBOL && m_isTerminal; }

  void print() {
    if (m_isTerminal)
      cout << "\033[91m" << m_value << "\033[0m";
    else
      cout << "\033[92m" << m_value << "\033[0m";
  }

  bool operator==(const Symbol &other) const {
    return m_isTerminal == other.m_isTerminal && m_value == other.m_value;
  }

  bool operator<(const Symbol &other) const {
    if (m_isTerminal == other.m_isTerminal)
      return m_value < other.m_value;
    return m_isTerminal;
  }
};

class Sequence {
public:
  vector<Symbol> m_seq;

  Sequence() { m_seq = {}; }

  Sequence(vector<Symbol> _seq) { m_seq = _seq; }

  void print() {
    for (auto symbol : m_seq) {
      symbol.print();
      cout << " ";
    }
  }

  bool isNull() { return m_seq.size() == 1 && m_seq[0].isNull(); }

  bool operator==(const Sequence &other) const {
    int n = m_seq.size(), m = other.m_seq.size();
    if (n != m)
      return false;

    for (int i = 0; i < n; i++) {
      if (m_seq[i] != other.m_seq[i])
        return false;
    }

    return true;
  }
};

class Production {
public:
  Symbol m_left;
  Sequence m_right;

  Production() {
    m_left = Symbol();
    m_right = Sequence();
  }

  Production(Symbol _left, Sequence _right) {
    m_left = _left;
    m_right = _right;
  }

  void print() {
    m_left.print();
    cout << "\033[93m -> \033[0m";
    m_right.print();
  }

  bool operator==(const Production &other) const {
    return m_left == other.m_left && m_right == other.m_right;
  }

  bool operator<(const Production &other) const { return true; }
};

struct comparator {
  bool operator()(const pair<Symbol, Symbol> &a,
                  const pair<Symbol, Symbol> &b) const {
    if (a.first == b.first)
      return a.second < b.second;
    return a.first < b.first;
  }
};

class Grammar {
private:
  Symbol m_start;
  vector<Symbol> m_terminals, m_non_terminals;
  vector<Production> m_productions;
  map<Symbol, int> depths;
  map<Symbol, set<Symbol>> best;

  map<pair<Symbol, Symbol>, Production, comparator> parsingTable;

  bool symbolType(string symbol) {
    if (m_start.getValue() == symbol)
      return false;

    for (auto non_term : m_non_terminals)
      if (non_term.getValue() == symbol)
        return false;

    for (auto term : m_terminals)
      if (term.getValue() == symbol)
        return true;

    cerr << "Error symbol: " << symbol << endl;
    assert(false); // this should never be called
    return true;
  }

public:
  Grammar() {
    m_start = Symbol();
    m_terminals = m_non_terminals = {};
    m_productions = {};
  }

  Grammar(string fileName) {
    ifstream fin(fileName, ios::binary);
    // Adding start symbol
    {
      string startSymbol;
      getline(fin, startSymbol);
      m_start = Symbol(startSymbol, false);
    }

    // Adding terminals
    {
      string terminalSymbols;
      getline(fin, terminalSymbols);

      vector<string> terminals;
      boost::split(terminals, terminalSymbols, [](char c) { return c == ' '; });

      for (auto symbol : terminals) {
        m_terminals.push_back(Symbol(symbol, true));
      }
    }

    // Adding non-terminals
    {
      string nonTerminalSymbols;
      getline(fin, nonTerminalSymbols);

      vector<string> nonTerminals;
      boost::split(nonTerminals, nonTerminalSymbols,
                   [](char c) { return c == ' '; });

      for (auto symbol : nonTerminals) {
        m_non_terminals.push_back(Symbol(symbol, false));
      }
    }

    // Adding productions
    {
      string productionSymbols;
      while (getline(fin, productionSymbols)) {
        vector<string> productions;
        boost::split(productions, productionSymbols,
                     [](char c) { return c == ' '; });

        assert(!symbolType(productions[0])); // check left is a non-terminal
        Symbol productionLeft = Symbol(productions[0], false);
        assert(productions[1] == "->");

        vector<Symbol> productionRight;
        int rightSize = 0;
        for (int i = 2; i < productions.size(); i++) {
          bool type = symbolType(productions[i]);
          productionRight.push_back(Symbol(productions[i], type));
          rightSize += productions[i].size();
        }

        WIDTH = max(WIDTH, (int)(productionLeft.getValue().size() + rightSize));

        m_productions.push_back(
            Production(productionLeft, Sequence(productionRight)));
      }
    }
  }

  void print() {
    // cout << setw(WIDTH);
    // cout << left << "Start: ";
    // cout << setw(WIDTH);
    m_start.print();
    cout << endl;

    // cout << setw(WIDTH);
    // cout << left << "Terminals: ";
    // cout << setw(WIDTH);
    for (auto term : m_terminals) {
      term.print();
      cout << " ";
    }
    cout << endl;

    // cout << setw(WIDTH);
    // cout << left << "Non-Terminals: ";
    // cout << setw(WIDTH);
    for (auto non_term : m_non_terminals) {
      non_term.print();
      cout << " ";
    }
    cout << endl;

    // cout << setw(WIDTH);
    // cout << left << "Productions: " << endl;
    for (auto prod : m_productions) {
      // cout << setw(WIDTH * 2);
      prod.print();
      cout << endl;
    }
    cout << endl;
  }

  set<Symbol> first(Symbol x) {
    if (x.isTerminal())
      return {x};

    set<Symbol> s;
    for (auto prods : m_productions) {
      if (prods.m_left != x)
        continue;

      if (prods.m_right.isNull())
        s.insert(Symbol(NULL_SYMBOL, true));
      else {
        set<Symbol> result = first(prods.m_right);
        s.insert(all(result));
      }
    }

    return s;
  }

  set<Symbol> first(Sequence x) {
    int length = x.m_seq.size();
    if (length < 1)
      return {Symbol(NULL_SYMBOL, true)};

    set<Symbol> result = first(x.m_seq[0]);
    if (length == 1 || result.find(Symbol(NULL_SYMBOL, true)) == result.end())
      return result;

    result.erase(Symbol(NULL_SYMBOL, true));

    vector<Symbol> remaining(x.m_seq.begin() + 1, x.m_seq.end());
    set<Symbol> next = first(Sequence(remaining));

    result.insert(all(next));
    return result;
  }

  set<Symbol> follow(Symbol x) {
    if (depths[x] > MAX_DEPTH)
      return best[x];
    depths[x]++;

    // cout << "Follow(";
    // x.print();
    // cout << ")\n";

    set<Symbol> result;
    if (x == m_start)
      result.insert(Symbol(DOLLAR_SYMBOL, true));

    for (auto prods : m_productions) {
      int len = prods.m_right.m_seq.size();

      for (int i = 0; i < len; i++) {
        auto y = prods.m_right.m_seq[i];
        if (x == y) {
          vector<Symbol> rem(prods.m_right.m_seq.begin() + i + 1,
                             prods.m_right.m_seq.end());
          auto s = first(Sequence(rem));

          if (s.find(Symbol(NULL_SYMBOL, true)) != s.end() || i == len - 1) {
            auto ss = follow(prods.m_left);
            result.insert(all(ss));
          }

          s.erase(Symbol(NULL_SYMBOL, true));
          result.insert(all(s));

          // maybe stop when no new symbols are being added
          // bool notNew = includes(all(best), all(result));

          best[x].insert(all(result));
        }
      }
    }

    return result;
  }

  void printFirsts() {
    cout << "The first sets of the non-terminals: " << endl;
    for (auto x : m_non_terminals) {
      auto y = first(x);
      x.print();
      cout << ": ";
      for (auto z : y) {
        z.print();
        cout << " ";
      }
      cout << endl;
    }
  }

  void printFollows() {
    cout << "The follow sets of the non-terminals: " << endl;
    for (auto x : m_non_terminals) {
      auto y = follow(x);
      x.print();
      cout << ": ";
      for (auto z : y) {
        z.print();
        cout << " ";
      }
      cout << endl;
    }
  }

  void generateParsingTable() {
    m_terminals.push_back(Symbol(DOLLAR_SYMBOL, true));

    for (auto prod : m_productions) {
      auto s = first(prod.m_right.m_seq);

      for (auto x : s) {
        if (x.isNull()) {
          auto y = follow(prod.m_left);
          for (auto z : y) {
            bool already =
                parsingTable.find({prod.m_left, z}) != parsingTable.end() &&
                parsingTable[{prod.m_left, z}] != prod;
            if (already) {
              cerr << "The given grammar is not LL-1 as there is a conflict "
                      "when generating the parsing table."
                   << endl;
              exit(1);
            }

            parsingTable[{prod.m_left, z}] = prod;
          }
        } else {
          bool already =
              parsingTable.find({prod.m_left, x}) != parsingTable.end() &&
              parsingTable[{prod.m_left, x}] != prod;
          if (already) {
            cerr << "The given grammar is not LL-1 as there is a conflict when "
                    "generating the parsing table."
                 << endl;
            exit(1);
          }

          parsingTable[{prod.m_left, x}] = prod;
        }
      }
    }
  }

  void printParsingTable() {
    for (auto x : m_non_terminals) {
      for (auto y : m_terminals) {
        if (parsingTable.find({x, y}) != parsingTable.end()) {
          x.print();
          cout << " , ";
          y.print();
          int spaces =
              MAX_WIDTH - x.getValue().size() - y.getValue().size() - 3;
          for (int i = 0; i < (spaces + 1) / 2; i++)
            cout << " ";
          cout << " : ";
          for (int i = 0; i < MAX_WIDTH / 2; i++)
            cout << " ";
          parsingTable[{x, y}].print();
          cout << endl;
        }
      }
      cout << endl;
    }
  }

  void printStack(stack<Symbol> stack) {
    cout << "Stack  : ";
    while (!stack.empty()) {
      Symbol x = stack.top();
      x.print();
      cout << " ";
      stack.pop();
    }
    cout << endl;
  }

  void printInput(Sequence s, int i) {
    cout << "Input  : ";
    for (; i < s.m_seq.size(); i++) {
      s.m_seq[i].print();
      cout << " ";
    }
    cout << endl;
  }

  bool checkString(string inputString) {
    cout << "Checking string: " << inputString << endl;
    vector<string> inputWords;
    boost::split(inputWords, inputString, [](char c) { return c == ' '; });

    vector<Symbol> inputSymbols;
    for (auto symbol : inputWords) {
      Symbol s = Symbol(symbol, symbolType(symbol));
      inputSymbols.push_back(s);
    }
    inputSymbols.push_back(Symbol(DOLLAR_SYMBOL, true));

    Sequence input = Sequence(inputSymbols);
    int inputPointer = 0;

    stack<Symbol> stack;
    stack.push(Symbol(DOLLAR_SYMBOL, true));
    stack.push(m_start);
    while (stack.top() != Symbol(DOLLAR_SYMBOL, true) &&
           inputPointer < input.m_seq.size()) {
      if (input.m_seq[inputPointer] == stack.top()) {
        printStack(stack);
        printInput(input, inputPointer);
        cout << "\033[92m"
             << "Match  : "
             << "\033[0m";
        input.m_seq[inputPointer].print();
        cout << " ";
        stack.top().print();
        stack.pop();
        cout << endl;
        cout << endl;
        inputPointer++;
        continue;
      }

      printStack(stack);
      printInput(input, inputPointer);

      if (parsingTable.find({stack.top(), input.m_seq[inputPointer]}) ==
          parsingTable.end()) {
        cout << "Cannot be reduced!" << endl;
        return false;
      }

      cout << "\033[92m"
           << "Reduce : "
           << "\033[0m";
      stack.top().print();
      cout << " ";
      input.m_seq[inputPointer].print();
      cout << " using ";

      auto prod = parsingTable[{stack.top(), input.m_seq[inputPointer]}];
      stack.pop();
      prod.print();
      cout << endl;
      cout << endl;

      if (prod.m_right.isNull())
        continue;

      for (int i = prod.m_right.m_seq.size() - 1; i >= 0; i--) {
        stack.push(prod.m_right.m_seq[i]);
      }
    }

    if (stack.top() == Symbol(DOLLAR_SYMBOL, true) &&
        inputPointer == input.m_seq.size() - 1 &&
        input.m_seq[inputPointer] == Symbol(DOLLAR_SYMBOL, true))
      return true;

    return false;
  }
};

int main(int argc, char *args[]) {
  if (argc < 2)
    cout << "Usage: <executable> <grammar-file>\n";

  Grammar g = Grammar(args[1]);
  cout << "The given grammar is: " << endl;
  g.print();
  g.generateParsingTable();

  string inputString;
  cout << "Enter the input string where each token (terminal) is space "
          "separated: ";
  getline(cin, inputString);

  // g.printFirsts();
  // g.printFollows();
  g.printParsingTable();

  bool accepted = g.checkString(inputString);
  if (accepted)
    cout << "\033[92m"
         << "String accepted."
         << "\033[0m" << endl;
  else
    cout << "\033[91m"
         << "String rejected."
         << "\033[0m" << endl;

  return 0;
}
