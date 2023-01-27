#ifndef EXPLAIN_H
#define EXPLAIN_H

#include <QWidget>

namespace Ui {
class explain;
}

class explain : public QWidget
{
    Q_OBJECT

public:
    explicit explain(QWidget *parent = nullptr);
    ~explain();

private:
    Ui::explain *ui;
};

#endif // EXPLAIN_H
