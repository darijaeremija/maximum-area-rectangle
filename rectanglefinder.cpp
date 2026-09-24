#include "rectanglefinder.h"
#include <algorithm>
#include <QThread>
#include <QCoreApplication>
#include <QDebug>

RectangleFinder::RectangleFinder(const QVector<QPointF> &points, int pause, QObject *parent)
    : QObject(parent), _pause(pause), _points(points), _sweepLineY(-1)
{
}

RectangleFinder::~RectangleFinder() {
    clearData();
}

void RectangleFinder::clearData() {
    _rectangles.clear();
    _maxRectangle = QRectF();
    _sweepLineY = -1;
    freeAllNodes();
}

void RectangleFinder::freeAllNodes() {
    for (Node *node : _nodes) {
        delete node;
    }
    _nodes.clear();
}

int RectangleFinder::sweepLineY() const
{
    return _sweepLineY;
}

const QRectF &RectangleFinder::maxRectangle() const
{
    return _maxRectangle;
}


RectangleFinder::Node* RectangleFinder::insert(Node *prev, int low, int high, int compX) {

    Node *curr = new Node();

    if(prev) {
        *curr = *prev;
    }

    _nodes.push_back(curr);

    if (low == high){
        curr->val++;
        return curr;
    }

    int mid = (low + high) / 2;

    if (compX <= mid)
        curr->left = insert(curr->left, low, mid, compX);
    else
        curr->right = insert(curr->right, mid + 1, high, compX);

    curr->val++;
    return curr;
}

int RectangleFinder::query(Node *n, int low, int high, int ql, int qr) const {
    if (!n || ql > high || qr < low) {
        return 0;
    }
    if (ql <= low && high <= qr) {
        return n->val;
    }

    int mid = (low + high) / 2;
    int p1 = query(n->left, low, mid, ql, qr);
    int p2 = query(n->right, mid + 1, high, ql, qr);
    return p1 + p2;
}

void RectangleFinder::startAlgorithm()
{
    qDebug() << "Algoritam je pokrenut!";

    if (_points.size() < 4) return;

    std::map<int, std::vector<int>> events;
    std::vector<int> xs_all;

    // grupisanje po y
    for (const auto& p : _points) {
        int px = static_cast<int>(p.x());
        int py = static_cast<int>(p.y());
        events[py].push_back(px);
        xs_all.push_back(px);
    }

    // sortiranje x vrednosti i uklanjanje duplikata
    for (auto& [y, xs] : events) {
        std::sort(xs.begin(), xs.end());
        xs.erase(std::unique(xs.begin(), xs.end()), xs.end());
    }

    // kompresija x koordinata
    std::sort(xs_all.begin(), xs_all.end());
    xs_all.erase(std::unique(xs_all.begin(), xs_all.end()), xs_all.end());

    std::unordered_map<int, int> compX;
    for (int i = 0; i < static_cast<int>(xs_all.size()); i++) {
        compX[xs_all[i]] = i;
    }

    int m = static_cast<int>(xs_all.size());
    if (m < 2) return;

    std::map<std::pair<int, int>, int> seg;
    std::unordered_map<int, int> lastY_at_x;
    std::unordered_map<int, Node*> rootAtY;

    Node *currentRoot = nullptr;
    long long maxArea = -1;

    for (const auto& [y, xs] : events) {
        _sweepLineY = y;
        emit sweepLineChanged(_sweepLineY);

        if (_pause > 0) {
            QThread::msleep(_pause);
            QCoreApplication::processEvents();
        }

        //stanje stabla pre dodavanja tačaka sa tekućeg nivoa y (sve tacke ispod tekuceg y)
        Node *rootBelowCurrentY = currentRoot;

        for (int i = 0; i < static_cast<int>(xs.size()) - 1; i++) {
            int x1 = xs[i];
            int x2 = xs[i + 1];

            auto itSeg = seg.find({x1, x2});
            if (itSeg != seg.end()) {
                int y_bottom = itSeg->second;

                // nema medjutačaka na vertikalnim ivicama
                if (lastY_at_x[x1] == y_bottom && lastY_at_x[x2] == y_bottom) {

                    int left = compX[x1] + 1;
                    int right = compX[x2] - 1;
                    int insideCount = 0;

                    // ako ima kompresovanih kolona između x1 i x2 proveravamo
                    if (left <= right) {
                        Node *rootAtBottom = rootAtY[y_bottom];

                        int countTop = query(rootBelowCurrentY, 0, m - 1, left, right);
                        int countBottom = query(rootAtBottom, 0, m - 1, left, right);
                        insideCount = countTop - countBottom;
                    }

                    if (insideCount == 0) {
                        QRectF newRect(QPointF(x1, y_bottom), QPointF(x2, y));
                        _rectangles.push_back(newRect);
                        emit rectangleFound(newRect);

                        long long tmpArea = 1LL * (x2 - x1) * (y - y_bottom);
                        if (tmpArea > maxArea) {
                            maxArea = tmpArea;
                            _maxRectangle = newRect;
                            emit maxRectangleChanged(_maxRectangle);

                            if (_pause > 0) {
                                QThread::msleep(_pause);
                                QCoreApplication::processEvents();
                            }
                        }


                    }
                }
            }
        }

        // azuriramo vertikalne ivice
        for (int xval : xs) {
            lastY_at_x[xval] = y;
        }

        // azuriramo segmente
        for (int i = 0; i < static_cast<int>(xs.size()) - 1; i++) {
            seg[{xs[i], xs[i + 1]}] = y;
        }

        // ubacujemo tacke sa tekuceg y u stablo (nova verzija)
        for (int xval : xs) {
            currentRoot = insert(currentRoot, 0, m - 1, compX[xval]);
        }

        // pamtimo verziju stabla za visinu y
        rootAtY[y] = currentRoot;
    }

    _sweepLineY = -1;
    emit sweepLineChanged(_sweepLineY);

    qDebug() << "Kraj. Ukupno pravougaonika: " << _rectangles.size();
}


