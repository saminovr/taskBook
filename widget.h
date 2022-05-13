#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QMessageBox>
#include <QMouseEvent>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:


    void on_addTaskButton_clicked();

    void on_confirmButton_clicked();

    void on_deleteTaskButton_clicked();

    void on_taskListView_clicked();

    void on_editTaskButton_clicked();

    void on_editButton_clicked();

    void on_isDoneButton_clicked();

    void on_backButton_clicked();
private:
    Ui::Widget *ui;

    QStandardItemModel* taskModel;

    bool checkInformation();

    void addInfo();

    void clearFields();

protected:
    void mousePressEvent(QMouseEvent* event) override;

};
#endif // WIDGET_H
