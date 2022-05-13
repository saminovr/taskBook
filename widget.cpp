#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->mainPage);


    taskModel = new QStandardItemModel;
    taskModel->insertColumns(0,5);
    ui->taskListView->setModel(taskModel);

    ui->deleteTaskButton->setEnabled(false);
    ui->editTaskButton->setEnabled(false);
    ui->isDoneButton->hide();


    addInfo();

}

Widget::~Widget()
{
    delete ui;
}



void Widget::on_addTaskButton_clicked()
{
    ui->headerLabelInAddTask->setText("Добавить задачу:");
    ui->confirmButton->show();
    ui->editButton->hide();
    ui->stackedWidget->setCurrentWidget(ui->addTaskPage);
}


void Widget::on_confirmButton_clicked()
{
    if (checkInformation())
    {
        QDate* date = new QDate;
        QDate deadLineDate = ui->calendarWidget->selectedDate();

        QJsonObject object;
        object.insert("discipline", ui->disciplineLineEdit->text());
        object.insert("task", ui->taskLineEdit->text());

        object.insert("deadLineDay", ui->calendarWidget->selectedDate().day());
        object.insert("deadLineMonth", ui->calendarWidget->selectedDate().month());
        object.insert("deadLineYear", ui->calendarWidget->selectedDate().year());

        object.insert("additionDay", date->currentDate().day());
        object.insert("additionMonth", date->currentDate().month());
        object.insert("additionYear", date->currentDate().year());

        object.insert("isDone", ui->isDoneBox->isChecked());

        QJsonDocument doc;
        doc.setObject(object);

        QString headerString;
        headerString.append(ui->disciplineLineEdit->text() + " - " + ui->taskLineEdit->text());

        int days = ui->calendarWidget->selectedDate().toJulianDay() - date->currentDate().toJulianDay();

        taskModel->insertRow(taskModel->rowCount());
        for (int i=taskModel->rowCount()-2; i>-1; i--)
        {
            taskModel->setData(taskModel->index(i+1,0),taskModel->data(taskModel->index(i,0)).toString());
            taskModel->setData(taskModel->index(i+1,1),taskModel->data(taskModel->index(i,1)).toString());
            taskModel->setData(taskModel->index(i+1,2),taskModel->data(taskModel->index(i,2)).toString());
            taskModel->setData(taskModel->index(i+1,3),taskModel->data(taskModel->index(i,3)).toString());
            taskModel->setData(taskModel->index(i+1,4),taskModel->data(taskModel->index(i,4)).toString());
            taskModel->setData(taskModel->index(i+1,0),taskModel->data(taskModel->index(i,0),Qt::BackgroundRole), Qt::BackgroundRole);
        }
        taskModel->setData(taskModel->index(0,0), headerString + " | " + QString::number(days) + " д. осталось");
        taskModel->setData(taskModel->index(0,1), ui->disciplineLineEdit->text());
        taskModel->setData(taskModel->index(0,2), ui->taskLineEdit->text());
        taskModel->setData(taskModel->index(0,3), deadLineDate);
        taskModel->setData(taskModel->index(0,4), ui->isDoneBox->isChecked());

        if (ui->isDoneBox->isChecked())
            taskModel->setData(taskModel->index(0,0), QBrush(QColor("#c6edc3")),  Qt::BackgroundRole);
        else if (!ui->isDoneBox->isChecked())
            taskModel->setData(taskModel->index(0,0), QBrush(QColor("#ecc4c6")),  Qt::BackgroundRole);

        bool fileIsReal = false;
        int fileName = 0;
        do
        {
            QFile testFile("release\\storage\\" + QString::number(fileName));
            if (testFile.open(QIODevice::ReadOnly))
            {
                fileName++;
                fileIsReal = true;
                testFile.close();
            }
            else fileIsReal = false;
        }while(fileIsReal);

        QFile file("release\\storage\\" + QString::number(fileName));
        if (file.open(QIODevice::WriteOnly))
        {
            QByteArray arr;
            arr.append(doc.toJson());
            file.write(arr);
            file.close();
        }

        clearFields();

        ui->stackedWidget->setCurrentWidget(ui->mainPage);

        delete date;
    }
}

bool Widget::checkInformation()
{
    if ((!ui->disciplineLineEdit->text().isEmpty()) && (!ui->taskLineEdit->text().isEmpty()))
        return true;
    else
    {
        QMessageBox::warning(0, "Adding a task: error", "Заполните все поля");
        return false;
    }
}

void Widget::addInfo()
{
    bool fileIsReal = false;
    int fileName = 0;
    do
    {
        QFile testFile("release\\storage\\" + QString::number(fileName));
        if (testFile.open(QIODevice::ReadOnly))
        {
            fileName++;
            fileIsReal = true;
            testFile.close();
        }
        else fileIsReal = false;
    } while(fileIsReal);
    fileName--;

    QJsonDocument doc;
    taskModel->insertRows(0,fileName+1);
    for (int i = fileName; i>-1; i--)
    {
        QFile file("release\\storage\\" + QString::number(fileName));
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray arr(file.readAll());
            doc = QJsonDocument::fromJson(arr);
            file.close();
        }
        QJsonObject object = doc.object();
        QDate deadLineDate;
        deadLineDate.setDate(object.value("deadLineYear").toInt(), object.value("deadLineMonth").toInt(),
                             object.value("deadLineDay").toInt());
        QString header(object.value("discipline").toString() + " - " + object.value("task").toString() +
                       " | " + QString::number(deadLineDate.toJulianDay() - QDate::currentDate().toJulianDay()) + " д. осталось");
        bool isDone = object.value("isDone").toBool();

        taskModel->setData(taskModel->index(taskModel->rowCount()-1-i,0),header);
        taskModel->setData(taskModel->index(taskModel->rowCount()-1-i,1), object.value("discipline").toString());
        taskModel->setData(taskModel->index(taskModel->rowCount()-1-i,2), object.value("task").toString());
        taskModel->setData(taskModel->index(taskModel->rowCount()-1-i,3), deadLineDate);
        taskModel->setData(taskModel->index(taskModel->rowCount()-1-i,4), object.value("isDone").toBool());
        if (isDone)
            taskModel->setData(taskModel->index(taskModel->rowCount()-1-i,0),QBrush(QColor("#c6edc3")), Qt::BackgroundRole);
        if (!isDone)
            taskModel->setData(taskModel->index(taskModel->rowCount()-1-i,0),QBrush(QColor("#ecc4c6")), Qt::BackgroundRole);

        fileName--;
    }
}


void Widget::on_deleteTaskButton_clicked()
{
    int fileName = taskModel->rowCount() - ui->taskListView->currentIndex().row() - 1;
    taskModel->removeRow(ui->taskListView->currentIndex().row());
    QFile deletableFile("release\\storage\\" + QString::number(fileName));
    deletableFile.remove();

    QByteArray arr;
    bool fileIsReal = false;
    fileName++;
    do
    {
        QFile testFile("release\\storage\\" + QString::number(fileName));
        if (testFile.open(QIODevice::ReadOnly))
        {
            arr = testFile.readAll();
            QFile newFile("release\\storage\\" + QString::number(fileName-1));
            if (newFile.open(QIODevice::WriteOnly))
            {
                newFile.write(arr);
                newFile.close();
            }
            fileName++;
            fileIsReal = true;
            testFile.close();
        }
        else fileIsReal = false;
        testFile.remove();
    } while(fileIsReal);
}


void Widget::on_taskListView_clicked()
{
    ui->deleteTaskButton->setEnabled(true);
    ui->editTaskButton->setEnabled(true);
    ui->isDoneButton->show();
}


void Widget::on_editTaskButton_clicked()
{
    qDebug() << ui->taskListView->currentIndex().row();
    ui->headerLabelInAddTask->setText("Изменить задачу:");
    ui->confirmButton->hide();
    ui->editButton->show();
    ui->stackedWidget->setCurrentWidget(ui->addTaskPage);
    ui->disciplineLineEdit->setText(taskModel->data(taskModel->index
                                                    (ui->taskListView->currentIndex().row(),1)).toString());
    ui->taskLineEdit->setText(taskModel->data(taskModel->index
                                                    (ui->taskListView->currentIndex().row(),2)).toString());
    ui->calendarWidget->setSelectedDate(taskModel->data(taskModel->index
                                             (ui->taskListView->currentIndex().row(),3)).toDate());
    ui->isDoneBox->setChecked(taskModel->data(taskModel->index
                                                    (ui->taskListView->currentIndex().row(),4)).toBool());
}


void Widget::on_editButton_clicked()
{
    if (checkInformation())
    {
        QDate* date = new QDate;
        QDate deadLineDate = ui->calendarWidget->selectedDate();

        QJsonObject object;
        object.insert("discipline", ui->disciplineLineEdit->text());
        object.insert("task", ui->taskLineEdit->text());

        object.insert("deadLineDay", ui->calendarWidget->selectedDate().day());
        object.insert("deadLineMonth", ui->calendarWidget->selectedDate().month());
        object.insert("deadLineYear", ui->calendarWidget->selectedDate().year());

        object.insert("additionDay", date->currentDate().day());
        object.insert("additionMonth", date->currentDate().month());
        object.insert("additionYear", date->currentDate().year());

        object.insert("isDone", ui->isDoneBox->isChecked());

        QJsonDocument doc;
        doc.setObject(object);

        QString headerString;
        headerString.append(ui->disciplineLineEdit->text() + " - " + ui->taskLineEdit->text());

        int days = ui->calendarWidget->selectedDate().toJulianDay() - date->currentDate().toJulianDay();

        taskModel->setData(taskModel->index(ui->taskListView->currentIndex().row(),0),
                           headerString + " | " + QString::number(days) + " д. осталось");

        taskModel->setData(taskModel->index(ui->taskListView->currentIndex().row(),1),
                           ui->disciplineLineEdit->text());

        taskModel->setData(taskModel->index(ui->taskListView->currentIndex().row(),2),
                           ui->taskLineEdit->text());

        taskModel->setData(taskModel->index(ui->taskListView->currentIndex().row(),3), deadLineDate);

        taskModel->setData(taskModel->index(ui->taskListView->currentIndex().row(),4),
                           ui->isDoneBox->isChecked());

        if (ui->isDoneBox->isChecked())
            taskModel->setData(taskModel->index(ui->taskListView->currentIndex().row(),0),
                               QBrush(QColor("#c6edc3")),  Qt::BackgroundRole);
        else if (!ui->isDoneBox->isChecked())
            taskModel->setData(taskModel->index(ui->taskListView->currentIndex().row(),0),
                               QBrush(QColor("#ecc4c6")),  Qt::BackgroundRole);

        QFile file("release\\storage\\" + QString::number(taskModel->rowCount() - ui->taskListView->currentIndex().row() - 1));
        file.remove();
        QFile newFile("release\\storage\\" + QString::number(taskModel->rowCount() - ui->taskListView->currentIndex().row() - 1));
        if (file.open(QIODevice::WriteOnly))
        {
            QByteArray arr;
            arr.append(doc.toJson());
            file.write(arr);
            file.close();
        }

        clearFields();

        ui->stackedWidget->setCurrentWidget(ui->mainPage);

        delete date;
    }
}


void Widget::on_isDoneButton_clicked()
{
    int fileName = taskModel->rowCount() - ui->taskListView->currentIndex().row() - 1;
    taskModel->setData(taskModel->index(ui->taskListView->currentIndex().row(),0),QBrush(QColor("#c6edc3")),  Qt::BackgroundRole);
    taskModel->setData(taskModel->index(ui->taskListView->currentIndex().row(),4), true);

    QDate* date = new QDate;
    QJsonObject object;
    object.insert("discipline", taskModel->data(taskModel->index(ui->taskListView->currentIndex().row(),1)).toString());
    object.insert("task", taskModel->data(taskModel->index(ui->taskListView->currentIndex().row(),2)).toString());

    object.insert("deadLineDay", taskModel->data(taskModel->index(ui->taskListView->currentIndex().row(),3)).toDate().day());
    object.insert("deadLineMonth", taskModel->data(taskModel->index(ui->taskListView->currentIndex().row(),3)).toDate().month());
    object.insert("deadLineYear", taskModel->data(taskModel->index(ui->taskListView->currentIndex().row(),3)).toDate().year());

    object.insert("additionDay", date->currentDate().day());
    object.insert("additionMonth", date->currentDate().month());
    object.insert("additionYear", date->currentDate().year());

    object.insert("isDone", taskModel->data(taskModel->index(ui->taskListView->currentIndex().row(),4)).toBool());

    QJsonDocument doc;
    doc.setObject(object);

    QFile oldFile("release\\storage\\" + QString::number(fileName));
    oldFile.remove();
    QFile file("release\\storage\\" + QString::number(fileName));
    if (file.open(QIODevice::WriteOnly))
    {
        QByteArray arr = doc.toJson();
        file.write(arr);
        file.close();
    }

    delete date;
}


void Widget::on_backButton_clicked()
{
    clearFields();
    ui->stackedWidget->setCurrentWidget(ui->mainPage);
}

void Widget::clearFields()
{
    ui->disciplineLineEdit->clear();
    ui->taskLineEdit->clear();
    ui->calendarWidget->clearFocus();
    ui->isDoneBox->setChecked(false);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    ui->isDoneButton->hide();
    ui->deleteTaskButton->setEnabled(false);
    ui->editTaskButton->setEnabled(false);
    ui->taskListView->clearFocus();
}


