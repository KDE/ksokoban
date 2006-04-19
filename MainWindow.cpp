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

#include <stdio.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kmenubar.h>
#include <q3popupmenu.h>
#include <qnamespace.h>
//Added by qt3to4:
#include <QPixmap>
#include <QDragEnterEvent>
#include <QFocusEvent>
#include <QDropEvent>
#include <assert.h>
#include <klocale.h>
#include <qstring.h>
#include <kfiledialog.h>
#include <q3frame.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kio/netaccess.h>
#include <kiconloader.h>
#include <qicon.h>
#include <q3dragobject.h>
#include <kmenu.h>
#include <k3urldrag.h>
#include <kstdaccel.h>

#include "MainWindow.h"
#include "PlayField.h"
#include "LevelCollection.h"

#include "MainWindow.moc"

void
MainWindow::createCollectionMenu() {
  collection_ = new Q3PopupMenu(0,"collection menu");
  collection_->setCheckable(true);
  //connect(collection_, SIGNAL(activated(int)), playField_, SLOT(changeCollection(int)));
  connect(collection_, SIGNAL(activated(int)), this, SLOT(changeCollection(int)));

  for (int i=0; i<internalCollections_.collections(); i++) {
    collection_->insertItem(internalCollections_[i]->name(), i);
  }
  checkedCollection_ = 0;

  KConfigGroup settings(KGlobal::config(), "settings");
  int id = settings.readEntry("collection", 10);

  currentCollection_ = 0;
  for (int i=0; i<internalCollections_.collections(); i++) {
    if (internalCollections_[i]->id() == id) currentCollection_ = i;
  }

  changeCollection(currentCollection_);
}


MainWindow::MainWindow() : KMainWindow(0), externalCollection_(0) {
  int i;
  QPixmap pixmap;

  QPalette palette;
  palette.setColor( backgroundRole(), Qt::black );
  setPalette( palette );

  KConfigGroup cfg(KGlobal::config(), "Geometry");
  int width = cfg.readEntry("width", 750);
  int height = cfg.readEntry("height", 562);
  resize(width, height);

  playField_ = new PlayField(this);
  playField_->setObjectName( "playfield" );
  setCentralWidget(playField_);
  playField_->show();

  menu_ = new KMenuBar(this);
  menu_->setObjectName("menubar" );

  game_ = new Q3PopupMenu(0);
  pixmap = SmallIcon("fileopen");
  game_->insertItem(QIcon(pixmap), i18n("&Load Levels..."), this, SLOT(loadLevels()));
  pixmap = SmallIcon("forward");
  game_->insertItem(QIcon(pixmap), i18n("&Next Level"), playField_, SLOT(nextLevel()), Qt::Key_N);
  pixmap = SmallIcon("back");
  game_->insertItem(QIcon(pixmap), i18n("&Previous Level"), playField_, SLOT(previousLevel()), Qt::Key_P);
  pixmap = SmallIcon("reload");
  game_->insertItem(QIcon(pixmap), i18n("Re&start Level"), playField_, SLOT(restartLevel()), Qt::Key_Escape);

  createCollectionMenu();
  game_->insertItem(i18n("&Level Collection"), collection_);

  pixmap = SmallIcon("undo");
  game_->insertItem(QIcon(pixmap), i18n("&Undo"), playField_, SLOT(undo()),QKeySequence( (KStdAccel::undo()).toString()));
  pixmap = SmallIcon("redo");
  game_->insertItem(QIcon(pixmap), i18n("&Redo"), playField_, SLOT(redo()), QKeySequence( (KStdAccel::redo()).toString()));
  game_->insertSeparator();
  pixmap = SmallIcon("exit");
  game_->insertItem(QIcon(pixmap), i18n("&Quit"), KApplication::kApplication(), SLOT(closeAllWindows()), QKeySequence( (KStdAccel::quit()).toString()));
  menu_->insertItem(i18n("&Game"), game_);

  animation_ = new Q3PopupMenu(0,"animation menu");
  animation_->setCheckable(true);
  connect(animation_, SIGNAL(activated(int)), this, SLOT(updateAnimMenu(int)));
  connect(animation_, SIGNAL(activated(int)), playField_, SLOT(changeAnim(int)));
  animation_->insertItem(i18n("&Slow"), 3);
  animation_->insertItem(i18n("&Medium"), 2);
  animation_->insertItem(i18n("&Fast"), 1);
  animation_->insertItem(i18n("&Off"), 0);
  checkedAnim_ = playField_->animDelay();
  updateAnimMenu(checkedAnim_);
  menu_->insertItem(i18n("&Animation"), animation_);

  pixmap = SmallIcon("bookmark_add");
  bookmarkMenu_ = new Q3PopupMenu(0,"bookmarks menu");
  setBM_ = new Q3PopupMenu(0, "set bookmark menu");
  setBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 1);
  setBM_->setAccel(Qt::CTRL+Qt::Key_1, 1);
  setBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 2);
  setBM_->setAccel(Qt::CTRL+Qt::Key_2, 2);
  setBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 3);
  setBM_->setAccel(Qt::CTRL+Qt::Key_3, 3);
  setBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 4);
  setBM_->setAccel(Qt::CTRL+Qt::Key_4, 4);
  setBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 5);
  setBM_->setAccel(Qt::CTRL+Qt::Key_5, 5);
  setBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 6);
  setBM_->setAccel(Qt::CTRL+Qt::Key_6, 6);
  setBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 7);
  setBM_->setAccel(Qt::CTRL+Qt::Key_7, 7);
  setBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 8);
  setBM_->setAccel(Qt::CTRL+Qt::Key_8, 8);
  setBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 9);
  setBM_->setAccel(Qt::CTRL+Qt::Key_9, 9);
  setBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 10);
  setBM_->setAccel(Qt::CTRL+Qt::Key_0, 10);
  connect(setBM_, SIGNAL(activated(int)), this, SLOT(setBookmark(int)));
  bookmarkMenu_->insertItem(i18n("&Set Bookmark"), setBM_);

  pixmap = SmallIcon("bookmark");
  goToBM_ = new Q3PopupMenu(0, "go to bookmark menu");
  goToBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 1);
  goToBM_->setAccel(Qt::Key_1, 1);
  goToBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 2);
  goToBM_->setAccel(Qt::Key_2, 2);
  goToBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 3);
  goToBM_->setAccel(Qt::Key_3, 3);
  goToBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 4);
  goToBM_->setAccel(Qt::Key_4, 4);
  goToBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 5);
  goToBM_->setAccel(Qt::Key_5, 5);
  goToBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 6);
  goToBM_->setAccel(Qt::Key_6, 6);
  goToBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 7);
  goToBM_->setAccel(Qt::Key_7, 7);
  goToBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 8);
  goToBM_->setAccel(Qt::Key_8, 8);
  goToBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 9);
  goToBM_->setAccel(Qt::Key_9, 9);
  goToBM_->insertItem(QIcon(pixmap), i18n("(unused)"), 10);
  goToBM_->setAccel(Qt::Key_0, 10);
  connect(goToBM_, SIGNAL(activated(int)), this, SLOT(goToBookmark(int)));
  bookmarkMenu_->insertItem(i18n("&Go to Bookmark"), goToBM_);
  menu_->insertItem(i18n("&Bookmarks"), bookmarkMenu_);

  for (i=1; i<=10; i++) {
    bookmarks_[i-1] = new Bookmark(i);
    updateBookmark(i);
  }

  help_ = helpMenu(QString::null, false);
  menu_->insertSeparator();
  menu_->insertItem(i18n("&Help"), help_);

  menu_->show();

  setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
  KConfigGroup cfg(KGlobal::config(), "Geometry");

  cfg.writeEntry("width", width());
  cfg.writeEntry("height", height());

  KConfigGroup settings(KGlobal::config(), "settings");
  settings.writeEntry("collection", internalCollections_[checkedCollection_]->id());

  for (int i=1; i<=10; i++) {
    delete bookmarks_[i-1];
  }


  delete externalCollection_;

  // The following line segfaults when linked against qt 1.44
  //delete help_;
  delete goToBM_;
  delete setBM_;
  delete bookmarkMenu_;
  delete animation_;
  delete collection_;
  delete game_;
  //delete menu_;

  //delete playField_;
}



void
MainWindow::focusInEvent(QFocusEvent *) {
  playField_->setFocus();
}

void
MainWindow::updateAnimMenu(int id) {
  animation_->setItemChecked(checkedAnim_, false);
  checkedAnim_ = id;
  animation_->setItemChecked(checkedAnim_, true);
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

#warning port to QAction
  setBM_->changeItem(num, name);
  goToBM_->changeItem(num, name);
}

void
MainWindow::setBookmark(int id) {
  assert(id >= 1 && id <= 10);
  playField_->setBookmark(bookmarks_[id-1]);
  updateBookmark(id);
}

void
MainWindow::goToBookmark(int id) {
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
  collection_->setItemChecked(checkedCollection_, false);
  checkedCollection_ = id;
  collection_->setItemChecked(checkedCollection_, true);

  delete externalCollection_;
  externalCollection_ = 0;
  playField_->changeCollection(internalCollections_[id]);
}

void
MainWindow::loadLevels() {
  KConfig *cfg=KGlobal::config();
  cfg->setGroup("settings");
  QString lastFile = cfg->readPathEntry("lastLevelFile");

  KUrl result = KFileDialog::getOpenURL(lastFile, "*", this, i18n("Load Levels From File"));
  if (result.isEmpty()) return;

  openURL(result);
}

void
MainWindow::openURL(KUrl _url) {
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

  collection_->setItemChecked(checkedCollection_, false);
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
         openURL(url);
     }
  }
}
