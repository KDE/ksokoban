/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "PlayField.h"

#include "Bookmark.h"
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

// hardcoded for now, featch from theme
static constexpr int SMALL_STONES = 4;
static constexpr int LARGE_STONES = 6;

PlayField::PlayField(QObject *parent)
    : QGraphicsScene(parent)
    , crossCursor(Qt::CrossCursor)
    , m_renderer(createClassicTheme())
    , levelText_(i18n("Level:"))
    , stepsText_(i18n("Steps:"))
    , pushesText_(i18n("Pushes:"))
    , statusFont_(QFontDatabase::systemFont(QFontDatabase::GeneralFont).family(), 18, QFont::Bold)
    , statusMetrics_(statusFont_)
{
    highlightX_ = highlightY_ = 0;
    stoneIndex_.setStoneCount(LARGE_STONES, SMALL_STONES);

    KSharedConfigPtr cfg = KSharedConfig::openConfig();
    KConfigGroup settingsGroup(cfg, "settings");

    animDelay_ = settingsGroup.readEntry("animDelay", QStringLiteral("2")).toInt();
    if (animDelay_ < 0 || animDelay_ > 3)
        animDelay_ = 2;

    history_ = new History;

    levelMap_ = new LevelMap;

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
    setSize(width(), height());

    updateLevelXpm();
    updateStepsXpm();
    updatePushesXpm();
    highlight();
}

QPixmap PlayField::stonePixmap(int stoneIndex) const
{
    const QString spriteName = QStringLiteral("stone_%1").arg(stoneIndex);

    return m_renderer.spritePixmap(spriteName, QSize(size_, halfSize_));
}

QPixmap PlayField::halfStonePixmap(int stoneIndex) const
{
    const QString spriteName = QStringLiteral("halfstone_%1").arg(stoneIndex);

    return m_renderer.spritePixmap(spriteName, QSize(halfSize_, halfSize_));
}

void PlayField::paintWall(int x, int y, QPainter &paint)
{
    const bool left = levelMap_->wallLeft(x, y);
    const bool right = levelMap_->wallRight(x, y);
    const int pixelX = x2pixel(x);
    const int pixelY = y2pixel(y);
    const int stoneIndex = x + y * (Map::MAX_X + 1);

    const qreal dpr = qApp->devicePixelRatio();
    const int deviceSize_ = size_ * dpr;
    const int halfdeviceSize_ = deviceSize_ / 2;

    if (left)
        paint.drawPixmap(pixelX, pixelY, stonePixmap(stoneIndex_.upperLarge(stoneIndex - 1)),
                         halfdeviceSize_, 0, -1, -1);
    else
        paint.drawPixmap(pixelX, pixelY, halfStonePixmap(stoneIndex_.leftSmall(stoneIndex)));

    if (right)
        paint.drawPixmap(pixelX + halfSize_, pixelY, stonePixmap(stoneIndex_.upperLarge(stoneIndex)),
                         0, 0, halfdeviceSize_, -1);
    else
        paint.drawPixmap(pixelX + halfSize_, pixelY, halfStonePixmap(stoneIndex_.rightSmall(stoneIndex)));

    paint.drawPixmap(pixelX, pixelY + halfSize_, stonePixmap(stoneIndex_.lowerLarge(stoneIndex)));
}

void PlayField::paintSquare(int x, int y, QPainter &paint)
{
    if (levelMap_->wall(x, y)) {
        paintWall(x, y, paint);
        return;
    }

    QString spriteName;
    if (levelMap_->xpos() == x && levelMap_->ypos() == y) {
        if (levelMap_->goal(x, y))
            spriteName = QStringLiteral("saveman");
        else {
            spriteName = QStringLiteral("man");
        }
    } else if (levelMap_->empty(x, y)) {
        if (levelMap_->floor(x, y)) {
            if (levelMap_->goal(x, y))
                spriteName = QStringLiteral("goal");
            else {
                // shortcut for now, replace with theme pixmap (or color property)
                paint.fillRect(x2pixel(x), y2pixel(y), size_, size_, QColor(0x67, 0x67, 0x67, 255));
                return;
            }
        }
    } else if (levelMap_->object(x, y)) {
        // TODO: add highlighting & other states to KGameRenderer
#if 0
        if (highlightX_ == x && highlightY_ == y) {
            if (levelMap_->goal(x, y))
                imageData_->brightTreasure(paint, x2pixel(x), y2pixel(y));
            else
                imageData_->brightObject(paint, x2pixel(x), y2pixel(y));
            return;
        } else
#endif
        {
            if (levelMap_->goal(x, y))
                spriteName = QStringLiteral("treasure");
            else
                spriteName = QStringLiteral("object");
        }
    }

    if (spriteName.isEmpty()) {
        return;
    }

    const QPixmap pixmap = m_renderer.spritePixmap(spriteName, QSize(size_, size_));
    paint.drawPixmap(x2pixel(x), y2pixel(y), pixmap);
}

void PlayField::drawBackground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawBackground(painter, rect);
    paintPainter(*painter, rect.toRect());
}

void PlayField::paintPainter(QPainter &paint, const QRect &rect)
{
    if (size_ <= 0)
        return;
    int minx = pixel2x(rect.x());
    int miny = pixel2y(rect.y());
    int maxx = pixel2x(rect.x() + rect.width() - 1);
    int maxy = pixel2y(rect.y() + rect.height() - 1);

    if (minx < 0)
        minx = 0;
    if (miny < 0)
        miny = 0;
    if (maxx >= levelMap_->width())
        maxx = levelMap_->width() - 1;
    if (maxy >= levelMap_->height())
        maxy = levelMap_->height() - 1;

    for (int y = miny; y <= maxy; y++) {
        for (int x = minx; x <= maxx; x++) {
            paintSquare(x, y, paint);
        }
    }

    if (collRect_.intersects(rect))
        paint.drawPixmap(collRect_.x(), collRect_.y(), collXpm_);
    if (ltxtRect_.intersects(rect))
        paint.drawPixmap(ltxtRect_.x(), ltxtRect_.y(), ltxtXpm_);
    if (lnumRect_.intersects(rect))
        paint.drawPixmap(lnumRect_.x(), lnumRect_.y(), lnumXpm_);
    if (stxtRect_.intersects(rect))
        paint.drawPixmap(stxtRect_.x(), stxtRect_.y(), stxtXpm_);
    if (snumRect_.intersects(rect))
        paint.drawPixmap(snumRect_.x(), snumRect_.y(), snumXpm_);
    if (ptxtRect_.intersects(rect))
        paint.drawPixmap(ptxtRect_.x(), ptxtRect_.y(), ptxtXpm_);
    if (pnumRect_.intersects(rect))
        paint.drawPixmap(pnumRect_.x(), pnumRect_.y(), pnumXpm_);
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

    {
        int x = pixel2x(dragX_ + size_ / 2);
        int y = pixel2y(dragY_ + size_ / 2);
        if (x >= 0 && x < levelMap_->width() && y >= 0 && y < levelMap_->height() && pathFinder_.canDragTo(x, y)) {
            x = x2pixel(x);
            y = y2pixel(y);

            if (dragX_ >= x - size_ / 4 && dragX_ < x + size_ / 4 && dragY_ >= y - size_ / 4 && dragY_ < y + size_ / 4) {
                dragX_ = x;
                dragY_ = y;
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
    if (size_ == 0)
        return;

    int x = pixel2x(lastMouseXPos_);
    int y = pixel2y(lastMouseYPos_);

    if (x < 0 || y < 0 || x >= levelMap_->width() || y >= levelMap_->height())
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
    updateStepsXpm();
    updatePushesXpm();

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
    updateStepsXpm();
    updatePushesXpm();
    repaint(false);
    return;
    break;
#endif

    case Qt::Key_Print:
        HtmlPrinter::printHtml(levelMap_);
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
    int x = pixel2x(xpixel - mousePosX_ + size_ / 2);
    int y = pixel2y(ypixel - mousePosY_ + size_ / 2);

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

    int x = pixel2x(e->scenePos().x());
    int y = pixel2y(e->scenePos().y());
    if (x < 0 || y < 0 || x >= levelMap_->width() || y >= levelMap_->height())
        return;

    if (e->button() == Qt::LeftButton && pathFinder_.canDrag(x, y)) {
        update();
        highlightX_ = x;
        highlightY_ = y;
        pathFinder_.updatePossibleDestinations(x, y);

        dragX_ = x2pixel(x);
        dragY_ = y2pixel(y);
        mousePosX_ = e->scenePos().x() - dragX_;
        mousePosY_ = e->scenePos().y() - dragY_;
        dragInProgress_ = true;
    }

    Move *m;
    switch (e->button()) {
    case Qt::LeftButton:
        m = pathFinder_.search(levelMap_, x, y);
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
    int sbarNumWidth = statusMetrics_.boundingRect(QStringLiteral("88888")).width() + 8;
    int sbarLevelWidth = statusMetrics_.boundingRect(levelText_).width() + 8;
    int sbarStepsWidth = statusMetrics_.boundingRect(stepsText_).width() + 8;
    int sbarPushesWidth = statusMetrics_.boundingRect(pushesText_).width() + 8;

    pnumRect_.setRect(w - sbarNumWidth, h - sbarHeight, sbarNumWidth, sbarHeight);
    ptxtRect_.setRect(pnumRect_.x() - sbarPushesWidth, h - sbarHeight, sbarPushesWidth, sbarHeight);
    snumRect_.setRect(ptxtRect_.x() - sbarNumWidth, h - sbarHeight, sbarNumWidth, sbarHeight);
    stxtRect_.setRect(snumRect_.x() - sbarStepsWidth, h - sbarHeight, sbarStepsWidth, sbarHeight);
    lnumRect_.setRect(stxtRect_.x() - sbarNumWidth, h - sbarHeight, sbarNumWidth, sbarHeight);
    ltxtRect_.setRect(lnumRect_.x() - sbarLevelWidth, h - sbarHeight, sbarLevelWidth, sbarHeight);
    collRect_.setRect(0, h - sbarHeight, ltxtRect_.x(), sbarHeight);

    // printf("collRect_:%d;%d;%d;%d\n",collRect_.x(), collRect_.y(), collRect_.width(), collRect_.height());

    const qreal dpr = qApp->devicePixelRatio();

    ltxtXpm_ = QPixmap(ltxtRect_.size() * dpr);
    ltxtXpm_.setDevicePixelRatio(dpr);
    lnumXpm_ = QPixmap(lnumRect_.size() * dpr);
    lnumXpm_.setDevicePixelRatio(dpr);
    stxtXpm_ = QPixmap(stxtRect_.size() * dpr);
    stxtXpm_.setDevicePixelRatio(dpr);
    snumXpm_ = QPixmap(snumRect_.size() * dpr);
    snumXpm_.setDevicePixelRatio(dpr);
    ptxtXpm_ = QPixmap(ptxtRect_.size() * dpr);
    ptxtXpm_.setDevicePixelRatio(dpr);
    pnumXpm_ = QPixmap(pnumRect_.size() * dpr);
    pnumXpm_.setDevicePixelRatio(dpr);
    collXpm_ = QPixmap(collRect_.size() * dpr);
    collXpm_.setDevicePixelRatio(dpr);

    h -= sbarHeight;

    int cols = levelMap_->width();
    int rows = levelMap_->height();

    // FIXME: the line below should not be needed
    if (cols == 0 || rows == 0)
        return;

    int xsize = w / cols;
    int ysize = h / rows;

    if (xsize < 8)
        xsize = 8;
    if (ysize < 8)
        ysize = 8;

    size_ = (xsize > ysize ? ysize : xsize);
    halfSize_ = size_ / 2;

    xOffs_ = (w - cols * size_) / 2;
    yOffs_ = (h - rows * size_) / 2;

    updateCollectionXpm();
    updateTextXpm();
    updateLevelXpm();
    updateStepsXpm();
    updatePushesXpm();

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
    updateStepsXpm();
    updatePushesXpm();
    update();
}

void PlayField::changeCollection(LevelCollection *collection)
{
    if (levelMap_->collection() == collection)
        return;
    levelMap_->changeCollection(collection);
    levelChange();
    // erase(collRect_);
    update();
}

void PlayField::updateCollectionXpm()
{
    if (collXpm_.isNull())
        return;
    // printf("executing PlayField::updateCollectionXpm() w:%d, h:%d\n",collXpm_->width(), collXpm_->height());

    collXpm_.fill(QColor(0, 0, 0, 0));

    QPainter paint(&collXpm_);

    paint.setFont(statusFont_);
    paint.setPen(QColor(0, 255, 0));
    paint.drawText(0, 0, collRect_.width(), collRect_.height(), Qt::AlignLeft, collectionName());
}

void PlayField::updateTextXpm()
{
    if (ltxtXpm_.isNull())
        return;
    // printf("executing PlayField::updateTextXpm() w:%d, h:%d\n",ltxtXpm_->width(), ltxtXpm_->height());

    ltxtXpm_.fill(QColor(0, 0, 0, 0));
    stxtXpm_.fill(QColor(0, 0, 0, 0));
    ptxtXpm_.fill(QColor(0, 0, 0, 0));

    QPainter paint;

    paint.begin(&ltxtXpm_);
    paint.setFont(statusFont_);
    paint.setPen(QColor(128, 128, 128));
    paint.drawText(0, 0, ltxtRect_.width(), ltxtRect_.height(), Qt::AlignLeft, levelText_);
    paint.end();

    paint.begin(&stxtXpm_);
    paint.setFont(statusFont_);
    paint.setPen(QColor(128, 128, 128));
    paint.drawText(0, 0, stxtRect_.width(), stxtRect_.height(), Qt::AlignLeft, stepsText_);
    paint.end();

    paint.begin(&ptxtXpm_);
    paint.setFont(statusFont_);
    paint.setPen(QColor(128, 128, 128));
    paint.drawText(0, 0, ptxtRect_.width(), ptxtRect_.height(), Qt::AlignLeft, pushesText_);
    paint.end();
}

void PlayField::updateLevelXpm()
{
    if (lnumXpm_.isNull())
        return;
    // printf("executing PlayField::updateLevelXpm()\n");

    lnumXpm_.fill(QColor(0, 0, 0, 0));

    QPainter paint(&lnumXpm_);

    paint.setFont(statusFont_);
    paint.setPen(QColor(255, 0, 0));
    paint.drawText(0, 0, lnumRect_.width(), lnumRect_.height(), Qt::AlignLeft, QString::asprintf("%05d", level() + 1));
}

void PlayField::updateStepsXpm()
{
    if (snumXpm_.isNull())
        return;
    // printf("executing PlayField::updateStepsXpm()\n");

    snumXpm_.fill(QColor(0, 0, 0, 0));

    QPainter paint(&snumXpm_);

    paint.setFont(statusFont_);
    paint.setPen(QColor(255, 0, 0));
    paint.drawText(0, 0, snumRect_.width(), snumRect_.height(), Qt::AlignLeft, QString::asprintf("%05d", totalMoves()));
}

void PlayField::updatePushesXpm()
{
    if (pnumXpm_.isNull())
        return;
    // printf("executing PlayField::updatePushesXpm()\n");

    pnumXpm_.fill(QColor(0, 0, 0, 0));

    QPainter paint(&pnumXpm_);

    paint.setFont(statusFont_);
    paint.setPen(QColor(255, 0, 0));
    paint.drawText(0, 0, pnumRect_.width(), pnumRect_.height(), Qt::AlignLeft, QString::asprintf("%05d", totalPushes()));
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
    // updateLevelXpm();
    updateStepsXpm();
    updatePushesXpm();
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
