#include "canvas.h"
#include <QPainter>
#include <QDebug>

Canvas::Canvas(QWidget *parent) : QWidget(parent), _sweepLine(-1), _minX(0), _maxX(1), _minY(0), _maxY(1), _dataW(1), _dataH(1)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

void Canvas::setPoints(const QVector<QPointF> &points)
{
    _points = points;

    if(!_points.empty()){
        _minX = _points[0].x();
        _maxX = _points[0].x();
        _minY = _points[0].y();
        _maxY = _points[0].y();

        for (const auto &p : _points) {
            if (p.x() < _minX) _minX = p.x();
            if (p.x() > _maxX) _maxX = p.x();
            if (p.y() < _minY) _minY = p.y();
            if (p.y() > _maxY) _maxY = p.y();
        }

        _dataW = _maxX - _minX;
        _dataH = _maxY - _minY;
        if (_dataW == 0) _dataW = 1;
        if (_dataH == 0) _dataH = 1;
    }

    update();
}

void Canvas::setSweepLine(int y)
{
    _sweepLine = y;
    update();
}

void Canvas::setMaxRectangle(const QRectF &rect)
{
    _maxRect = rect;
    update();
}

void Canvas::setRectangles(const QVector<QRectF> &rectangles)
{
    _rectangles = rectangles;
    update();
}

void Canvas::clearCanvas()
{
    _rectangles.clear();
    _sweepLine = -1;
    _points.clear();
    _maxRect = QRectF();
    update();
}

void Canvas::onRectangleFound(const QRectF &rect)
{
    _rectangles.append(rect);
    update();
}
void Canvas::onMaxRectangleChanged(const QRectF &maxRect){
    _maxRect = maxRect;
    update();
}

void Canvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (_points.isEmpty())
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);

    int margin = 20;
    double availWidth = width() - 2 * margin;
    double availHeight = height() - 2 * margin;

    double scaleX = availWidth / _dataW;
    double scaleY = availHeight / _dataH;
    double scale = qMin(scaleX, scaleY);

    double xOffset = margin + (availWidth - _dataW * scale) / 2.0;
    double yOffset = margin + (availHeight - _dataH * scale) / 2.0;

    painter.translate(xOffset - _minX * scale, height() - yOffset + _minY * scale);
    painter.scale(scale, -scale); // Invertujemo y da 0 bude dole


    // pravougaonici
    QPen redPen(Qt::red, 1.5);
    redPen.setCosmetic(true);
    painter.setPen(redPen);
    painter.setBrush(QColor(255, 0, 0, 35));
    for (const auto &r : _rectangles) {
        painter.drawRect(r.normalized());
    }

    //najveci pravougaonik
    if (_maxRect.isValid()) {
        QPen bluePen(Qt::blue, 2.0);
        bluePen.setCosmetic(true);
        painter.setPen(bluePen);
        painter.setBrush(QColor(0, 0, 255, 35));
        painter.drawRect(_maxRect.normalized());
    }


    // brišuća prava
    if (_sweepLine >= 0) {
        QPen sweepPen(Qt::darkRed, 1.5, Qt::SolidLine);
        sweepPen.setCosmetic(true);
        painter.setPen(sweepPen);
        painter.drawLine(QPointF(_minX - _dataW, _sweepLine),QPointF(_maxX + _dataW, _sweepLine));
    }

    // tačke
    QTransform currentTransform = painter.transform();
    painter.resetTransform();

    for (const auto &p : _points) {
        QPointF screenPt = currentTransform.map(p);

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::black);
        painter.drawEllipse(screenPt, 3.5, 3.5);

        // Ispis koordinata
        painter.setPen(Qt::darkGray);
        QString label = QString("(%1, %2)").arg(p.x()).arg(p.y());
        painter.drawText(screenPt + QPointF(7, -5), label);
    }
}

