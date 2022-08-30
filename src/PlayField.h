/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "LevelMap.h"
#include "PathFinder.h"

#include <KGameRenderer>

#include <QBrush>
#include <QCursor>
#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QGraphicsScene>

class ImageData;
class MoveSequence;
class Move;
class LevelMap;

class History;
class Bookmark;
class LevelCollection;

class KGamePopupItem;

class PlayField : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit PlayField(QObject *parent);
    ~PlayField() override;

public:
    int animDelay() const
    {
        return animDelay_;
    }

    void setBookmark(Bookmark *bm);
    void goToBookmark(Bookmark *bm);

    void changeAnim(int num);
    void changeCollection(LevelCollection *collection);

    void setSize(int w, int h);

    void showMessage(const QString &message);

public Q_SLOTS:
    void nextLevel();
    void previousLevel();
    void undo();
    void redo();
    void restartLevel();

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void keyPressEvent(QKeyEvent *) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *e) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *) override;
    void wheelEvent(QGraphicsSceneWheelEvent *) override;
    void timerEvent(QTimerEvent *) override;

private:
    void updateBackground();

    bool canMoveNow();
    void level(int _l)
    {
        levelMap_->level(_l);
    }
    LevelCollection *collection() const
    {
        return levelMap_->collection();
    }

    int level() const;
    const QString &collectionName() const;
    int totalMoves() const;
    int totalPushes() const;

    void updateCollectionXpm();
    void updateTextXpm();
    void updateLevelXpm();
    void updateStepsXpm();
    void updatePushesXpm();

    void levelChange();
    void paintSquare(int x, int y, QPainter &paint);
    void paintPainter(QPainter &paint, const QRect &rect);
    void step(int _x, int _y);
    void push(int _x, int _y);
    void stopDrag();
    void dragObject(int xpixel, int ypixel);
    void highlight();
    void changeCursor(const QCursor *c);

    int x2pixel(int x) const
    {
        return size_ * x + xOffs_;
    }
    int y2pixel(int y) const
    {
        return size_ * y + yOffs_;
    }

    int pixel2x(int x) const
    {
        return (x - xOffs_) / size_;
    }
    int pixel2y(int y) const
    {
        return (y - yOffs_) / size_;
    }

    void startMoving(Move *m);
    void startMoving(MoveSequence *ms);
    void stopMoving();

    void killTimers();

private:
    ImageData *imageData_ = nullptr;
    LevelMap *levelMap_;
    History *history_;
    int lastLevel_ = -1;
    MoveSequence *moveSequence_ = nullptr;
    bool moveInProgress_ = false;
    bool dragInProgress_ = false;
    PathFinder pathFinder_;
    int animDelay_;
    const QCursor *cursor_;

    int size_;
    int xOffs_ = 0;
    int yOffs_ = 0;
    int highlightX_;
    int highlightY_;
    qreal dragX_;
    qreal dragY_;
    qreal lastMouseXPos_;
    qreal lastMouseYPos_;
    qreal mousePosX_;
    qreal mousePosY_;
    int wheelDelta_ = 0;

    QList<int> timers;
    QCursor crossCursor;

    KGamePopupItem *m_messageItem;
    KGameRenderer m_renderer;

    QRect pnumRect_;
    QRect ptxtRect_;
    QRect snumRect_;
    QRect stxtRect_;
    QRect lnumRect_;
    QRect ltxtRect_;
    QRect collRect_;

    const QString levelText_;
    const QString stepsText_;
    const QString  pushesText_;
    QPixmap pnumXpm_;
    QPixmap ptxtXpm_;
    QPixmap snumXpm_;
    QPixmap stxtXpm_;
    QPixmap lnumXpm_;
    QPixmap ltxtXpm_;
    QPixmap collXpm_;
    QFont statusFont_;
    QFontMetrics statusMetrics_;
};

#endif /* PLAYFIELD_H */
