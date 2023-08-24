#include "softkeylineedit.h"
#include <QMessageBox>

SoftKeyLineEdit::SoftKeyLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    numkeyboard = new NumKeyboard(this);
}

void SoftKeyLineEdit::mousePressEvent(QMouseEvent *e)
{
 //   QMessageBox::information(NULL,"test","test");
    if(e->button() == Qt::LeftButton)
    {
        numkeyboard->setWindowFlags(Qt::Dialog );
        numkeyboard->setStyleSheet("border:2px solid white;");
        numkeyboard->setText(this->text());  //当前的文本框的内容，this->指向的是SoftKeyLineEdit
  //      numkeyboard->show();
        if(numkeyboard->valid)
        {
            this->setText(numkeyboard->getText());  //此处的setText是继承QLineEdit的，numkeyboard->setTex是自定义的，目的是获取当前的文本
        }
    }
}
