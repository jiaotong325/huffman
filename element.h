#ifndef ELEMENT_H
#define ELEMENT_H
#include <QString>

using namespace std;

class Symbol {
    QChar symbol;//字符
    int numberSymbols;//次数（权）
public:
    Symbol(QChar _symbol, int _numSym) : symbol(_symbol), numberSymbols(_numSym) {}
    Symbol() : symbol(0), numberSymbols(0) {}
    void setSymbol(const Symbol &_symbol) {
        symbol = _symbol.symbol;
        numberSymbols = _symbol.numberSymbols;
    }


    void increaseNumberSymbol(int num=1) {
        numberSymbols+=num;
    }
    void decreaseNumberSymbol() {
        --numberSymbols;
    }
    void setSymbolNumber(const int &_symbolNumber) {
        numberSymbols = _symbolNumber;
    }

    const QChar &getSymbol() {
        return symbol;
    }
    int getSymbolNumber() {
        return numberSymbols;
    }

    //重载<运算符，用于直接比较numberSymbols的大小
    friend bool operator<(const Symbol &sym1, const Symbol &sym2) {
        return  sym1.numberSymbols < sym2.numberSymbols;
    }
};


//二叉树结构
class Element{
public:
    Element * parentNode = NULL;
    Element * leftNode = NULL;
    Element * rifgtNode = NULL;
    Symbol symbol;//字符及权
    int numberSymbolsNode;//总权
    QString huffmanCode = "";

    Element(Element *_leftNode, Element *_rightNode, const int _numSymNode = 0)
        : leftNode(_leftNode), rifgtNode(_rightNode), numberSymbolsNode(_numSymNode) {}
    Element(const Symbol _sym, int _numberSymNoe) : symbol(_sym), numberSymbolsNode(_numberSymNoe) {}
    Element(const Symbol _sym) : symbol(_sym) {}
    Element();
};
#endif // ELEMENT_H
