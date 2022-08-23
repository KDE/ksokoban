/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "ImageData.h"
#include "LevelMap.h"
#include "PathFinder.h"

#include <QBrush>
#include <QFont>
#include <QFontMetrics>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QWidget>

class MapDelta;
class MoveSequence;
class Move;

class History;
class Bookmark;
class LevelCollection;
class QPainter;
class QCursor;

class PlayField : public QWidget
{
    Q_OBJECT

public:
    explicit PlayField(QWidget *parent);
    ~PlayField() override;

public:
    bool canMoveNow();
    int animDelay() const
    {
        return animDelay_;
    }

    void setSize(int w, int h);
    void level(int _l)
    {
        levelMap_->level(_l);
    }
    LevelCollection *collection() const
    {
        return levelMap_->collection();
    }
    void setBookmark(Bookmark *bm);
    void goToBookmark(Bookmark *bm);

    int level() const;
    const QString &collectionName() const;
    int totalMoves() const;
    int totalPushes() const;

    void updateCollectionXpm();
    void updateTextXpm();
    void updateLevelXpm();
    void updateStepsXpm();
    void updatePushesXpm();

public Q_SLOTS:
    void nextLevel();
    void previousLevel();
    void undo();
    void redo();
    void restartLevel();
    void changeCollection(LevelCollection *collection);
    void changeAnim(int num);

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void focusInEvent(QFocusEvent *) override;
    void focusOutEvent(QFocusEvent *) override;
    void keyPressEvent(QKeyEvent *) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void leaveEvent(QEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void wheelEvent(QWheelEvent *) override;
    void timerEvent(QTimerEvent *) override;

protected:
    void levelChange();
    void paintSquare(int x, int y, QPainter &paint);
    void paintDelta();
    void paintPainterClip(QPainter &paint, int x, int y, int w, int h);
    void paintPainter(QPainter &paint, const QRect &rect);
    void step(int _x, int _y);
    void push(int _x, int _y);
    void stopDrag();
    void dragObject(int xpixel, int ypixel);
    void highlight();
    void changeCursor(const QCursor *c);

private:
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
    MapDelta *mapDelta_;
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
    int dragX_;
    int dragY_;
    int lastMouseXPos_;
    int lastMouseYPos_;
    int mousePosX_;
    int mousePosY_;
    int wheelDelta_ = 0;
    int debug_counter = 0;

    QList<int> timers;
    QCursor crossCursor;

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
    QBrush background_;
};

#endif /* PLAYFIELD_H */
