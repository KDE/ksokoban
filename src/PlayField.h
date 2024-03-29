/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "LevelMap.h"
#include "PathFinder.h"

#include <KGameRenderer>
#include <KGamePopupItem>

#include <QCursor>
#include <QFont>
#include <QFontMetrics>
#include <QGraphicsScene>

class MoveSequence;
class Move;
class LevelMap;

class History;
class Bookmark;
class LevelCollection;
class GroundItem;
class SimpleTextItem;

class KGamePopupItem;

class QGraphicsWidget;

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

    template<class Receiver, class Func>
    inline typename std::enable_if<!std::is_convertible<Func, const char *>::value, void>::type
        showMessage(const QString &message, const Receiver *recvr, Func slot)
    {
        // enforce emission of any hidden signal
        m_messageItem->forceHide(KGamePopupItem::InstantHide);

        if (m_messageHiddenConnecttion) {
            QObject::disconnect(m_messageHiddenConnecttion);
        }
        m_messageHiddenConnecttion = connect(m_messageItem, &KGamePopupItem::hidden, recvr, slot);

        m_messageItem->showMessage(message, KGamePopupItem::Center, KGamePopupItem::ReplacePrevious);

    }

public Q_SLOTS:
    void nextLevel();
    void previousLevel();
    void undo();
    void redo();
    void restartLevel();

protected:
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

    void updateStepsDisplay();
    void updatePushesDisplay();

    void levelChange();
    void step(int _x, int _y);
    void push(int _x, int _y);
    void stopDrag();
    void dragObject(int xpixel, int ypixel);
    void highlight();
    void changeCursor(const QCursor *c);

    void startMoving(Move *m);
    void startMoving(MoveSequence *ms);
    void stopMoving();

    void killTimers();

private:
    LevelMap *levelMap_;
    History *history_;
    int lastLevel_ = -1;
    MoveSequence *moveSequence_ = nullptr;
    bool moveInProgress_ = false;
    bool dragInProgress_ = false;
    PathFinder pathFinder_;
    int animDelay_;
    const QCursor *cursor_;

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
    QGraphicsWidget *m_mainWidget;
    GroundItem *m_groundItem;
    SimpleTextItem *m_collectionName;
    SimpleTextItem *m_levelLabel;
    SimpleTextItem *m_levelNumber;
    SimpleTextItem *m_stepsLabel;
    SimpleTextItem *m_stepsNumber;
    SimpleTextItem *m_pushesLabel;
    SimpleTextItem *m_pushesNumber;

    QFont statusFont_;
    QFontMetrics statusMetrics_;

    QMetaObject::Connection m_messageHiddenConnecttion;
};

#endif /* PLAYFIELD_H */
