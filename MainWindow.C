/*
 *  ksokoban - a Sokoban game for KDE
 *  Copyright (C) 1998  Anders Widell  <d95-awi@nada.kth.se>
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdio.h>

#include <kapp.h>
#include <kconfig.h>
#include <ktopwidget.h>
#include <kmenubar.h>
#include <qpopupmenu.h>
#include <qkeycode.h>
#include <assert.h>
#include <klocale.h>
#include <qstring.h>
#include <kfiledialog.h>
#include <qframe.h>
#include <qmessagebox.h>

#include "MainWindow.H"
#include "PlayField.H"
#include "LevelCollection.H"

#include "MainWindow.moc"

void
MainWindow::createCollectionMenu() {
  collection_ = new QPopupMenu(0,"collection menu");
  collection_->setCheckable(true);
  //connect(collection_, SIGNAL(activated(int)), playField_, SLOT(changeCollection(int)));
  connect(collection_, SIGNAL(activated(int)), this, SLOT(changeCollection(int)));

  for (int i=0; i<internalCollections_.collections(); i++) {
    collection_->insertItem(internalCollections_[i]->name(), i);
  }
  checkedCollection_ = 0;

  KConfig *cfg=(KApplication::getKApplication())->getConfig();
  cfg->setGroup("settings");
  int id = cfg->readNumEntry("collection", 0);

  currentCollection_ = 0;
  for (int i=0; i<internalCollections_.collections(); i++) {
    if (internalCollections_[i]->id() == id) currentCollection_ = i;
  }

  changeCollection(currentCollection_);
}


MainWindow::MainWindow() : KTMainWindow(), externalCollection_(0) {
  int i;

  KConfig *cfg=(KApplication::getKApplication())->getConfig();
  cfg->setGroup("Geometry");
  int width = cfg->readNumEntry("width", 600);
  int height = cfg->readNumEntry("height", 450);
  resize(width, height);

  playField_ = new PlayField(this, "playfield");
  setView(playField_, FALSE);
  playField_->show();

  menu_ = new KMenuBar(this, "menubar" );

  game_ = new QPopupMenu(0,"game menu");
  game_->insertItem(i18n("&Load levels..."), this, SLOT(loadLevels()));
  game_->insertItem(i18n("&Next level"), playField_, SLOT(nextLevel()), Key_N);
  game_->insertItem(i18n("&Previous level"), playField_, SLOT(previousLevel()), Key_P);
  game_->insertItem(i18n("Re&start level"), playField_, SLOT(restartLevel()), Key_Escape);

  createCollectionMenu();
  game_->insertItem(i18n("&Level collection"), collection_);

  game_->insertItem(i18n("&Undo"), playField_, SLOT(undo()), Key_U);
  game_->insertItem(i18n("&Redo"), playField_, SLOT(redo()), Key_R);
  game_->insertSeparator();
  game_->insertItem(i18n("&Quit"), KApplication::getKApplication(), SLOT(quit()), Key_Q);
  menu_->insertItem(i18n("&Game"), game_);

  animation_ = new QPopupMenu(0,"animation menu");
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

  bookmarkMenu_ = new QPopupMenu(0,"bookmarks menu");
  setBM_ = new QPopupMenu(0, "set bookmark menu");
  setBM_->insertItem(i18n("(unused)"), 1);
  setBM_->setAccel(CTRL+Key_1, 1);
  setBM_->insertItem(i18n("(unused)"), 2);
  setBM_->setAccel(CTRL+Key_2, 2);
  setBM_->insertItem(i18n("(unused)"), 3);
  setBM_->setAccel(CTRL+Key_3, 3);
  setBM_->insertItem(i18n("(unused)"), 4);
  setBM_->setAccel(CTRL+Key_4, 4);
  setBM_->insertItem(i18n("(unused)"), 5);
  setBM_->setAccel(CTRL+Key_5, 5);
  setBM_->insertItem(i18n("(unused)"), 6);
  setBM_->setAccel(CTRL+Key_6, 6);
  setBM_->insertItem(i18n("(unused)"), 7);
  setBM_->setAccel(CTRL+Key_7, 7);
  setBM_->insertItem(i18n("(unused)"), 8);
  setBM_->setAccel(CTRL+Key_8, 8);
  setBM_->insertItem(i18n("(unused)"), 9);
  setBM_->setAccel(CTRL+Key_9, 9);
  setBM_->insertItem(i18n("(unused)"), 10);
  setBM_->setAccel(CTRL+Key_0, 10);
  connect(setBM_, SIGNAL(activated(int)), this, SLOT(setBookmark(int)));
  bookmarkMenu_->insertItem(i18n("&Set bookmark"), setBM_);

  goToBM_ = new QPopupMenu(0, "go to bookmark menu");
  goToBM_->insertItem(i18n("(unused)"), 1);
  goToBM_->setAccel(Key_1, 1);
  goToBM_->insertItem(i18n("(unused)"), 2);
  goToBM_->setAccel(Key_2, 2);
  goToBM_->insertItem(i18n("(unused)"), 3);
  goToBM_->setAccel(Key_3, 3);
  goToBM_->insertItem(i18n("(unused)"), 4);
  goToBM_->setAccel(Key_4, 4);
  goToBM_->insertItem(i18n("(unused)"), 5);
  goToBM_->setAccel(Key_5, 5);
  goToBM_->insertItem(i18n("(unused)"), 6);
  goToBM_->setAccel(Key_6, 6);
  goToBM_->insertItem(i18n("(unused)"), 7);
  goToBM_->setAccel(Key_7, 7);
  goToBM_->insertItem(i18n("(unused)"), 8);
  goToBM_->setAccel(Key_8, 8);
  goToBM_->insertItem(i18n("(unused)"), 9);
  goToBM_->setAccel(Key_9, 9);
  goToBM_->insertItem(i18n("(unused)"), 10);
  goToBM_->setAccel(Key_0, 10);
  connect(goToBM_, SIGNAL(activated(int)), this, SLOT(goToBookmark(int)));
  bookmarkMenu_->insertItem(i18n("&Go to bookmark"), goToBM_);
  menu_->insertItem(i18n("&Bookmarks"), bookmarkMenu_);

  for (i=1; i<=10; i++) {
    bookmarks_[i-1] = new Bookmark(i);
    updateBookmark(i);
  }

  KApplication *app = KApplication::getKApplication();
  QString aboutMsg = "ksokoban 0.3.0 - ";
  aboutMsg += i18n("\
a Sokoban game for KDE\n\
\n\
ksokoban is copyright (c) 1998-1999 Anders Widell <awl@hem.passagen.se>\n\
See http://hem.passagen.se/awl/ksokoban/ for more info on ksokoban\n\
\n\
The levels are copyrighted by their authors:\n\
\n\
Original - the 50 original sokoban levels made by Thinking rabbit Inc.\n\
Extra - some levels from xsokoban\n\
Still more - by J. Franklin Mentzer <wryter@aol.com>\n\
MacTommy inventions - by Tommy in Pennsylvania\n\
Yoshio's autogenerated - by Yoshio Murase <yoshio@asahi.email.ne.jp>\n\
For the kids - by Ross (W.R.) Brown <ross@bnr.ca>\n\
Dimitri & Yorick - by Jacques Duthen <duthen@club-internet.fr>\n\
Simple Sokoban - by Phil Shapiro <pshapiro@his.com>\n\
");
  help_ = app->getHelpMenu(true, aboutMsg.data());
  menu_->insertSeparator();
  menu_->insertItem(i18n("&Help"), help_);

  setMenu(menu_);
  menu_->show();

  updateRects();
}

MainWindow::~MainWindow()
{
  KConfig *cfg=(KApplication::getKApplication())->getConfig();

  cfg->setGroup("Geometry");
  cfg->writeEntry("width", width());
  cfg->writeEntry("height", height());

  cfg->setGroup("settings");
  cfg->writeEntry("collection", internalCollections_[checkedCollection_]->id());

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
  int col = bookmarks_[num-1]->collection();
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

  setBM_->changeItem(name, num);
  goToBM_->changeItem(name, num);
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
  int collection = bm->collection();
  int level = bm->level();
  if (collection < 0 || collection >= internalCollections_.collections()) return;
  if (level < 0 || level >= internalCollections_[collection]->noOfLevels()) return;
  if (level > internalCollections_[collection]->completedLevels()) return;

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
  KConfig *cfg=(KApplication::getKApplication())->getConfig();
  cfg->setGroup("settings");
  QString lastFile = cfg->readEntry("lastLevelFile");

  QString result = KFileDialog::getOpenFileName(lastFile, "*", this, i18n("Load levels from file"));
  if (result.isNull()) return;

  cfg->setGroup("settings");
  cfg->writeEntry("lastLevelFile", result);

  int namepos = result.findRev('/') + 1; // NOTE: findRev can return -1

  LevelCollection *tmpCollection = new LevelCollection(result, result.mid(namepos, result.length()));
  if (tmpCollection->noOfLevels() < 1) {
    QMessageBox::warning(this, "Ksokoban", i18n("No levels found in file"), i18n("OK"));

    delete tmpCollection;

    return;
  }
  delete externalCollection_;
  externalCollection_ = tmpCollection;

  collection_->setItemChecked(checkedCollection_, false);
  playField_->changeCollection(externalCollection_);
}
