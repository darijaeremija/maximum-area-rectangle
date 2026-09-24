#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QRectF>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = nullptr);
    void clearCanvas();

    void setPoints(const QVector<QPointF> &points);
    void setRectangles(const QVector<QRectF> &rectangles);
    void setMaxRectangle(const QRectF &rect);

public slots:
    void setSweepLine(int y);
    void onRectangleFound(const QRectF &rect);
    void onMaxRectangleChanged(const QRectF &maxRect);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QPointF> _points;
    int _sweepLine;
    QVector<QRectF> _rectangles;
    QRectF _maxRect;
    double _minX, _maxX, _minY, _maxY;
    double _dataW, _dataH;
};

#endif // CANVAS_H
