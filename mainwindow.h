#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFileDialog>
#include "element.h"
#include <QVector>
#include <math.h>

#include<explain.h>

namespace Ui {
class MainWindow;
}

enum TypeNode { Root, Left, Right };

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_Code_clicked();
    void on_pushButton_Clear_clicked();
    void on_pushButton_SaveImage_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_Button_encode_clicked();

    void on_Button_decode_clicked();

    void on_action_exit_triggered();


    void on_action_triggered();

    void on_action_SaveImage_triggered();

private:
    //在窗口中绘制二叉树
    void drawNodes(int xPosition, int Position, Element *node, TypeNode typeNode , int level,  int levels);

    //计算二叉树的层数
    int countLevel(Element *node, int numberSymbols);

    void setCodeHuffmanaToElement(QChar _char, QString codeHuff);

    //返回用于在二叉树绘图中的正方形中插入符号数的 x 位置
    int getXPosition(int n);

    //从输入中提取字符串，并将其排序保存到“elements”向量
    void shortSymbolsToVector(QString dataiInput);

    void TableSymbolsToInput(QString& dataiInput);


    //将数据导入表格
    void setSymbolsToTable();


    //在“rootElement”中构建二叉树
    void setRoot();

    //将最终数据插入程序窗口的字段
    void setDataOut(QString dataInput);

    Ui::MainWindow *ui;

    explain open_explain;


    QGraphicsScene * scene;

    QVector<Symbol> symbols;
    QVector<Element*> elements;
    Element *rootElement;

    int bytesInput = 0;
    int bytesOutput = 0;
    int bitsInput = 0;
    int bitsOutput = 0;
    QString dataOutput;
    //是否储存数据的标志(是否按下"编码"的标志)
    bool isData = false;
};

#endif // MAINWINDOW_H
