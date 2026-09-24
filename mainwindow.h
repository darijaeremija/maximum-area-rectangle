#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPoint>
#include <QRectF>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;
    QVector<QPointF> _points;

private slots:
    void on_pbRandomPoints_clicked();
    void on_pbLoadFromFile_clicked();
    void on_pbStartAlgorithm_clicked();

};

#endif // MAINWINDOW_H
