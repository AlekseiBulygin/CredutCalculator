#include "mainwidget.h"
#include <iostream>
#include <Qt>
#include <string>


class NotEditableDelegate : public QItemDelegate
{
public:
    explicit NotEditableDelegate(QObject *parent = 0)
        : QItemDelegate(parent)
    {}

protected:
    QWidget* createEditor(QWidget *, const QStyleOptionViewItem &, const QModelIndex &) const
    { return Q_NULLPTR; }

};


class EditableDelegate : public QItemDelegate
{
public:
    explicit EditableDelegate(QObject *parent = 0)
        : QItemDelegate(parent)
    {}

protected:
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
    {
        QRegExp *rx = new QRegExp("|\\d+|(\\d+\\.{1}\\d{1,2})");
        QRegExpValidator *validator = new QRegExpValidator(*rx);
        QLineEdit *principalBalance = new QLineEdit(parent);
        principalBalance->setValidator(validator);
        return principalBalance;
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model,   const QModelIndex &index) const {
        QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
        QString value = QString::number(lineEdit->text().toDouble(), 'f', 2);
        model->setData(index, value, Qt::EditRole);
    }
};


MainWidget::MainWidget()
{
    this->setMinimumSize(660, 500);
    amountLineEdit->setValidator(validator);
    amountLineEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    amount->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    monthLineEdit->setValidator(validator);
    month->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    monthLineEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    percentLineEdit->setValidator(validator);
    percentLineEdit->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    percent->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    layout->addWidget(amount, 0, 0, 1, 1);
    layout->addWidget(amountLineEdit, 0, 1, 1, 1);
    layout->addWidget(month, 1, 0, 1, 1);
    layout->addWidget(monthLineEdit, 1, 1, 1, 1);
    layout->addWidget(percent, 2, 0, 1, 1);
    layout->addWidget(percentLineEdit, 2, 1, 1, 1);

    layout->addWidget(payment, 0, 2, 1, 1);
    layout->addWidget(paymentValue, 0, 3, 1, 1);
    layout->addWidget(overPay, 1, 2, 1, 1);
    layout->addWidget(overPayValue, 1, 3, 1, 1);
    layout->addWidget(totalAmount, 2, 2, 1, 1);
    layout->addWidget(totalAmountValue, 2, 3, 1, 1);

    layout->addWidget(countButton, 3, 1, 1, 2);

    model->setColumnCount(6);
    QStringList headers = {tr("Data"), tr("Mortgage Payment"), tr("Principal"), tr("Interests"),
                           tr("Principal Balance"), tr("Early payoff")};
    model->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < model->columnCount(); i++) {
        tableView->setColumnWidth(i, 120);
    }

    layout->addWidget(tableView, 4, 0, 4, 4);

    QObject::connect(countButton, SIGNAL(clicked()), this, SLOT(countCredit()));
    QObject::connect(model, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(itemChanged(QStandardItem *)));
    QObject::connect(choiceWidget->ok_btn, SIGNAL(clicked()), this, SLOT(optionChosed()));
}


void MainWidget::countCredit(){
    creditBalance.clear();
    creditPrincipal.clear();
    model->removeRows(0, model->rowCount());
    months = monthLineEdit->text().toInt();
    double money = amountLineEdit->text().toFloat();
    double monthPercent = percentLineEdit->text().toFloat() / 12 / 100;
    double mortgagePayment = amountLineEdit->text().toFloat() *
            (monthPercent / (1 - pow(1 + monthPercent, -months)));
    double overPay = money * monthPercent;
    double principal = mortgagePayment - money * monthPercent;
    double principalBalance = money - principal;

    creditBalance.append(round(principalBalance*100.0)/100.0);
    creditPrincipal.append(round(principal*100.0)/100.0);

    for (int i=0; i < months - 1; i++){
        overPay += principalBalance * monthPercent;
        principal = mortgagePayment - principalBalance * monthPercent;
        principalBalance -= principal;

        creditBalance.append(round(principalBalance*100.0)/100.0);
        creditPrincipal.append(round(principal*100.0)/100.0);
    }

    this->paymentValue->setText(QString::number(mortgagePayment, 'f', 2));
    this->overPayValue->setText(QString::number(overPay, 'f', 2));
    this->totalAmountValue->setText(QString::number(money + overPay, 'f', 2));

    QDate today = QDate::currentDate();
    QString dateFormat = "dd.MM.yyyy";

    for(int i = 0; i < months; i++){
        model->setItem(i, 0, new QStandardItem(today.toString(dateFormat)));
        model->setItem(i, 1, new QStandardItem(QString::number(mortgagePayment, 'f', 2)));
        model->setItem(i, 2, new QStandardItem(QString::number(creditPrincipal[i], 'f', 2)));
        model->setItem(i, 3, new QStandardItem(QString::number(mortgagePayment - creditPrincipal[i], 'f', 2)));
        model->setItem(i, 4, new QStandardItem(QString::number(creditBalance[i], 'f', 2)));
        today = today.addMonths(1);
    }

    tableView->setModel(model);

    for(int c = 0; c < tableView->model()->columnCount(); c++){
        if(c != 5){
            tableView->setItemDelegateForColumn(c, new NotEditableDelegate(tableView));
        }
        else{
            tableView->setItemDelegateForColumn(c, new EditableDelegate(tableView));
        }
    }
}


void MainWidget::itemChanged(QStandardItem *item){
    if (item->column() == 5){
        choiceWidget->chosenRow = item->row();
        choiceWidget->show();
    }
}

void MainWidget::optionChosed(){

    earlyPayoff(choiceWidget->chosenRow);
    choiceWidget->decrease_payment_rbtn->setChecked(true);
    choiceWidget->close();
}


void MainWidget::earlyPayoff(int row){
    int size = creditBalance.size();
    model->removeRows(row + 1, size-1 - row);
    while(size - 1 > row){
        creditBalance.removeLast();
        creditPrincipal.removeLast();
        size--;
    }
    QString dateFormat = "dd.MM.yyyy";
    QDate date = QDate::fromString(model->item(row, 0)->text(), dateFormat);
    double money = model->item(row, 4)->text().toFloat() - model->item(row, 5)->text().toFloat();
    double monthPercent = percentLineEdit->text().toFloat() / 12 / 100;

    if (choiceWidget->decrease_payment_rbtn->isChecked()){
        double mortgagePayment = money * (monthPercent / (1 - pow(1 + monthPercent, -(months-1-row))));
        double overPay = money * monthPercent;
        double principal = mortgagePayment - money * monthPercent;
        double principalBalance = money - principal;

        creditBalance.append(round(principalBalance*100.0)/100.0);
        creditPrincipal.append(round(principal*100.0)/100.0);

        for (int i=row + 1; i < months - 1; i++){
            overPay += principalBalance * monthPercent;
            principal = mortgagePayment - principalBalance * monthPercent;
            principalBalance -= principal;

            creditBalance.append(round(principalBalance*100.0)/100.0);
            creditPrincipal.append(round(principal*100.0)/100.0);
           }

        for(int i = row + 1; i < months; i++){
            date = date.addMonths(1);
            model->setItem(i, 0, new QStandardItem(date.toString(dateFormat)));
            model->setItem(i, 1, new QStandardItem(QString::number(mortgagePayment, 'f', 2)));
            model->setItem(i, 2, new QStandardItem(QString::number(creditPrincipal[i], 'f', 2)));
            model->setItem(i, 3, new QStandardItem(QString::number(mortgagePayment - creditPrincipal[i], 'f', 2)));
            model->setItem(i, 4, new QStandardItem(QString::number(creditBalance[i], 'f', 2)));
        }
    }
    else {
        double mortgagePayment = amountLineEdit->text().toDouble() *
                (monthPercent / (1 - pow(1 + monthPercent, -monthLineEdit->text().toInt())));
        double overPay = money * monthPercent;
        double principal = mortgagePayment - money * monthPercent;
        double principalBalance = money - principal;

        creditBalance.append(round(principalBalance*100.0)/100.0);
        creditPrincipal.append(round(principal*100.0)/100.0);

        while (principalBalance > mortgagePayment - principalBalance * monthPercent){
            overPay += principalBalance * monthPercent;
            principal = mortgagePayment - principalBalance * monthPercent;
            principalBalance -= principal;

            creditBalance.append(round(principalBalance*100.0)/100.0);
            creditPrincipal.append(round(principal*100.0)/100.0);

        }

        QList <QStandardItem *> lastRow;

        if (principal > principalBalance){
            QDate lastDate = date.addMonths(creditBalance.size() - row);
            lastRow.append(new QStandardItem(lastDate.toString(dateFormat)));

            double lastMortgagePayment = principalBalance + principalBalance * monthPercent;
            lastRow.append(new QStandardItem(QString::number(lastMortgagePayment, 'f', 2)));
            lastRow.append(new QStandardItem(QString::number(principalBalance, 'f', 2)));
            lastRow.append(new QStandardItem(QString::number(lastMortgagePayment - principalBalance, 'f', 2)));
            lastRow.append(new QStandardItem(QString::number(std::fabs(lastMortgagePayment - principalBalance -
                                                             principalBalance * monthPercent), 'f', 2)));
        }

        for(int i = row + 1; i < creditPrincipal.size(); i++){
            date = date.addMonths(1);
            model->setItem(i, 0, new QStandardItem(date.toString(dateFormat)));
            model->setItem(i, 1, new QStandardItem(QString::number(mortgagePayment, 'f', 2)));
            model->setItem(i, 2, new QStandardItem(QString::number(creditPrincipal[i], 'f', 2)));
            model->setItem(i, 3, new QStandardItem(QString::number(mortgagePayment - creditPrincipal[i], 'f', 2)));
            model->setItem(i, 4, new QStandardItem(QString::number(creditBalance[i], 'f', 2)));
        }

        model->insertRow(creditPrincipal.size(), lastRow);
        months = model->rowCount();

    }
};
