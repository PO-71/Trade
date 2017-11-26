#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QKeyEvent>
#include <QtSql>
#include <windows.h>
#include "aboutwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    int ID_version;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void keyPressEvent(QKeyEvent *event);

private slots:
    void SelectNomenclature();
    void SelectStock();
    void SelectRemnants();
    void SelectPurchase();
    void SelectSale();
    void SelectAbout();
    void SelectUpdate();
    void on_button_add_clicked();
    void on_button_delete_clicked();

private:
    Ui::MainWindow *ui;
    AboutWindow *aboutWindow;
    QSqlDatabase db;
    QSqlRelationalTableModel *table;
    enum {NOMENCLATURE, STOCK, REMNANTS, PURCHASE, SALE};
    std::map <QString, int> mapTable;
    void SelectTable(QString nameTable);
};

#endif // MAINWINDOW_H
