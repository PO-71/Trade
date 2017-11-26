#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ID_version = 1010;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath() + "/trade.sqlite");
    if(!db.open())
        QMessageBox::critical(this, tr("ERROR!"), db.lastError().databaseText());
    else
    {
        db.exec("pragma foreign_keys=1;");
        table = new QSqlRelationalTableModel(0, db);
        ui->tableView->setModel(table);
    }

    ui->button_add->setEnabled(false);
    ui->button_delete->setEnabled(false);

    mapTable[QString("nomenclature")] = NOMENCLATURE;
    mapTable[QString("stock")] = STOCK;
    mapTable[QString("remnants")] = REMNANTS;
    mapTable[QString("purchase")] = PURCHASE;
    mapTable[QString("sale")] = SALE;

    connect(ui->action_nomenclature, SIGNAL(triggered()), this, SLOT(SelectNomenclature()));
    connect(ui->action_stock, SIGNAL(triggered()), this, SLOT(SelectStock()));
    connect(ui->action_remnants, SIGNAL(triggered()), this, SLOT(SelectRemnants()));
    connect(ui->action_purchase, SIGNAL(triggered()), this, SLOT(SelectPurchase()));
    connect(ui->action_sale, SIGNAL(triggered()), this, SLOT(SelectSale()));
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(SelectAbout()));
    connect(ui->action_CheckUpdate, SIGNAL(triggered()), this, SLOT(SelectUpdate()));
}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}

void MainWindow::SelectNomenclature()
{
    SelectTable(QString("nomenclature"));
}

void MainWindow::SelectStock()
{
    SelectTable(QString("stock"));
}

void MainWindow::SelectRemnants()
{
    SelectTable(QString("remnants"));
}

void MainWindow::SelectPurchase()
{
    SelectTable(QString("purchase"));
}

void MainWindow::SelectSale()
{
    SelectTable(QString("sale"));
}

void MainWindow::SelectAbout()
{
    aboutWindow = new AboutWindow();
    std::string ver = APP_VERSION;
    aboutWindow->setVersion(QString::fromStdString(ver));
    aboutWindow->show();
}

void MainWindow::SelectUpdate()
{
    std::string path = (QCoreApplication::applicationDirPath() + "/Update.exe").toStdString();
    std::string version = (QString::number(ID_version)).toStdString();
    ShellExecuteA(NULL, "runas", path.c_str(), version.c_str(), NULL, SW_SHOW);
}

void MainWindow::SelectTable(QString nameTable)
{
    ui->button_add->setEnabled(true);
    ui->button_delete->setEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    table = new QSqlRelationalTableModel(0, db);
    table->setTable(nameTable);
    table->setEditStrategy(QSqlTableModel::OnManualSubmit);

    switch (mapTable[nameTable]) {
    case NOMENCLATURE:
    {
        table->setHeaderData(0, Qt::Horizontal, QObject::tr("Артикул"), Qt::DisplayRole);
        table->setHeaderData(1, Qt::Horizontal, QObject::tr("Номенклатура"), Qt::DisplayRole);
        table->setHeaderData(2, Qt::Horizontal, QObject::tr("Наценка, %"), Qt::DisplayRole);
        break;
    }
    case STOCK:
    {
        table->setHeaderData(1, Qt::Horizontal, QObject::tr("Склад"), Qt::DisplayRole);
        break;
    }
    case REMNANTS:
    {
        table->setHeaderData(1, Qt::Horizontal, QObject::tr("Склад"), Qt::DisplayRole);
        table->setHeaderData(2, Qt::Horizontal, QObject::tr("Номенклатура"), Qt::DisplayRole);
        table->setHeaderData(3, Qt::Horizontal, QObject::tr("Количество, шт."), Qt::DisplayRole);
        table->setHeaderData(4, Qt::Horizontal, QObject::tr("Сумма закупки, руб."), Qt::DisplayRole);
        table->setRelation(1, QSqlRelation("stock", "id_stock", "name"));
        table->setRelation(2, QSqlRelation("nomenclature", "art", "name"));
        ui->button_add->setEnabled(false);
        ui->button_delete->setEnabled(false);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        break;
    }
    case PURCHASE:
    {
        table->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер"), Qt::DisplayRole);
        table->setHeaderData(1, Qt::Horizontal, QObject::tr("Дата"), Qt::DisplayRole);
        table->setHeaderData(2, Qt::Horizontal, QObject::tr("Склад"), Qt::DisplayRole);
        table->setHeaderData(3, Qt::Horizontal, QObject::tr("Номенклатура"), Qt::DisplayRole);
        table->setHeaderData(4, Qt::Horizontal, QObject::tr("Количество, шт."), Qt::DisplayRole);
        table->setHeaderData(5, Qt::Horizontal, QObject::tr("Цена закупки, руб."), Qt::DisplayRole);
        table->setRelation(2, QSqlRelation("stock", "id_stock", "name"));
        table->setRelation(3, QSqlRelation("nomenclature", "art", "name"));
        ui->button_delete->setEnabled(false);
        break;
    }
    case SALE:
    {
        table->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер"), Qt::DisplayRole);
        table->setHeaderData(1, Qt::Horizontal, QObject::tr("Дата"), Qt::DisplayRole);
        table->setHeaderData(2, Qt::Horizontal, QObject::tr("Склад"), Qt::DisplayRole);
        table->setHeaderData(3, Qt::Horizontal, QObject::tr("Номенклатура"), Qt::DisplayRole);
        table->setHeaderData(4, Qt::Horizontal, QObject::tr("Количество, шт."), Qt::DisplayRole);
        table->setHeaderData(5, Qt::Horizontal, QObject::tr("Цена продажи, руб."), Qt::DisplayRole);
        table->setRelation(2, QSqlRelation("stock", "id_stock", "name"));
        table->setRelation(3, QSqlRelation("nomenclature", "art", "name"));
        ui->button_delete->setEnabled(false);
        break;
    }
    default:
        break;
    }

    table->select();
    ui->tableView->setModel(table);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
    ui->tableView->show();
    if(nameTable == "remnants" || nameTable == "stock")
        ui->tableView->setColumnHidden(0, true);
    else
        ui->tableView->setColumnHidden(0, false);
}

void MainWindow::on_button_add_clicked()
{
    int count = table->rowCount();
    table->insertRow(count);
    if(mapTable[table->tableName()] == SALE || mapTable[table->tableName()] == PURCHASE)
    {
        ui->tableView->model()->setData(ui->tableView->model()->index(count, 1), QDate::currentDate());
    }
}

void MainWindow::on_button_delete_clicked()
{
    int row = ui->tableView->selectionModel()->currentIndex().row();
    table->removeRow(row);
    if(!table->submitAll())
    {
        QMessageBox::critical(this, tr("ERROR!"), table->lastError().text());
        table->revertAll();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(ui->tableView->hasFocus() && (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter))
    {
        if(mapTable[table->tableName()] == SALE || mapTable[table->tableName()] == PURCHASE)
        {
            int row = ui->tableView->selectionModel()->currentIndex().row();
            // получаем поля из строки
            QSqlQueryModel *queryModel = new QSqlQueryModel;
            QString date = (ui->tableView->model()->data(ui->tableView->model()->index(row, 1))).toString();
            QString stock = (ui->tableView->model()->data(ui->tableView->model()->index(row, 2))).toString();
            QString nomenclature = (ui->tableView->model()->data(ui->tableView->model()->index(row, 3))).toString();
            int quantity = (ui->tableView->model()->data(ui->tableView->model()->index(row, 4))).toInt();
            /* получаем внешние ключи */
            // получаем id_stock
            QString str_query = "SELECT id_stock FROM stock WHERE name = '" + stock + "'";
            queryModel->setQuery(str_query);
            QString id_stock = queryModel->data(queryModel->index(0, 0)).toString();
            // получаем art
            str_query = "SELECT art FROM nomenclature WHERE name = '" + nomenclature + "'";
            queryModel->setQuery(str_query);
            QString art = queryModel->data(queryModel->index(0, 0)).toString();
            // ищем записи об остатках по складу и номенклатуре
            str_query = "SELECT id_record, quantity, amount FROM remnants WHERE id_stock = " + id_stock + " AND art = " + art + " AND quantity >= " + QString("%1").arg(quantity);
            queryModel->setQuery(str_query);
            if(mapTable[table->tableName()] == SALE)
            {
                if(QDate::fromString(date, "yyyy-MM-dd") >= QDate::currentDate())
                {
                    if(queryModel->rowCount() > 0)
                    {
                        double purchase_price = queryModel->data(queryModel->index(0, 2)).toDouble() / queryModel->data(queryModel->index(0, 1)).toDouble();
                        str_query = "SELECT extra_charge FROM nomenclature WHERE art = " + art;
                        queryModel->setQuery(str_query);
                        double sale_price = purchase_price * double(100 + queryModel->data(queryModel->index(0, 0)).toInt()) / 100.0;
                        ui->tableView->model()->setData(ui->tableView->model()->index(row, 5), sale_price);
                        if(!table->submitAll())
                        {
                            QMessageBox::critical(this, tr("ERROR!"), table->lastError().text());
                        }
                        else
                        {
                            double amount = double(quantity) * purchase_price;
                            str_query = "UPDATE remnants SET quantity = quantity - " + QString("%1").arg(quantity) + ", amount = amount - " + QString("%1").arg(amount) + " WHERE id_stock = " + id_stock + " AND art = " + art;
                            queryModel->setQuery(str_query);
                        }
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("ERROR!"), QString("При списании номенклатуры '" + nomenclature + "' со склада '" + stock + "' образуется отрицательный остаток. Операция отменена."));
                    }
                }
                else
                {
                    QMessageBox::critical(this, tr("ERROR!"), QString("Дата продажи не может быть меньше текущей. Операция отменена."));
                }
            }
            else if(mapTable[table->tableName()] == PURCHASE)
            {
                if(QDate::fromString(date, "yyyy-MM-dd") <= QDate::currentDate())
                {
                    if(!table->submitAll())
                    {
                        QMessageBox::critical(this, tr("ERROR!"), table->lastError().text());
                     }
                    else
                    {
                        double price = (ui->tableView->model()->data(ui->tableView->model()->index(row, 5))).toDouble();
                        double amount = double(quantity) * price;
                        if(queryModel->rowCount() > 0)
                            str_query = "UPDATE remnants SET quantity = quantity + " + QString("%1").arg(quantity) + ", amount = amount + " + QString("%1").arg(amount) + " WHERE id_stock = " + id_stock + " AND art = " + art;
                        else
                            str_query = "INSERT INTO remnants(id_stock, art, quantity, amount) VALUES (" + id_stock + "," + art + "," + QString("%1").arg(quantity) + "," + QString("%1").arg(quantity * price) + ")";
                        queryModel->setQuery(str_query);
                    }
                }
                else
                {
                    QMessageBox::critical(this, tr("ERROR!"), QString("Дата закупки не может быть больше текущей. Операция отменена."));
                }
            }
        }
        else
        {
            if(!table->submitAll())
            {
                QMessageBox::critical(this, tr("ERROR!"), table->lastError().text());
            }
        }
    }
}
