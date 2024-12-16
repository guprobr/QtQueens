#ifndef QUEEN_H
#define QUEEN_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QGraphicsSceneMouseEvent>

class Queen : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Queen(QGraphicsItem *parent = nullptr);


    // Methods to set/get logical positions
    void setPosition(int row, int col);
    int row() const;
    int col() const;

protected:
    // Events for dragging the queen
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    int m_row;
    int m_col;

    bool beingDragged; // State to track dragging
};

#endif // QUEEN_H
