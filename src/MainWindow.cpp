/*
    SPDX-FileCopyrightText: 1998 Anders Widell <awl@hem.passagen.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "MainWindow.h"

#include "LevelCollection.h"
#include "PlayField.h"
#include "PlayFieldView.h"

#include <KActionCollection>
#include <KConfigGroup>
#include <KIO/FileCopyJob>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSelectAction>
#include <KSharedConfig>
#include <KStandardAction>
#include <KUrlMimeData>

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDragEnterEvent>
#include <QFileDialog>
#include <QKeyEvent>
#include <QMimeData>
#include <QString>
#include <QTemporaryFile>

#include <cassert>
#include <cstdio>

void MainWindow::createCollectionMenu()
{
    collectionsAct_ = new KSelectAction(i18n("&Level Collection"), this);
    actionCollection()->addAction(QStringLiteral("collections"), collectionsAct_);
    connect(collectionsAct_, &KSelectAction::indexTriggered, this, &MainWindow::changeCollection);

    for (int i = 0; i < internalCollections_.collections(); i++) {
        collectionsAct_->addAction(internalCollections_[i]->name());
    }

    KSharedConfigPtr cfg = KSharedConfig::openConfig();
    KConfigGroup settingsGroup(cfg, QStringLiteral("settings"));
    int id = settingsGroup.readEntry("collection", "10").toInt();

    int currentCollection = 0;
    for (int i = 0; i < internalCollections_.collections(); i++) {
        if (internalCollections_[i]->id() == id)
            currentCollection = i;
    }

    changeCollection(currentCollection);
}

MainWindow::MainWindow()
    : KXmlGuiWindow()
{
    // setEraseColor(QColor(0,0,0));

    playField_ = new PlayField(this);
    playFieldView_ = new PlayFieldView (playField_, this);
    setCentralWidget(playFieldView_);

    setupActions();
    setupGUI(Keys | Save | Create);

    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    KSharedConfigPtr cfg = KSharedConfig::openConfig();

    KConfigGroup settingsGroup(cfg, QStringLiteral("settings"));
    settingsGroup.writeEntry("collection", QStringLiteral("%1").arg(internalCollections_[collectionsAct_->currentItem()]->id()));

    for (int i = 1; i <= 10; i++) {
        delete bookmarks_[i - 1];
    }

    delete externalCollection_;

    // The following line segfaults when linked against qt 1.44
    // delete playField_;
}

void MainWindow::setupActions()
{
    KActionCollection *ac = actionCollection();

    QAction *qact = ac->addAction(QStringLiteral("load_levels"), this, &MainWindow::loadLevels);
    qact->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    qact->setText(i18n("&Load Levels..."));

    QAction *nextLevel = ac->addAction(QStringLiteral("next_level"), playField_, &PlayField::nextLevel);
    nextLevel->setIcon(QIcon::fromTheme(QStringLiteral("go-next")));
    nextLevel->setText(i18n("&Next Level"));
    ac->setDefaultShortcut(nextLevel, QKeySequence(Qt::Key_N));

    QAction *prevLevel = ac->addAction(QStringLiteral("prev_level"), playField_, &PlayField::previousLevel);
    prevLevel->setIcon(QIcon::fromTheme(QStringLiteral("go-previous")));
    prevLevel->setText(i18n("&Previous Level"));
    ac->setDefaultShortcut(prevLevel, QKeySequence(Qt::Key_P));

    QAction *reload = ac->addAction(QStringLiteral("reload_level"), playField_, &PlayField::restartLevel);
    reload->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));
    reload->setText(i18n("Re&start Level"));
    ac->setDefaultShortcut(reload, QKeySequence(Qt::Key_Escape));

    createCollectionMenu();

    ac->addAction(KStandardAction::Undo, QStringLiteral("undo"), playField_, &PlayField::undo);
    ac->addAction(KStandardAction::Redo, QStringLiteral("redo"), playField_, &PlayField::redo);
    ac->addAction(KStandardAction::Quit, QStringLiteral("quit"), this, &MainWindow::close);

    auto *animGrp = new QActionGroup(this);
    connect(animGrp, &QActionGroup::triggered, this, &MainWindow::handleAnimSpeedSelected);

    qa_slow = ac->addAction(QStringLiteral("anim_slow"));
    qa_slow->setText(i18n("&Slow"));
    qa_slow->setCheckable(true);
    qa_slow->setData(3);
    qa_slow->setActionGroup(animGrp);
    qa_medium = ac->addAction(QStringLiteral("anim_medium"));
    qa_medium->setText(i18n("&Medium"));
    qa_medium->setCheckable(true);
    qa_medium->setData(2);
    qa_medium->setActionGroup(animGrp);
    qa_fast = ac->addAction(QStringLiteral("anim_fast"));
    qa_fast->setText(i18n("&Fast"));
    qa_fast->setCheckable(true);
    qa_fast->setData(1);
    qa_fast->setActionGroup(animGrp);
    qa_off = ac->addAction(QStringLiteral("anim_off"));
    qa_off->setText(i18n("&Off"));
    qa_off->setCheckable(true);
    qa_off->setData(0);
    qa_off->setActionGroup(animGrp);

    const int animDelay = playField_->animDelay();
    if (animDelay == 3) {
        qa_slow->setChecked(true);
    } else if (animDelay == 2) {
        qa_medium->setChecked(true);
    } else if (animDelay == 1) {
        qa_fast->setChecked(true);
    } else if (animDelay == 0) {
        qa_off->setChecked(true);
    }

    auto *setBmGrp = new QActionGroup(this);
    setBmGrp->setExclusive(false);
    connect(setBmGrp, &QActionGroup::triggered, this, &MainWindow::setBookmark);

    const struct {
        QString addActionId;
        QString gotoActionId;
        Qt::Key key;
    } bokkmarkActionData[10] = {
        {QStringLiteral("bm_add_1"), QStringLiteral("bm_goto_1"), Qt::Key_1},
        {QStringLiteral("bm_add_2"), QStringLiteral("bm_goto_2"), Qt::Key_2},
        {QStringLiteral("bm_add_3"), QStringLiteral("bm_goto_3"), Qt::Key_3},
        {QStringLiteral("bm_add_4"), QStringLiteral("bm_goto_4"), Qt::Key_4},
        {QStringLiteral("bm_add_5"), QStringLiteral("bm_goto_5"), Qt::Key_5},
        {QStringLiteral("bm_add_6"), QStringLiteral("bm_goto_6"), Qt::Key_6},
        {QStringLiteral("bm_add_7"), QStringLiteral("bm_goto_7"), Qt::Key_7},
        {QStringLiteral("bm_add_8"), QStringLiteral("bm_goto_8"), Qt::Key_8},
        {QStringLiteral("bm_add_9"), QStringLiteral("bm_goto_9"), Qt::Key_9},
        {QStringLiteral("bm_add_10"), QStringLiteral("bm_goto_10"), Qt::Key_0},
    };
    for (int i = 0; i < 10; i++) {
        auto &actionData = bokkmarkActionData[i];
        QAction *action = ac->addAction(actionData.addActionId);
        action->setIcon(QIcon::fromTheme(QStringLiteral("bookmark-new")));
        action->setText(i18n("(unused)"));
        ac->setDefaultShortcut(action, QKeySequence(Qt::CTRL | actionData.key));
        action->setData(i + 1);
        action->setActionGroup(setBmGrp);
        setBM_act[i] = action;
    }

    auto *gotoBmGrp = new QActionGroup(this);
    gotoBmGrp->setExclusive(false);
    connect(gotoBmGrp, &QActionGroup::triggered, this, &MainWindow::goToBookmark);

    for (int i = 0; i < 10; i++) {
        auto &actionData = bokkmarkActionData[i];
        QAction *action = ac->addAction(actionData.gotoActionId);
        action->setText(i18n("(unused)"));
        ac->setDefaultShortcut(action, QKeySequence(actionData.key));
        action->setData(i + 1);
        action->setActionGroup(gotoBmGrp);
        goToBM_act[i] = action;
    }

    for (int i = 1; i <= 10; i++) {
        bookmarks_[i - 1] = new Bookmark(i);
        updateBookmark(i);
    }
}

void MainWindow::handleAnimSpeedSelected(QAction *action)
{
    playField_->changeAnim(action->data().toInt());
}

void MainWindow::focusInEvent(QFocusEvent *)
{
    playFieldView_->setFocus();
}

void MainWindow::updateBookmark(int num)
{
    int col = internalCollections_.toInternalId(bookmarks_[num - 1]->collection());
    int lev = bookmarks_[num - 1]->level();
    int mov = bookmarks_[num - 1]->moves();

    if (col < 0 || lev < 0)
        return;

    QString name;
    if (col >= 0 && col < internalCollections_.collections())
        name = internalCollections_[col]->name();
    else
        name = i18n("(invalid)");
    QString l;
    l.setNum(lev + 1);
    name += QLatin1String(" #") + l;
    l.setNum(mov);
    name += QLatin1String(" (") + l + QLatin1Char(')');

    setBM_act[num - 1]->setText(name);
    goToBM_act[num - 1]->setText(name);
}

void MainWindow::setBookmark(QAction *action)
{
    const int id = action->data().toInt();
    assert(id >= 1 && id <= 10);
    playField_->setBookmark(bookmarks_[id - 1]);
    updateBookmark(id);
}

void MainWindow::goToBookmark(QAction *action)
{
    const int id = action->data().toInt();
    assert(id >= 1 && id <= 10);

    Bookmark *bm = bookmarks_[id - 1];
    int collection = internalCollections_.toInternalId(bm->collection());
    int level = bm->level();

    if (collection < 0 || collection >= internalCollections_.collections())
        return;
    LevelCollection *colPtr = internalCollections_[collection];
    if (colPtr == nullptr)
        return;
    if (level < 0 || level >= colPtr->noOfLevels())
        return;
    if (level > colPtr->completedLevels())
        return;

    playFieldView_->setUpdatesEnabled(false);
    changeCollection(collection);
    playFieldView_->setUpdatesEnabled(true);
    playField_->goToBookmark(bookmarks_[id - 1]);
}

void MainWindow::changeCollection(int id)
{
    collectionsAct_->setCurrentItem(id);

    delete externalCollection_;
    externalCollection_ = nullptr;
    playField_->changeCollection(internalCollections_[id]);
}

void MainWindow::loadLevels()
{
    KSharedConfigPtr cfg = KSharedConfig::openConfig();
    KConfigGroup settingsGroup(cfg, QStringLiteral("settings"));
    const QUrl lastFile = QUrl::fromLocalFile(settingsGroup.readPathEntry("lastLevelFile", QString()));

    QUrl result = QFileDialog::getOpenFileUrl(this, i18nc("@title:window", "Load Levels from a File"), lastFile, QStringLiteral("*"));
    if (result.isEmpty())
        return;

    openURL(result);
}

void MainWindow::openURL(const QUrl &_url)
{
    KSharedConfigPtr cfg = KSharedConfig::openConfig();

    //   int namepos = _url.path().findRev('/') + 1; // NOTE: findRev can return -1
    //   QString levelName = _url.path().mid(namepos);
    QString levelName = _url.fileName();

    QString levelFile;
    QTemporaryFile f;
    if (_url.isLocalFile()) {
        levelFile = _url.toLocalFile();
    } else {
        //     levelFile = locateLocal("appdata", "levels/" + levelName);
        // download file into temporary one
        f.open();
        KIO::FileCopyJob *job = KIO::file_copy(_url, QUrl::fromLocalFile(f.fileName()), -1, KIO::Overwrite);

        job->exec();
        if (job->error()) {
            return;
        }
        levelFile = f.fileName();
    }

    auto *tmpCollection = new LevelCollection(levelFile, levelName);

    if (tmpCollection->noOfLevels() < 1) {
        KMessageBox::error(this, i18n("No levels found in file"));
        delete tmpCollection;
        return;
    }
    if (_url.isLocalFile()) {
        KConfigGroup settingsGroup(cfg, QStringLiteral("settings"));
        settingsGroup.writePathEntry("lastLevelFile", _url.toLocalFile());
    }

    delete externalCollection_;
    externalCollection_ = tmpCollection;

    playField_->changeCollection(externalCollection_);
}

QSize MainWindow::sizeHint() const
{
    return KXmlGuiWindow::sizeHint().expandedTo(QSize(750, 562));
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = KUrlMimeData::urlsFromMimeData(event->mimeData());
    if (!urls.isEmpty()) {
        //     kdDebug() << "MainWindow:Handling QUriDrag..." << endl;
        if (urls.count() > 0) {
            const QUrl &url = urls.first();
            openURL(url);
        }
    }
}

#include "moc_MainWindow.cpp"
