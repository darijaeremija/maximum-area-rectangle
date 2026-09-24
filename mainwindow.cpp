#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rectanglefinder.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1000, 800);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbRandomPoints_clicked()
{
    ui->canvas->clearCanvas();
    _points.clear();

    const int numPoints = 12;
    const int minCoord = 1;
    const int maxCoord = 7;

    auto *rng = QRandomGenerator::global();

    for(int i =0 ; i < numPoints; i++){
        int x = rng->bounded(minCoord,maxCoord);
        int y = rng->bounded(minCoord, maxCoord);
        QPointF newPoint(x, y);

        if (!_points.contains(newPoint)) {
            _points.append(newPoint);
        }
    }

    ui->canvas->setPoints(_points);
}

void MainWindow::on_pbLoadFromFile_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, "Choose file", QDir::currentPath());

    if (fileName.isEmpty())
        return;

    ui->canvas->clearCanvas();
    _points.clear();

    QFile inputFile(fileName);

    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Can't open file.");
        return;
    }

    //parsiranje linija
    QTextStream fileStream(&inputFile);

    while(!fileStream.atEnd()){
        QString currentLine = fileStream.readLine().trimmed();
        if (currentLine.isEmpty())
            continue;

        QStringList coords = currentLine.split(QRegExp("\\s+"), Qt::SkipEmptyParts);

        if (coords.size() >= 2){
            bool parsedX = false;
            bool parsedY = false;

            int x = coords[0].toInt(&parsedX);
            int y = coords[1].toInt(&parsedY);

            if(parsedX && parsedY){
                _points.append(QPointF(x,y));
            }
        }
    }

    inputFile.close();

    if (_points.isEmpty()) {
            QMessageBox::warning(this, "Warning", "No valid points found in file.");
            return;
        }

    ui->canvas->setPoints(_points);
}

void MainWindow::on_pbStartAlgorithm_clicked()
{
    if (_points.size() < 4) {
        QMessageBox::warning(this, "Warning", "At least 4 points are required to form a rectangle.");
        return;
    }
    ui->pbStartAlgorithm->setEnabled(false);
    ui->pbLoadFromFile->setEnabled(false);
    ui->pbRandomPoints->setEnabled(false);
    ui->statusbar->clearMessage();

    ui->canvas->clearCanvas();
    ui->canvas->setPoints(_points);

    RectangleFinder *solution = new RectangleFinder(_points, 500, this); // 500ms pauza

    connect(solution, &RectangleFinder::sweepLineChanged, ui->canvas, &Canvas::setSweepLine);
    connect(solution, &RectangleFinder::rectangleFound, ui->canvas, &Canvas::onRectangleFound);
    connect(solution, &RectangleFinder::maxRectangleChanged, ui->canvas, &Canvas::onMaxRectangleChanged);

    solution->startAlgorithm();

    QRectF maxR = solution->maxRectangle().normalized();

    if (maxR.isValid() && maxR.width() > 0 && maxR.height() > 0) {
        long long width = static_cast<long long>(maxR.width());
        long long height = static_cast<long long>(maxR.height());
        long long area = width * height;
        ui->statusbar->showMessage(QString("Maximum area: %1").arg(area));
    }
    else {
        ui->statusbar->showMessage("No valid rectangle found (-1).");
    }

    ui->pbStartAlgorithm->setEnabled(true);
    ui->pbLoadFromFile->setEnabled(true);
    ui->pbRandomPoints->setEnabled(true);

    solution->deleteLater();
}
