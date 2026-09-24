#ifndef RECTANGLEFINDER_H
#define RECTANGLEFINDER_H

#include <vector>
#include <map>
#include <unordered_map>
#include <QVector>
#include <QPointF>
#include <QRectF>
#include <QObject>

class RectangleFinder : public QObject
{
    Q_OBJECT
public:
    RectangleFinder(const QVector<QPointF> &points, int pauseTime, QObject *parent = nullptr);
    ~RectangleFinder();

    QVector<QRectF> getRectangles() const { return _rectangles; }
    const QRectF &maxRectangle() const;

    int sweepLineY() const;

    void clearData();

public slots:
    void startAlgorithm();

signals:
    void sweepLineChanged(int y);
    void rectangleFound(const QRectF &rect);
    void maxRectangleChanged(const QRectF &currentMax);

private:

    struct Node {
        int val;
        Node *left;
        Node *right;

        Node(int v = 0, Node *l = nullptr, Node *r = nullptr)
            : val(v), left(l), right(r) {}

    };

    Node *insert(Node *prev, int low, int high, int xVal);
    int query(Node *n, int low, int high, int ql, int qr) const;
    void freeAllNodes();


    int _pause;
    QVector<QPointF> _points;
    QVector<QRectF> _rectangles;
    QRectF _maxRectangle;
    int _sweepLineY;    

    std::vector<Node*> _nodes;
};

#endif // RECTANGLEFINDER_H
