#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItem>
#include <QStandardItemModel>
#include <QGraphicsTextItem>
#include<QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"字符"<<"数量(权)"<<"ASCII码"<<"哈夫曼编码");
    ui->tableWidget->setColumnWidth(0,60);
    ui->tableWidget->setColumnWidth(1,65);
    ui->tableWidget->setColumnWidth(2,60);
    ui->tableWidget->setColumnWidth(3,127);

    ui->pushButton_SaveImage->setVisible(false);

    ui->Button_encode->setVisible(false);
    ui->Button_decode->setVisible(false);

    ui->action_SaveImage->setEnabled(false);

}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::on_pushButton_Code_clicked(){
    QString dataiInput;
    //优先看spinbox中是否有数据
    //若有
    if(ui->spinBox->value()!=0){
        //ui->tiaoshikuang->setText("非0");
        scene->clear();
        TableSymbolsToInput(dataiInput);

        scene->clear();
        shortSymbolsToVector(dataiInput);//储存数据并降序排列

    }
    else{
        if(ui->textEdit_InputData->toPlainText().isEmpty()) {
            //当用户未输入时便编码
            ui->textEdit_InputData->setText("This is apple.");
            dataiInput.append("This is apple.");
        }
        scene->clear();

        dataiInput= ui->textEdit_InputData->toPlainText();//获取输入内容
        shortSymbolsToVector(dataiInput);//储存数据并降序排列
    }


    setRoot();
    drawNodes(0, 0, rootElement, TypeNode::Root, 0, countLevel(rootElement,0));//画树

    setSymbolsToTable();//将数据写入表格中
    isData = true;
    //setDataOut(dataiInput);//输出数据

    //ui->pushButton_SaveImage->setVisible(true);

    ui->Button_decode->setVisible(true);
    ui->Button_encode->setVisible(true);

    ui->action_SaveImage->setEnabled(true);
}



void MainWindow::on_pushButton_Clear_clicked(){
    scene->clear();

    if (!isData){
        return;
    }
    ui->tableWidget->clear();
    while (ui->tableWidget->rowCount()!= 0){
        ui->tableWidget->removeRow(0);
    }


    ui->lineEdit_CompreseBits->clear();
    ui->lineEdit_CompreseBytes->clear();
    ui->lineEdit_InputBits->clear();
    ui->lineEdit_InputBytes->clear();
    ui->lineEdit_OutpputBytes->clear();
    ui->lineEdit_OutputBits->clear();
    ui->textEdit_InputData->clear();
    ui->textEdit_OutputData->clear();

    ui->Button_decode->setVisible(false);
    ui->Button_encode->setVisible(false);

    ui->spinBox->setValue(0);

    ui->tableWidget_Input->clear();
    ui->tableWidget_Input->setHorizontalHeaderLabels(QStringList()<<"字符"<<"数量(权)");

    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"字符"<<"数量(权)"<<"ASCII码"<<"哈夫曼编码");
    elements.clear();

    rootElement = NULL;
    isData = false;
    ui->pushButton_SaveImage->setVisible(false);
    bytesInput = 0;
    bytesOutput = 0;
    bitsInput = 0;
    bitsOutput = 0;
    dataOutput.clear();

}

void MainWindow::shortSymbolsToVector(QString dataiInput) {
    //QString dataiInput = ui->textEdit_InputData->toPlainText();//获取输入内容

    //将每个字符添加进elements容器中
    for (int i = 0; i < dataiInput.length(); i++) {
        bool isRepeat = false;//先设无相同字符
        for (int j = 0; j < elements.size(); j++){
            //在已添加的字符中寻找是否重复，若重复，数量++且标记为有重复
            if (dataiInput[i] == elements[j]->symbol.getSymbol()){
                elements[j]->symbol.increaseNumberSymbol();
                isRepeat = true;
                break;
            }
        }
        if (!isRepeat){//添加新字符
            elements.push_back(new Element(Symbol(dataiInput[i], 1)));
        }
    }


    //将elements中储存的字符数核对且去掉没有的字符（去除多次按button而数量叠加的情况）
    for (int i = 0; i < elements.size(); i++) {
        int numSym = 0;
        for (int j = 0; j < dataiInput.length(); j++){
            if (elements[i]->symbol.getSymbol() == dataiInput[j]){
                numSym++;
            }
        }
        if(numSym){
            elements[i]->symbol.setSymbolNumber(numSym);
        }
        else{
            elements.remove(i);
        }
    }

    //冒泡排序，根据numberSymbols降序排列
    for (int i = 0; i < elements.size(); i++){
        for (int j = 1; j < elements.size() - i; j++){
            if (elements[j-1]->symbol < elements[j]->symbol){//重载过运算符
                Element * tempElem = elements[j-1];
                elements[j-1] = elements[j];
                elements[j] = tempElem;
            }
        }
    }
}



void MainWindow::TableSymbolsToInput(QString& dataiInput){
    int row=ui->spinBox->value();
    QString str;
    QString strNum;
    for(int i=0;i<row;i++){
        str =ui->tableWidget_Input->item(i,0)->text();
        strNum=ui->tableWidget_Input->item(i,1)->text();
        for(int j=strNum[0].toLatin1()-48;j>0;j--){
            dataiInput+=str;
        }
    }
}


void MainWindow::setSymbolsToTable() {
    ui->tableWidget->clear();
    while (ui->tableWidget->rowCount()!= 0){
        ui->tableWidget->removeRow(0);
    }
    //设置表格水平表头
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"字符"<<"数量(权)"<<"ASCII码"<<"哈夫曼编码");
    //根据vector中元素数量设置表格行数
    ui->tableWidget->setRowCount(elements.size());

    for (int i = 0; i < elements.size(); i++){
       QTableWidgetItem * item;//单元格
       //单独考虑空格键
       if (elements[i]->symbol.getSymbol() != ' '){
           item =new QTableWidgetItem(QString(elements[i]->symbol.getSymbol()));
       }
       else{
           item =new QTableWidgetItem("Space");
       }
       ui->tableWidget->setItem(i, 0, item);//将指定单元格添加至指定位置
       item =new QTableWidgetItem(QString::number(elements[i]->symbol.getSymbolNumber()));
       ui->tableWidget->setItem(i, 1, item);
       //toLatin1()函数可以返回字符的ASCII码
       item =new QTableWidgetItem(QString::number(elements[i]->symbol.getSymbol().toLatin1()));
       ui->tableWidget->setItem(i, 2, item);
       item =new QTableWidgetItem(elements[i]->huffmanCode);
       ui->tableWidget->setItem(i, 3, item);
    }
}

void MainWindow::setRoot() {
    //将elements完全复制一份
    QVector<Element*> tempEl;
    for (int i = 0; i < elements.size(); i++){
        tempEl.push_back(new Element(elements[i]->symbol, elements[i]->symbol.getSymbolNumber()));
    }

    for (int i = tempEl.size() - 1; i > 0;  i--) {
        //降序排列，故取最小的两结点时从后向前
        //该构造函数原型是（leftNode,rightNode,numberSymbol）
        Element* temp = new Element(tempEl[i-1], tempEl[i], tempEl[i-1]->numberSymbolsNode + tempEl[i]->numberSymbolsNode);
        tempEl[i-1]->parentNode = temp;
        tempEl[i]->parentNode = temp;
        tempEl[i-1] = temp;
        //重新构造出结点后的排序
        for (int j = i-1; j > 0; j--) {
            if (tempEl[j]->numberSymbolsNode > tempEl[j-1]->numberSymbolsNode) {
                Element *temp1 = tempEl[j-1];
                tempEl[j-1] = tempEl[j];;
                tempEl[j] = temp1;
            }
            else{
                break;
            }
        }
    }
    rootElement = tempEl[0];
}

void MainWindow::drawNodes(int xPosition,int yPosition,Element *node,TypeNode typeNode,int level,int levels){
    //yLine使得描述垂直结点间距更容易
    int yLine = 50, y = 100, x = pow(2, levels) * 26 ;//x,y表示每个Item之间的间距
    int lev = level + 1;

    QColor nodeColor(208,222,170);
    QColor rectColor(92,179,204);
    QColor lineColor(246,140,96);

    /*
    分为6种情况，不考虑有左结点无右结点和有右结点无左结点情况
    *1.根节点，左右子结点均为空
    *2.根节点，左右子结点均不为空
    *3.左节点，左右子结点均为空    注:此左节点包括右子结点的左节点
    *4.左节点，左右子结点均不为空
    *5.右节点，左右子结点均为空
    *6.右节点，左右子结点均不为空
    */

    //1
    if (typeNode == Root && node->leftNode == NULL && node->rifgtNode == NULL) {
        //QGraphicsScene类中的绘图方法，其中addEllipse()函数用于绘制椭圆
        scene->addEllipse(xPosition,yPosition,50,50,QPen(nodeColor,2),nodeColor);
        //scene->addEllipse(xPosition,yPosition,50,50, QPen(QBrush(Qt::red),2), QBrush(Qt::NoBrush));

        QGraphicsTextItem* textItem = new QGraphicsTextItem();
        textItem->setPos(xPosition+18 - getXPosition(node->numberSymbolsNode),yPosition +13);
        textItem->setPlainText(QString::number(node->numberSymbolsNode));
        scene->addItem(textItem);

        node->huffmanCode = "0";
        xPosition -= 100; yPosition += y;
        //画矩形,矩形边长为50
        scene->addRect(xPosition-25, yPosition+3,46,46,QPen(rectColor, 2),rectColor);

        QGraphicsTextItem * textItem1 = new QGraphicsTextItem();
        if (node->symbol.getSymbol() != ' ') {
            textItem1->setPos(xPosition+1-25,yPosition +15);
            textItem1->setPlainText("字符:'" + QString(node->symbol.getSymbol()) + "'");
        }
        else {
            textItem1->setPos(xPosition - 25,yPosition +15);
            textItem1->setPlainText("Space");
        }
        scene->addItem(textItem1);

        QGraphicsTextItem* textItem2 = new QGraphicsTextItem();
        textItem2->setPos(xPosition+10-25-getXPosition(node->numberSymbolsNode),yPosition);
        textItem2->setPlainText("权:" + QString::number(node->numberSymbolsNode));
        scene->addItem(textItem2);

        QGraphicsTextItem * textItem3 = new QGraphicsTextItem();
        textItem3->setPos(xPosition - 3.5 - 3 * node->huffmanCode.length(),yPosition + 30);
        textItem3->setPlainText(node->huffmanCode);
        scene->addItem(textItem3);

        //线上标记的值
        QGraphicsTextItem * textItem4 = new QGraphicsTextItem();
        textItem4->setPos(xPosition + 125 / 2 - 20  ,yPosition - 50);
        textItem4->setPlainText("0");
        scene->addItem(textItem4);

        scene->addLine(xPosition + 125, yPosition - yLine, xPosition, yPosition, QPen(lineColor, 2.5, Qt::SolidLine,Qt::RoundCap));
        setCodeHuffmanaToElement(node->symbol.getSymbol(), node->huffmanCode);
        //drawNodes(xPosition + 25 - x, yPosition + y, node, Left, 0, levels);
        return;
    }
    //2
    else if (typeNode == Root && node->leftNode != NULL && node->rifgtNode != NULL){
        scene->addEllipse(xPosition,yPosition,50,50, QPen(nodeColor,2), nodeColor);
        //scene->addEllipse(xPosition,yPosition,50,50, QPen(QBrush(Qt::red),2), QBrush(Qt::NoBrush));
        QGraphicsTextItem* textItem = new QGraphicsTextItem();
        textItem->setPos(xPosition+18 - getXPosition(node->numberSymbolsNode),yPosition +13);
        textItem->setPlainText(QString::number(node->numberSymbolsNode));
        scene->addItem(textItem);
        node->leftNode->huffmanCode = "0";
        node->rifgtNode->huffmanCode = "1";
        drawNodes(xPosition + 25 - x , yPosition + y, node->leftNode, Left, lev, levels);
        drawNodes(xPosition + 25 + x, yPosition + y, node->rifgtNode, Right ,lev, levels);
    }
    //3
    else if (typeNode == Left && node->leftNode == NULL && node->rifgtNode == NULL) {
        //画框
        scene->addRect(xPosition-25, yPosition+3,46,46,QPen(rectColor, 2),rectColor);
        QGraphicsTextItem* textItem = new QGraphicsTextItem();
        if (node->symbol.getSymbol() != ' ') {
            textItem->setPos(xPosition+3 - 25,yPosition +15);
            textItem->setPlainText("字符:'" + QString(node->symbol.getSymbol()) + "'");
        }
        else {
            textItem->setPos(xPosition - 25,yPosition +15);
            textItem->setPlainText("Space");
        }
        scene->addItem(textItem);
        QGraphicsTextItem* textItem2 = new QGraphicsTextItem();
        textItem2->setPos(xPosition+10 - 25 - getXPosition(node->numberSymbolsNode),yPosition );
        textItem2->setPlainText("权:" + QString::number(node->numberSymbolsNode));
        scene->addItem(textItem2);

        QGraphicsTextItem* textItem3 = new QGraphicsTextItem();
        textItem3->setPos(xPosition - 3.5 - 3 * node->huffmanCode.length(),yPosition + 30);
        textItem3->setPlainText(node->huffmanCode);
        scene->addItem(textItem3);

        QGraphicsTextItem * textItem4 = new QGraphicsTextItem();
        textItem4->setPos(xPosition + (x / pow(2, level -1)) / 2 - 20  ,yPosition - 50);
        textItem4->setPlainText("0");
        scene->addItem(textItem4);
        scene->addLine(xPosition + (x / pow(2, level -1)), yPosition - yLine, xPosition, yPosition, QPen(lineColor, 2.5, Qt::SolidLine,Qt::RoundCap));
        setCodeHuffmanaToElement(node->symbol.getSymbol(), node->huffmanCode);
    }
    //4
    else if (typeNode == Left && node->leftNode != NULL && node->rifgtNode != NULL){
        scene->addEllipse(xPosition - 25,yPosition,50,50, QPen(nodeColor,2), nodeColor);
        //scene->addEllipse(xPosition - 25,yPosition,50,50, QPen(QBrush(Qt::red),2), QBrush(Qt::NoBrush));
        QGraphicsTextItem * textItem = new QGraphicsTextItem();
        textItem->setPos(xPosition+18 - 25 - getXPosition(node->numberSymbolsNode),yPosition +13);
        textItem->setPlainText(QString::number(node->numberSymbolsNode));
        scene->addItem(textItem);
        if (level != 1){
            scene->addLine(xPosition + (x / pow(2, level -1)), yPosition - yLine, xPosition, yPosition, QPen(lineColor, 2.5, Qt::SolidLine,Qt::RoundCap));
        }
        //第1，2层之间
        else{
            scene->addLine(xPosition + x, yPosition - yLine, xPosition, yPosition, QPen(lineColor, 2.5, Qt::SolidLine,Qt::RoundCap));
        }
        int mLev = 0;
        node->leftNode->huffmanCode = node->huffmanCode + "0";
        node->rifgtNode->huffmanCode = node->huffmanCode + "1";
        QGraphicsTextItem * textItem4 = new QGraphicsTextItem();
        textItem4->setPos(xPosition + (x / pow(2, level -1)) / 2 - 20  ,yPosition - 50);
        textItem4->setPlainText("0");
        scene->addItem(textItem4);
        drawNodes(xPosition - (x / pow(2, level - mLev)), yPosition + y, node->leftNode, Left, lev - mLev, levels);
        drawNodes(xPosition + (x / pow(2, level - mLev)), yPosition + y, node->rifgtNode, Right ,lev - mLev, levels);
    }
    //5
    else if (typeNode == Right && node->leftNode == NULL && node->rifgtNode == NULL) {
        scene->addRect(xPosition-25, yPosition+3,46,46,QPen(rectColor, 2),rectColor);
        QGraphicsTextItem* textItem = new QGraphicsTextItem();
        if (node->symbol.getSymbol() != ' ') {
            textItem->setPos(xPosition+3 - 25,yPosition +15);
            textItem->setPlainText("字符:'" + QString(node->symbol.getSymbol()) + "'");
        }
        else {
            textItem->setPos(xPosition+ - 25,yPosition +15);
            textItem->setPlainText("Space");
        }
        scene->addItem(textItem);
        QGraphicsTextItem* textItem2 = new QGraphicsTextItem();
        textItem2->setPos(xPosition+10 -25 - getXPosition(node->numberSymbolsNode),yPosition);
        textItem2->setPlainText("权:" + QString::number(node->numberSymbolsNode));
        scene->addItem(textItem2);
        QGraphicsTextItem* textItem3 = new QGraphicsTextItem();
        textItem3->setPos(xPosition - 3.5 - 3 * node->huffmanCode.length() ,yPosition + 30);
        textItem3->setPlainText(node->huffmanCode);
        scene->addItem(textItem3);
        QGraphicsTextItem* textItem4 = new QGraphicsTextItem();
        textItem4->setPos(xPosition - (x / pow(2, level -1)) / 2 + 10  ,yPosition - 50);
        textItem4->setPlainText("1");
        scene->addItem(textItem4);
        scene->addLine(xPosition - (x / pow(2, level -1)), yPosition - yLine, xPosition, yPosition,QPen(lineColor, 2.5, Qt::SolidLine,Qt::RoundCap));
        setCodeHuffmanaToElement(node->symbol.getSymbol(), node->huffmanCode);
    }
    //6
    else if (typeNode == Right && node->leftNode != NULL && node->rifgtNode != NULL){
        scene->addEllipse(xPosition - 25,yPosition,50,50, QPen(nodeColor,2), nodeColor);
        //scene->addEllipse(xPosition - 25,yPosition,50,50, QPen(QBrush(Qt::red),2), QBrush(Qt::NoBrush));
        QGraphicsTextItem * textItem = new QGraphicsTextItem();
        textItem->setPos(xPosition+18 -25 - getXPosition(node->numberSymbolsNode),yPosition +13);
        textItem->setPlainText(QString::number(node->numberSymbolsNode));
        scene->addItem(textItem);
        if (level != 1){
            scene->addLine(xPosition - (x / pow(2, level -1)), yPosition - yLine, xPosition, yPosition, QPen(lineColor, 2, Qt::SolidLine,Qt::RoundCap));
        }
        else{
            scene->addLine(xPosition - x, yPosition - yLine, xPosition, yPosition, QPen(lineColor, 2, Qt::SolidLine,Qt::RoundCap));
        }
        int mLev = 0;
        node->leftNode->huffmanCode = node->huffmanCode  + "0";
        node->rifgtNode->huffmanCode = node->huffmanCode + "1";
        QGraphicsTextItem * textItem4 = new QGraphicsTextItem();
        textItem4->setPos(xPosition - (x / pow(2, level -1)) / 2 + 10  ,yPosition - 50);
        textItem4->setPlainText("1");
        scene->addItem(textItem4);
        drawNodes(xPosition - (x / pow(2, level - mLev)), yPosition + y, node->leftNode, Left, lev - mLev, levels);
        drawNodes(xPosition + (x / pow(2, level - mLev)), yPosition + y, node->rifgtNode, Right ,lev  - mLev, levels);
    }
}


int MainWindow::countLevel(Element *node, int numberSymbols) {
    if (node->leftNode == NULL && node->rifgtNode == NULL){
        return numberSymbols - 1;
    }
    else {
        int leftN = countLevel(node->leftNode, numberSymbols + 1);
        int rightN = countLevel(node->rifgtNode, numberSymbols +1);
        if (leftN > rightN){
            return  leftN;
        }
        return rightN;
    }
    return numberSymbols;
}


void MainWindow::setCodeHuffmanaToElement(QChar _char, QString codeHuff){
    for (int i = 0; i < elements.size(); i++){
        if (elements[i]->symbol.getSymbol() == _char){
            elements[i]->huffmanCode = codeHuff;
            return;
        }
    }
}

void MainWindow::on_pushButton_SaveImage_clicked(){
    //QFileDialog是Qt中用于文件打开和保存的对话框
    //其中getSaveFileName()方法用于打开保存文件对话框
    QString fileName = QFileDialog::getSaveFileName(this, "保存图片", "..\\hafman\\hfmTree", "JPEG (*.JPEG);;BMP Files (*.bmp);;PNG (*.png)");

    //需要动态调整保存图片的大小-->未解决
    //scene->setSceneRect(-pix.width() / 2,-pix.height() / 2,pix.width(),pix.height());
    QPixmap pix;
    if (!fileName.isNull()) {
        pix = this->ui->graphicsView->grab();
        //scene->setSceneRect(-1*(pix.width() / 2),-1*(-pix.height() / 2),pix.width(),pix.height());
        pix.save(fileName);
    }
}

int MainWindow::getXPosition(int n) {
    int xPos = 0;
    if (n >9){
        xPos += 3;
    }
    if (n >99){
        xPos += 3;
    }
    if (n >999){
        xPos += 3;
    }
    return xPos;
}

void MainWindow::setDataOut(QString dataInput) {
   dataOutput.clear();

   bytesInput = dataInput.length();
   bitsInput = bytesInput*8;

   bytesOutput = 0;
   bitsOutput = 0;

   for (int i = 0; i < bytesInput; i++) {
       for (int j = 0; j < elements.size(); j++) {
           if (dataInput[i] == elements[j]->symbol.getSymbol()){
               dataOutput += elements[j]->huffmanCode;//向输出框中依次添加每个字符对应的哈夫曼编码
               if (i < bytesInput -1){
                    dataOutput += " ";
               }
               bitsOutput += elements[j]->huffmanCode.length();
               break;
           }
       }
   }
   bytesOutput = bitsOutput / 8;
   if (bitsOutput % 8 != 0){//不整除则进位
       bytesOutput++;
   }
   ui->textEdit_OutputData->setText(dataOutput);
   //Qstring::number()可将数字型变量转化为Qstring类型，以使用setText()输出
   ui->lineEdit_InputBits->setText(QString::number(bitsInput));
   ui->lineEdit_OutputBits->setText(QString::number(bitsOutput));
   ui->lineEdit_InputBytes->setText(QString::number(bytesInput));
   ui->lineEdit_OutpputBytes->setText(QString::number(bytesOutput));
   //节省字节数=1-(新编码字节数/原编码字节数)
   ui->lineEdit_CompreseBytes->setText(QString::number((1 - (((float) bytesOutput) / ((float) bytesInput)))*100,'f',2) + "%");
   ui->lineEdit_CompreseBits->setText(QString::number((1 - (((float) bitsOutput) / ((float) bitsInput)))*100,'f',2) + "%");
}



//检测输入字符集数
void MainWindow::on_spinBox_valueChanged(int arg1){
    ui->tableWidget_Input->setRowCount(arg1);
    ui->tableWidget_Input->setColumnCount(2);
    ui->tableWidget_Input->setHorizontalHeaderLabels(QStringList()<<"字符"<<"数量(权)");
}

//编码
void MainWindow::on_Button_encode_clicked(){
    //读取叫ToBeTran文件内容，编码保存至CodeFile中，输出结果
    QString fileName=QFileDialog::getOpenFileName(this,"编码","..\\hafman\\ToBeTran.txt","TXT文本(*.txt)");
    QString fileContent;
    if(!fileName.isEmpty()){
        QFile file(fileName);
        //以只读以及文本方式读取
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);//读取文件内容
            fileContent = in.readAll();
            file.close();
        }
    }
    else{
        QMessageBox::information(this,"编码"," 您没有选择任何文件！ ");
        return;
    }
    setDataOut(fileContent);
    //此时文件内容储存于fileContent中
    //编码储存于encodeString中
    QString encodeString;
    for(int i=0;i<fileContent.size();i++){
        bool flag=false;
        for(int j=0;j<elements.size();j++){
            if(fileContent[i]==elements[j]->symbol.getSymbol()){
                encodeString+=elements[j]->huffmanCode+' ';
                flag=true;
                break;
            }
        }
        if(!flag){//若不能编码则以原形式储存
            //！！可删去
            //encodeString+=fileContent[i];
        }
    }
    ui->textEdit_OutputData->setText(encodeString);
    //将encodeString中内容保存至CodeFile.txt中

    QFile file_encode("..\\hafman\\CodeFile.txt");
    //if(file.open(QIODevice::WriteOnly|QIODevice::Text|QIODevice::Append))-->以这种方式写入不会覆盖原有数据
    if (file_encode.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file_encode);
        out <<encodeString<<"\n";
        file_encode.close();
    }

}


//译码
void MainWindow::on_Button_decode_clicked(){
    //将文件CodeFile中的代码进行译码，结果存入文件TextFile，输出结果
    QString fileName=QFileDialog::getOpenFileName(this,"译码","..\\hafman\\CodeFile.txt","TXT文本(*.txt)");
    QString fileContent;
    if(!fileName.isEmpty()){
        QFile file(fileName);
        //以只读以及文本方式读取
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
            QTextStream in(&file);//读取文件内容
            fileContent = in.readAll();
            file.close();
        }
    }
    else{
        QMessageBox::information(this,"译码"," 您没有选择任何文件！ ");
        return;
    }


    //读取到的内容储存在fileContent中
    //译码储存于decodeString中

    //设置一个临时字符串temp保存fileContent，查找是否与某个哈夫曼编码相同
    //不同则字符串长度加一
    bool flag=false;
    QString temp;
    QString decodeString;
    for(int i=0;i<fileContent.size();i++){
        temp+=fileContent[i];
        for(int j=0;j<elements.size();j++){
            if(temp==elements[j]->huffmanCode){
                decodeString+=elements[j]->symbol.getSymbol();
                flag=true;

                //若匹配，则i需自增，以防止编码后空格的影响
                i++;

                break;
            }
        }
        if(flag==true){
            temp.clear();
            flag=false;
        }
    }
    ui->textEdit_OutputData->setText(decodeString);
    //将decodeString内容写入文件TextFile
    QFile file_encode("..\\hafman\\TextFile.txt");
    if (file_encode.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file_encode);
        out <<decodeString<<"\n";
        file_encode.close();
    }
}


void MainWindow::on_action_exit_triggered(){
    this->close();
}

void MainWindow::on_action_triggered(){
    open_explain.setWindowTitle("说明");
    open_explain.show();
}

void MainWindow::on_action_SaveImage_triggered(){
    //QFileDialog是Qt中用于文件打开和保存的对话框
    //其中getSaveFileName()方法用于打开保存文件对话框
    QString fileName = QFileDialog::getSaveFileName(this, "保存图片", "..\\hafman\\hfmTree", "JPEG (*.JPEG);;BMP Files (*.bmp);;PNG (*.png)");

    //需要动态调整保存图片的大小-->未解决
    //scene->setSceneRect(-pix.width() / 2,-pix.height() / 2,pix.width(),pix.height());
    QPixmap pix;
    if (!fileName.isNull()) {
        pix = this->ui->graphicsView->grab();
        //scene->setSceneRect(-1*(pix.width() / 2),-1*(-pix.height() / 2),pix.width(),pix.height());
        pix.save(fileName);
    }
}
