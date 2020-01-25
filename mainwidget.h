#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QObject>
#include <QtWidgets>

class ChoiceWidget : public QWidget
{
    Q_OBJECT
public:
    ChoiceWidget(Qt::WindowFlags f = Qt::WindowStaysOnTopHint){
        decrease_payment_rbtn->setChecked(true);
        layout->addWidget(label, 0, 0, 1, 2);
        layout->addWidget(decrease_payment_rbtn, 1, 0, 1, 2);
        layout->addWidget(decrease_term_rbtn, 2, 0, 1, 2);
        layout->addWidget(ok_btn, 3, 1, 1, 1);
    };
    QGridLayout *layout = new QGridLayout(this);
    QLabel *label = new QLabel(tr("Please choose one of the options:"));
    QRadioButton *decrease_payment_rbtn = new QRadioButton(tr("Decrease payment"));
    QRadioButton *decrease_term_rbtn = new QRadioButton(tr("Decrease term"));
    QPushButton *ok_btn = new QPushButton("Ok");
    int chosenRow;
};


class MainWidget : public QWidget
{
    Q_OBJECT
public:
    MainWidget();
    int months;
    QList<double> creditBalance;
    QList<double> creditPrincipal;
    QGridLayout *layout = new QGridLayout(this);
    QRegExp *rx = new QRegExp("(\\d+\\.{1}\\d{1,2})");
    QRegExpValidator *validator = new QRegExpValidator(*rx);
    QLabel *amount = new QLabel(tr("Amount"));
    QLineEdit *amountLineEdit = new QLineEdit();
    QLabel *month = new QLabel(tr("Months"));
    QLineEdit *monthLineEdit = new QLineEdit();
    QLabel *percent = new QLabel(tr("Interest rate"));
    QLineEdit *percentLineEdit = new QLineEdit();
    QLabel *payment = new QLabel(tr("Payment"));
    QLabel *paymentValue = new QLabel();
    QLabel *overPay = new QLabel(tr("Overpay"));
    QLabel *overPayValue = new QLabel();
    QLabel *totalAmount = new QLabel(tr("Total amount"));
    QLabel *totalAmountValue = new QLabel();
    QPushButton *countButton = new QPushButton(tr("Count"));
    ChoiceWidget *choiceWidget = new ChoiceWidget();

    QTableView *tableView = new QTableView();
    QStandardItemModel *model = new QStandardItemModel();

public slots:
    void countCredit();
    void earlyPayoff(int row);
    void itemChanged(QStandardItem *item);
    void optionChosed();

};


#endif // MAINWIDGET_H
