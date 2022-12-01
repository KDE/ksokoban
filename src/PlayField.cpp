/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "PlayField.h"

#include "Bookmark.h"
#include "GroundItem.h"
#include "History.h"
#include "HtmlPrinter.h"
#include "LevelCollection.h"
#include "LevelMap.h"
#include "Move.h"
#include "MoveSequence.h"
#include "PathFinder.h"
#include "PlayField.h"

#include <KGamePopupItem>
#include <KgTheme>

#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsSimpleTextItem>
#include <QFontDatabase>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QPainter>
#include <QPixmap>
#include <QStandardPaths>

#include <cassert>
#include <cstdio>

static KgTheme* createClassicTheme()
{
    auto *theme = new KgTheme("themes/ksokoban_classic.desktop");
    theme->setGraphicsPath(QStandardPaths::locate(QStandardPaths::AppDataLocation, QStringLiteral("themes/ksokoban_classic.svg")));
    return theme;
}

PlayField::PlayField(QObject *parent)
    : QGraphicsScene(parent)
    , crossCursor(Qt::CrossCursor)
    , m_renderer(createClassicTheme())
    , statusFont_(QFontDatabase::systemFont(QFontDatabase::GeneralFont).family(), 18, QFont::Bold)
    , statusMetrics_(statusFont_)
{
    highlightX_ = highlightY_ = 0;

    KSharedConfigPtr cfg = KSharedConfig::openConfig();
    KConfigGroup settingsGroup(cfg, "settings");

    animDelay_ = settingsGroup.readEntry("animDelay", QStringLiteral("2")).toInt();
    if (animDelay_ < 0 || animDelay_ > 3)
        animDelay_ = 2;

    history_ = new History;

    levelMap_ = new LevelMap;

    m_groundItem = new GroundItem(&levelMap_->map(), &m_renderer);
    addItem(m_groundItem);

    m_collectionName = new QGraphicsSimpleTextItem();
    m_collectionName->setBrush(QColor(0, 255, 0));
    m_collectionName->setFont(statusFont_);
    addItem(m_collectionName);

    m_levelLabel = new QGraphicsSimpleTextItem(i18n("Level:"));
    m_levelLabel->setFont(statusFont_);
    m_levelLabel->setBrush(QColor(128, 128, 128));
    m_levelNumber = new QGraphicsSimpleTextItem();
    m_levelNumber->setFont(statusFont_);
    m_levelNumber->setBrush(QColor(255, 0, 0));
    addItem(m_levelLabel);
    addItem(m_levelNumber);

    m_stepsLabel = new QGraphicsSimpleTextItem(i18n("Steps:"));
    m_stepsLabel->setFont(statusFont_);
    m_stepsLabel->setBrush(QColor(128, 128, 128));
    m_stepsNumber = new QGraphicsSimpleTextItem();
    m_stepsNumber->setFont(statusFont_);
    m_stepsNumber->setBrush(QColor(255, 0, 0));
    addItem(m_stepsLabel);
    addItem(m_stepsNumber);

    m_pushesLabel = new QGraphicsSimpleTextItem(i18n("Pushes:"));
    m_pushesLabel->setFont(statusFont_);
    m_pushesLabel->setBrush(QColor(128, 128, 128));
    m_pushesNumber = new QGraphicsSimpleTextItem();
    m_pushesNumber->setFont(statusFont_);
    m_pushesNumber->setBrush(QColor(255, 0, 0));
    addItem(m_pushesLabel);
    addItem(m_pushesNumber);

    levelChange();

    m_messageItem = new KGamePopupItem();
    addItem(m_messageItem);

    updateBackground();
}

PlayField::~PlayField()
{
    KSharedConfigPtr cfg = KSharedConfig::openConfig();
    KConfigGroup settingsGroup(cfg, "settings");
    settingsGroup.writeEntry("animDelay", QStringLiteral("%1").arg(animDelay_));

    delete history_;
    delete levelMap_;
}

void PlayField::updateBackground()
{
    const QString backgroundId = QStringLiteral("background");
    const QSize backgroundSize = m_renderer.boundsOnSprite(backgroundId).size().toSize();
    setBackgroundBrush(m_renderer.spritePixmap(backgroundId, backgroundSize));
//     setBackgroundBrush(m_renderer.spritePixmap(backgroundId, sceneRect().size()));
}

void PlayField::showMessage(const QString &message)
{
    m_messageItem->setMessageTimeout(4000);
    m_messageItem->showMessage(message, KGamePopupItem::Center);
}

void PlayField::changeCursor(const QCursor *c)
{
    if (cursor_ == c)
        return;

    cursor_ = c;
#if 0
    if (c == nullptr)
        unsetCursor();
    else
        setCursor(*c);
#endif
}

int PlayField::level() const
{
    if (levelMap_ == nullptr)
        return 0;
    return levelMap_->level();
}

const QString &PlayField::collectionName() const
{
    static QString error = QStringLiteral("????");
    if (levelMap_ == nullptr)
        return error;
    return levelMap_->collectionName();
}

int PlayField::totalMoves() const
{
    if (levelMap_ == nullptr)
        return 0;
    return levelMap_->totalMoves();
}

int PlayField::totalPushes() const
{
    if (levelMap_ == nullptr)
        return 0;
    return levelMap_->totalPushes();
}

void PlayField::levelChange()
{
    stopMoving();
    stopDrag();
    history_->clear();

    m_levelNumber->setText(QString::asprintf("%05d", level() + 1));
    updateStepsDisplay();
    updatePushesDisplay();

    setSize(width(), height());
    highlight();
}

void PlayField::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
    lastMouseXPos_ = e->scenePos().x();
    lastMouseYPos_ = e->scenePos().y();

    if (!dragInProgress_)
        return highlight();

    int old_x = dragX_, old_y = dragY_;

    dragX_ = lastMouseXPos_ - mousePosX_;
    dragY_ = lastMouseYPos_ - mousePosY_;

    const int suqareSize = m_groundItem->squareSize();
    {
        // TODO: this logic seems broken, dragx/Y is relative
        const QPoint square = m_groundItem->squareFromScene({dragX_ + suqareSize / 2, dragY_ + suqareSize / 2});
        int x = square.x();
        int y = square.y();
        if (levelMap_->map().hasCoord(x, y) && pathFinder_.canDragTo(x, y)) {
            const QPointF dragScenePos = m_groundItem->squareToScene({x, y});
            const qreal dragX =  dragScenePos.x();
            const qreal dragY =  dragScenePos.y();

            if (dragX_ >= dragX - suqareSize / 4 && dragX_ < dragX + suqareSize / 4 &&
                dragY_ >= dragY - suqareSize / 4 && dragY_ < dragY + suqareSize / 4) {
                dragX_ = dragX;
                dragY_ = dragY;
            }
        }
    }

    if (dragX_ == old_x && dragY_ == old_y)
        return;

    update();
}

void PlayField::highlight()
{
    // FIXME: the line below should not be needed
    if (m_groundItem->squareSize() == 0)
        return;

    const QPoint square = m_groundItem->squareFromScene({lastMouseXPos_, lastMouseYPos_});
    const int x = square.x();
    const int y = square.y();

    if (!levelMap_->map().hasCoord(x, y))
        return;

    if (x == highlightX_ && y == highlightY_)
        return;

    if (pathFinder_.canDrag(x, y)) {
        highlightX_ = x;
        highlightY_ = y;
        update();
    } else {
        if (pathFinder_.canWalkTo(x, y))
            changeCursor(&crossCursor);
        else
            changeCursor(nullptr);
        if (highlightX_ >= 0) {
            update();
        }
    }
}

void PlayField::stopMoving()
{
    killTimers();
    delete moveSequence_;
    moveSequence_ = nullptr;
    moveInProgress_ = false;
    updateStepsDisplay();
    updatePushesDisplay();

    update();
    pathFinder_.updatePossibleMoves();
}

void PlayField::startMoving(Move *m)
{
    startMoving(new MoveSequence(m, levelMap_));
}

void PlayField::startMoving(MoveSequence *ms)
{
    static const int delay[4] = {0, 15, 35, 60};

    assert(moveSequence_ == nullptr && !moveInProgress_);
    moveSequence_ = ms;
    moveInProgress_ = true;
    if (animDelay_)
        timers.append(startTimer(delay[animDelay_]));
    timerEvent(nullptr);
}

void PlayField::timerEvent(QTimerEvent *)
{
    assert(moveInProgress_);
    if (moveSequence_ == nullptr) {
        killTimers();
        moveInProgress_ = false;
        return;
    }

    bool more = false;

    if (animDelay_)
        more = moveSequence_->next();
    else {
        while (moveSequence_->next())
            if (levelMap_->completed())
                break;
        more = true; // FIXME: clean this up
        stopMoving();
    }

    if (more) {
        update();
        if (levelMap_->completed()) {
            stopMoving();
            showMessage(i18n("Level completed"));
            nextLevel();
            return;
        }
    } else
        stopMoving();
}

void PlayField::step(int _x, int _y)
{
    if (!canMoveNow())
        return;

    int oldX = levelMap_->xpos();
    int oldY = levelMap_->ypos();
    int x = oldX, y = oldY;

    int dx = 0, dy = 0;
    if (_x > oldX)
        dx = 1;
    if (_x < oldX)
        dx = -1;
    if (_y > oldY)
        dy = 1;
    if (_y < oldY)
        dy = -1;

    while (!(x == _x && y == _y) && levelMap_->step(x + dx, y + dy)) {
        x += dx;
        y += dy;
    }

    if (x != oldX || y != oldY) {
        auto *m = new Move(oldX, oldY);
        m->step(x, y);
        m->finish();
        history_->add(m);
        m->undo(levelMap_);

        startMoving(m);
    }
}

void PlayField::push(int _x, int _y)
{
    if (!canMoveNow())
        return;

    int oldX = levelMap_->xpos();
    int oldY = levelMap_->ypos();
    int x = oldX, y = oldY;

    int dx = 0, dy = 0;
    if (_x > oldX)
        dx = 1;
    if (_x < oldX)
        dx = -1;
    if (_y > oldY)
        dy = 1;
    if (_y < oldY)
        dy = -1;

    while (!(x == _x && y == _y) && levelMap_->step(x + dx, y + dy)) {
        x += dx;
        y += dy;
    }
    int objX = x, objY = y;
    while (!(x == _x && y == _y) && levelMap_->push(x + dx, y + dy)) {
        x += dx;
        y += dy;
    }

    if (x != oldX || y != oldY) {
        auto *m = new Move(oldX, oldY);

        if (objX != oldX || objY != oldY)
            m->step(objX, objY);

        if (objX != x || objY != y) {
            m->push(x, y);

            objX += dx;
            objY += dy;
        }
        m->finish();
        history_->add(m);

        m->undo(levelMap_);

        startMoving(m);
    }
}

void PlayField::keyPressEvent(QKeyEvent *e)
{
    int x = levelMap_->xpos();
    int y = levelMap_->ypos();

    switch (e->key()) {
    case Qt::Key_Up:
        if (e->modifiers() & Qt::ControlModifier)
            step(x, 0);
        else if (e->modifiers() & Qt::ShiftModifier)
            push(x, 0);
        else
            push(x, y - 1);
        break;
    case Qt::Key_Down:
        if (e->modifiers() & Qt::ControlModifier)
            step(x, Map::MAX_Y);
        else if (e->modifiers() & Qt::ShiftModifier)
            push(x, Map::MAX_Y);
        else
            push(x, y + 1);
        break;
    case Qt::Key_Left:
        if (e->modifiers() & Qt::ControlModifier)
            step(0, y);
        else if (e->modifiers() & Qt::ShiftModifier)
            push(0, y);
        else
            push(x - 1, y);
        break;
    case Qt::Key_Right:
        if (e->modifiers() & Qt::ControlModifier)
            step(Map::MAX_X, y);
        else if (e->modifiers() & Qt::ShiftModifier)
            push(Map::MAX_X, y);
        else
            push(x + 1, y);
        break;

    case Qt::Key_Q:
        qApp->quit();
        break;

    case Qt::Key_Backspace:
    case Qt::Key_Delete:
        if (e->modifiers() & Qt::ControlModifier)
            redo();
        else
            undo();
        break;

#if 0
  case Qt::Key_X:
    levelMap_->random();
    levelChange();
    repaint(false);
    break;

  case Qt::Key_R:
    level(levelMap_->level());
    return;
    break;
  case Qt::Key_N:
    nextLevel();
    return;
    break;
  case Qt::Key_P:
    previousLevel();
    return;
    break;
  case Qt::Key_U:
    undo();
    return;
    break;
  case Qt::Key_I:
    history_->redo(levelMap_);
    repaint(false);
    return;
    break;

  case Qt::Key_S:
    {
      QString buf;
      history_->save(buf);
      printf("%s\n", (char *) buf);
    }
    return;
    break;

  case Qt::Key_L:
    stopMoving();
    history_->clear();
    level(levelMap_->level());
    {
      char buf[4096]="r1*D1*D1*r1*@r1*D1*";
      //scanf("%s", buf);
      history_->load(levelMap_, buf);
    }
    updateStepsDisplay();
    updatePushesDisplay();
    repaint(false);
    return;
    break;
#endif

    case Qt::Key_Print:
        HtmlPrinter::printHtml(&levelMap_->map());
        break;

    default:
        e->ignore();
        return;
    }
}

void PlayField::stopDrag()
{
    if (!dragInProgress_)
        return;

    changeCursor(nullptr);

    update();
    dragInProgress_ = false;
}

void PlayField::dragObject(int xpixel, int ypixel)
{
    const int squareSize = m_groundItem->squareSize();
    const QPoint square = m_groundItem->squareFromScene({xpixel - mousePosX_ + squareSize / 2,
                                                         ypixel - mousePosY_ + squareSize / 2});
    const int x = square.x();
    const int y = square.y();

    if (x == highlightX_ && y == highlightY_)
        return;

    printf("drag %d,%d to %d,%d\n", highlightX_, highlightY_, x, y);
    pathFinder_.drag(highlightX_, highlightY_, x, y);
    stopDrag();
}

void PlayField::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    if (!canMoveNow())
        return;

    if (dragInProgress_) {
        if (e->button() == Qt::LeftButton)
            dragObject(e->scenePos().x(), e->scenePos().y());
        else
            stopDrag();
        return;
    }

    const QPoint square = m_groundItem->squareFromScene(e->scenePos());
    const int x = square.x();
    const int y = square.y();
    if (!levelMap_->map().hasCoord(x, y))
        return;

    if (e->button() == Qt::LeftButton && pathFinder_.canDrag(x, y)) {
        update();
        highlightX_ = x;
        highlightY_ = y;
        pathFinder_.updatePossibleDestinations(x, y);

        const QPointF dragScenePos = m_groundItem->squareToScene({x, y});
        dragX_ = dragScenePos.x();
        dragY_ = dragScenePos.y();
        mousePosX_ = e->scenePos().x() - dragX_;
        mousePosY_ = e->scenePos().y() - dragY_;
        dragInProgress_ = true;
    }

    Move *m;
    switch (e->button()) {
    case Qt::LeftButton:
        m = pathFinder_.search(&levelMap_->map(), x, y);
        if (m != nullptr) {
            history_->add(m);

            startMoving(m);
        }
        break;
    case Qt::MiddleButton:
        undo();
        return;
        break;
    case Qt::RightButton:
        push(x, y);
        break;

    default:
        return;
    }
}

void PlayField::wheelEvent(QGraphicsSceneWheelEvent *e)
{
    wheelDelta_ += e->delta();

    if (wheelDelta_ >= 120) {
        wheelDelta_ %= 120;
        redo();
    } else if (wheelDelta_ <= -120) {
        wheelDelta_ = -(-wheelDelta_ % 120);
        undo();
    }
}

void PlayField::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if (dragInProgress_)
        dragObject(e->scenePos().x(), e->scenePos().y());
}


#if 0
void PlayField::leaveEvent(QEvent *)
{
    TODO: no leaveEvent for scene, so disabled for now. but does this make sense at all?
    stopDrag();
}
#endif

void PlayField::setSize(int w, int h)
{
    setSceneRect(0, 0, w, h);

    int sbarHeight = statusMetrics_.height();

    // TODO: ellide if overlapping with the other items
    m_collectionName->setPos(0, h - sbarHeight);

    int sbarNumWidth = statusMetrics_.boundingRect(QStringLiteral("88888")).width() + 8;
    int sbarLevelWidth = m_levelLabel->boundingRect().width() + 8;
    int sbarStepsWidth = m_stepsLabel->boundingRect().width() + 8;
    int sbarPushesWidth = m_pushesLabel->boundingRect().width() + 8;

    // from right to left
    m_pushesNumber->setPos(w - sbarNumWidth, h - sbarHeight);
    m_pushesLabel->setPos(m_pushesNumber->x() - sbarPushesWidth, h - sbarHeight);
    m_stepsNumber->setPos(m_pushesLabel->x() - sbarNumWidth, h - sbarHeight);
    m_stepsLabel->setPos(m_stepsNumber->x() - sbarStepsWidth, h - sbarHeight);
    m_levelNumber->setPos(m_stepsLabel->x() - sbarNumWidth, h - sbarHeight);
    m_levelLabel->setPos(m_levelNumber->x() - sbarLevelWidth, h - sbarHeight);

    h -= sbarHeight;

    int cols = levelMap_->map().width();
    int rows = levelMap_->map().height();

    // FIXME: the line below should not be needed
    if (cols == 0 || rows == 0)
        return;

    int xsize = w / cols;
    int ysize = h / rows;

    if (xsize < 8)
        xsize = 8;
    if (ysize < 8)
        ysize = 8;

    const int suqareSize = (xsize > ysize ? ysize : xsize);

    const qreal groundX = (w - cols * suqareSize) / 2;
    const qreal groundY = (h - rows * suqareSize) / 2;
    m_groundItem->setPos(groundX, groundY);
    m_groundItem->setSquareSize(suqareSize);

    updateStepsDisplay();
    updatePushesDisplay();

    updateBackground();
}

void PlayField::nextLevel()
{
    if (levelMap_->level() + 1 >= levelMap_->noOfLevels()) {
        showMessage(i18n("\
This is the last level in\n\
the current collection."));
        return;
    }
    if (levelMap_->level() >= levelMap_->completedLevels()) {
        showMessage(i18n("\
You have not completed\n\
this level yet."));
        return;
    }

    level(levelMap_->level() + 1);
    levelChange();
    update();
}

void PlayField::previousLevel()
{
    if (levelMap_->level() <= 0) {
        showMessage(i18n("\
This is the first level in\n\
the current collection."));
        return;
    }
    level(levelMap_->level() - 1);
    levelChange();
    update();
}

void PlayField::undo()
{
    if (!canMoveNow())
        return;

    startMoving(history_->deferUndo(levelMap_));
}

void PlayField::redo()
{
    if (!canMoveNow())
        return;

    startMoving(history_->deferRedo(levelMap_));
}

void PlayField::restartLevel()
{
    stopMoving();
    history_->clear();
    level(levelMap_->level());
    updateStepsDisplay();
    updatePushesDisplay();
    update();
}

void PlayField::changeCollection(LevelCollection *collection)
{
    if (levelMap_->collection() == collection)
        return;
    levelMap_->changeCollection(collection);

    m_collectionName->setText(collectionName());
    levelChange();

    update();
}

void PlayField::updateStepsDisplay()
{
    m_stepsNumber->setText(QString::asprintf("%05d", totalMoves()));
}

void PlayField::updatePushesDisplay()
{
    m_pushesNumber->setText(QString::asprintf("%05d", totalPushes()));
}

void PlayField::changeAnim(int num)
{
    assert(num >= 0 && num <= 3);

    animDelay_ = num;
}

// FIXME: clean up bookmark stuff

// static const int bookmark_id[] = {
//   0, 1, 8, 2, 9, 3, 5, 6, 7, 4
// };

void PlayField::setBookmark(Bookmark *bm)
{
    if (!levelMap_->goodLevel())
        return;

    if (collection()->id() < 0) {
        KMessageBox::error(nullptr,
                           i18n("Sorry, bookmarks for external levels\n"
                                "is not implemented yet."));
        return;
    }

    bm->set(collection()->id(), levelMap_->level(), levelMap_->totalMoves(), history_);
}

void PlayField::goToBookmark(Bookmark *bm)
{
    level(bm->level());
    levelChange();
    if (!bm->goTo(levelMap_, history_))
        fprintf(stderr, "Warning: bad bookmark\n");

    updateStepsDisplay();
    updatePushesDisplay();
    update();
}

bool PlayField::canMoveNow()
{
    if (moveInProgress_)
        return false;
    if (!levelMap_->goodLevel()) {
        showMessage(i18n("This level is broken"));
        return false;
    }
    return true;
}

void PlayField::killTimers()
{
    for (int timerId : std::as_const(timers)) {
        killTimer(timerId);
    }
    timers.clear();
}

#include "moc_PlayField.cpp"
