/*
 *  ksokoban - a Sokoban game for KDE
 *  Copyright (C) 1998  Anders Widell  <awl@hem.passagen.se>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <kapplication.h>
#include <kstandardaction.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <kselectaction.h>
#include <klocale.h>
#include <kconfig.h>
#include <kio/netaccess.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kicon.h>

#include <k3urldrag.h>
#include <QPixmap>
#include <QDragEnterEvent>
#include <QFocusEvent>
#include <QDropEvent>

#include "MainWindow.h"
#include "PlayField.h"
#include "LevelCollection.h"

#include "MainWindow.moc"

void
MainWindow::createCollectionMenu() {
  collectionsAct_ = new KSelectAction( i18n("&Level Collection"), this );
  actionCollection()->addAction( "collections", collectionsAct_ );
  connect( collectionsAct_, SIGNAL(triggered(int)), this, SLOT(changeCollection(int)) );

  for (int i=0; i<internalCollections_.collections(); i++) {
    collectionsAct_->addAction(internalCollections_[i]->name());
  }

  KConfigGroup settings(KGlobal::config(), "settings");
  int id = settings.readEntry("collection", 10);

  int currentCollection = 0;
  for (int i=0; i<internalCollections_.collections(); i++) {
    if (internalCollections_[i]->id() == id) currentCollection = i;
  }

  changeCollection(currentCollection);
}


MainWindow::MainWindow() : KMainWindow(0), externalCollection_(0), collectionsAct_(0) {
  KConfigGroup cfg(KGlobal::config(), "Geometry");
  int width = cfg.readEntry("width", 750);
  int height = cfg.readEntry("height", 562);
  resize(width, height);

  playField_ = new PlayField(this);
  playField_->setObjectName( "playfield" );
  setCentralWidget(playField_);
  playField_->show();

  setupActions();

  for (int i=1; i<=10; i++) {
    bookmarks_[i-1] = new Bookmark(i);
    updateBookmark(i);
  }

  setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
  KConfigGroup cfg(KGlobal::config(), "Geometry");

  cfg.writeEntry("width", width());
  cfg.writeEntry("height", height());

  KConfigGroup settings(KGlobal::config(), "settings");
  settings.writeEntry("collection", internalCollections_[collectionsAct_->currentItem()]->id());

  for (int i=1; i<=10; i++) {
    delete bookmarks_[i-1];
  }


  delete externalCollection_;

  //delete playField_;
}

void
MainWindow::setupActions() {
  actionCollection()->addAction( KStandardAction::Open, "load_levels",
                                 this, SLOT(loadLevels()) );

  QAction* nextLevel = actionCollection()->addAction( "next_level" );
  nextLevel->setIcon( KIcon("forward") );
  nextLevel->setText( i18n("&Next Level") );
  nextLevel->setShortcut( Qt::Key_N );
  connect( nextLevel, SIGNAL(triggered(bool)), playField_, SLOT(nextLevel()) );

  QAction* prevLevel = actionCollection()->addAction( "prev_level" );
  prevLevel->setIcon( KIcon("back") );
  prevLevel->setText( i18n("&Previous Level") );
  prevLevel->setShortcut( Qt::Key_P );
  connect( prevLevel, SIGNAL(triggered(bool)), playField_, SLOT(previousLevel()) );

  QAction* reload = actionCollection()->addAction( "reload_level" );
  reload->setIcon( KIcon("reload") );
  reload->setText( i18n("Re&start Level") );
  reload->setShortcut( Qt::Key_Escape );
  connect( reload, SIGNAL(triggered(bool)), playField_, SLOT(restartLevel()) );

  createCollectionMenu();

  actionCollection()->addAction( KStandardAction::Undo, "undo", playField_, SLOT(undo()) );
  actionCollection()->addAction( KStandardAction::Redo, "redo", playField_, SLOT(redo()) );
  actionCollection()->addAction( KStandardAction::Quit, "quit", this, SLOT(close()) );

  QActionGroup* animGrp = new QActionGroup(this);
  animGrp->setExclusive(true);
  connect(animGrp, SIGNAL(triggered(QAction*)), SLOT(slotAnimSpeedSelected(QAction*)) );

  // FIXME dimsuz: use QVariant setData() as with bookmarks?
  KToggleAction* slow   = new KToggleAction( i18n("&Slow"), this );
  actionCollection()->addAction( "anim_slow", slow );
  slow->setActionGroup( animGrp );
  KToggleAction* medium = new KToggleAction( i18n("&Medium"), this );
  actionCollection()->addAction( "anim_medium", medium );
  medium->setActionGroup( animGrp );
  KToggleAction* fast   = new KToggleAction( i18n("&Fast"), this );
  actionCollection()->addAction( "anim_fast", fast );
  fast->setActionGroup( animGrp );
  KToggleAction* off    = new KToggleAction( i18n("&Off"), this );
  actionCollection()->addAction( "anim_off", off );
  off->setActionGroup( animGrp );

  int animDelay = playField_->animDelay();

  // select saved animation speed
  if( animDelay == 3 )
      slow->setChecked( true );
  else if( animDelay == 2 )
      medium->setChecked( true );
  else if( animDelay == 1 )
      fast->setChecked( true );
  else if( animDelay == 0 )
      off->setChecked( true );


  QAction* setBm[NUM_BOOKMARKS] = { 0 };
  QActionGroup* setBmGrp = new QActionGroup(this);
  setBmGrp->setExclusive(false);
  for(int i=0; i<NUM_BOOKMARKS; i++)
  {
      setBm[i] = actionCollection()->addAction( QString("bm_add_%1").arg(i+1) );
      setBm[i]->setIcon( KIcon("bookmark_add") );
      setBm[i]->setText( i18n("(unused)") );
      setBm[i]->setData( QVariant(i+1) );
      setBmGrp->addAction( setBm[i] );
  }

  // uhhh... have to do this by hand...
  // Or there is a way?
  // If you know one, mail me: dimsuz@gmail.com ;)
  setBm[0]->setShortcut( Qt::CTRL + Qt::Key_1 );
  setBm[1]->setShortcut( Qt::CTRL + Qt::Key_2 );
  setBm[2]->setShortcut( Qt::CTRL + Qt::Key_3 );
  setBm[3]->setShortcut( Qt::CTRL + Qt::Key_4 );
  setBm[4]->setShortcut( Qt::CTRL + Qt::Key_5 );
  setBm[5]->setShortcut( Qt::CTRL + Qt::Key_6 );
  setBm[6]->setShortcut( Qt::CTRL + Qt::Key_7 );
  setBm[7]->setShortcut( Qt::CTRL + Qt::Key_8 );
  setBm[8]->setShortcut( Qt::CTRL + Qt::Key_9 );
  setBm[9]->setShortcut( Qt::CTRL + Qt::Key_0 );

  connect(setBmGrp, SIGNAL(triggered(QAction*)), this, SLOT(slotSetBookmark(QAction*)));

  QAction* gotoBm[NUM_BOOKMARKS] = { 0 };
  QActionGroup* gotoBmGrp = new QActionGroup(this);
  gotoBmGrp->setExclusive(false);
  for(int i=0; i<NUM_BOOKMARKS; i++)
  {
      gotoBm[i] = actionCollection()->addAction( QString("bm_goto_%1").arg(i+1) );
      gotoBm[i]->setIcon( KIcon("bookmark") );
      gotoBm[i]->setText( i18n("(unused)") );
      gotoBm[i]->setData( QVariant(i+1) );
      gotoBmGrp->addAction( gotoBm[i] );
  }

  // uhhh... have to do this by hand...
  // Or there is a way?
  // If you know one, mail me: dimsuz@gmail.com ;)
  gotoBm[0]->setShortcut( Qt::Key_1 );
  gotoBm[1]->setShortcut( Qt::Key_2 );
  gotoBm[2]->setShortcut( Qt::Key_3 );
  gotoBm[3]->setShortcut( Qt::Key_4 );
  gotoBm[4]->setShortcut( Qt::Key_5 );
  gotoBm[5]->setShortcut( Qt::Key_6 );
  gotoBm[6]->setShortcut( Qt::Key_7 );
  gotoBm[7]->setShortcut( Qt::Key_8 );
  gotoBm[8]->setShortcut( Qt::Key_9 );
  gotoBm[9]->setShortcut( Qt::Key_0 );

  connect(gotoBmGrp, SIGNAL(triggered(QAction*)), this, SLOT(slotGotoBookmark(QAction*)));

  createGUI();
}

void
MainWindow::focusInEvent(QFocusEvent *) {
  playField_->setFocus();
}

void
MainWindow::updateBookmark(int num) {
  int col = internalCollections_.toInternalId(bookmarks_[num-1]->collection());
  int lev = bookmarks_[num-1]->level();
  int mov = bookmarks_[num-1]->moves();

  if (col < 0 || lev < 0) return;

  QString name;
  if (col >= 0 && col < internalCollections_.collections())
    name = internalCollections_[col]->name();
  else
    name = i18n("(invalid)");
  QString l;
  l.setNum(lev+1);
  name += " #" + l;
  l.setNum(mov);
  name += " (" + l + ")";

  actionCollection()->action( QString("bm_add_%1").arg(num) )->setText(name);
  actionCollection()->action( QString("bm_goto_%1").arg(num) )->setText(name);
}

void
MainWindow::slotSetBookmark(QAction* bmAct) {
  int id = bmAct->data().toInt();
  assert(id >= 1 && id <= 10);
  playField_->setBookmark(bookmarks_[id-1]);
  updateBookmark(id);
}

void
MainWindow::slotGotoBookmark(QAction* bmAct) {
  int id = bmAct->data().toInt();
  assert(id >= 1 && id <= 10);

  Bookmark *bm = bookmarks_[id-1];
  int collection = internalCollections_.toInternalId(bm->collection());
  int level = bm->level();

  if (collection < 0 || collection >= internalCollections_.collections()) return;
  LevelCollection* colPtr = internalCollections_[collection];
  if (colPtr == 0) return;
  if (level < 0 || level >= colPtr->noOfLevels()) return;
  if (level > colPtr->completedLevels()) return;

  playField_->setUpdatesEnabled(false);
  changeCollection(collection);
  playField_->setUpdatesEnabled(true);
  playField_->goToBookmark(bookmarks_[id-1]);
}

void
MainWindow::changeCollection(int id)
{
  collectionsAct_->setCurrentItem(id);

  delete externalCollection_;
  externalCollection_ = 0;
  playField_->changeCollection(internalCollections_[id]);
}

void
MainWindow::loadLevels() {
  KConfig *cfg=KGlobal::config();
  cfg->setGroup("settings");
  QString lastFile = cfg->readPathEntry("lastLevelFile");

  KUrl result = KFileDialog::getOpenUrl(lastFile, "*", this, i18n("Load Levels From File"));
  if (result.isEmpty()) return;

  openUrl(result);
}

void
MainWindow::slotAnimSpeedSelected(QAction* speedAct)
{
    // FIXME dimsuz: attach data to each action and check it instead?
    if(speedAct == actionCollection()->action("anim_slow"))
        playField_->setAnimationSpeed(3);
    else if(speedAct == actionCollection()->action("anim_medium"))
        playField_->setAnimationSpeed(2);
    else if(speedAct == actionCollection()->action("anim_fast"))
        playField_->setAnimationSpeed(1);
    else if(speedAct == actionCollection()->action("anim_off"))
        playField_->setAnimationSpeed(0);
}

void
MainWindow::openUrl(KUrl _url) {
  KConfig *cfg=KGlobal::config();

//   int namepos = _url.path().findRev('/') + 1; // NOTE: findRev can return -1
//   QString levelName = _url.path().mid(namepos);
  QString levelName = _url.fileName();

  QString levelFile;
  if (_url.isLocalFile()) {
    levelFile = _url.path();
  } else {
//     levelFile = locateLocal("appdata", "levels/" + levelName);
    if(!KIO::NetAccess::download( _url, levelFile, this ) )
	  return;
  }

  LevelCollection *tmpCollection = new LevelCollection(levelFile, levelName);
  KIO::NetAccess::removeTempFile(levelFile );

  if (tmpCollection->noOfLevels() < 1) {
    KMessageBox::sorry(this, i18n("No levels found in file"));
    delete tmpCollection;
    return;
  }
  if (_url.isLocalFile()) {
    cfg->setGroup("settings");
    cfg->writePathEntry("lastLevelFile", _url.path());
  }

  delete externalCollection_;
  externalCollection_ = tmpCollection;

  // FIXME dimsuz: is it ok to uncheck _selected_ action in KSelectAction?
  collectionsAct_->currentAction()->setChecked(false);

  playField_->changeCollection(externalCollection_);
}

void
MainWindow::dragEnterEvent(QDragEnterEvent* event) {
  event->accept(K3URLDrag::canDecode(event));
}

void
MainWindow::dropEvent(QDropEvent* event) {
  KUrl::List urls;
  if (K3URLDrag::decode(event, urls)) {
//     kDebug() << "MainWindow:Handling QUriDrag..." << endl;
     if (urls.count() > 0) {
         const KUrl &url = urls.first();
         openUrl(url);
     }
  }
}
