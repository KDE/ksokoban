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

#include <kapp.h>
#include <ktopwidget.h>
#include <kmenubar.h>
#include <qpopupmenu.h>
#include <qkeycode.h>
#include <kstatusbar.h>
#include <qlabel.h>
#include <assert.h>

#include "MainWindow.H"
#include "PlayField.H"

#include "MainWindow.moc"

MainWindow::MainWindow() : KTopLevelWidget() {
  int i;

  playField_ = new PlayField (this, "pf");
  setView (playField_);

  menu_ = new KMenuBar(this, "_mainMenu" );
  game_ = new QPopupMenu(0,"fgame");
  game_->insertItem (i18n("&Next level"), playField_, SLOT(nextLevel()), Key_N);
  game_->insertItem (i18n("&Previous level"), playField_, SLOT(previousLevel()), Key_P);
  game_->insertItem (i18n("Re&start level"), playField_, SLOT(restartLevel()), Key_Escape);

  collection_ = new QPopupMenu(0,"fcollection");
  collection_->setCheckable (true);
  connect (collection_, SIGNAL(activated(int)), playField_, SLOT(changeCollection(int)));
  connect (collection_, SIGNAL(activated(int)), this, SLOT(updateCollectionMenu(int)));
  for (i=0; i<playField_->noOfCollections (); i++) {
    collection_->insertItem (playField_->collectionName (i), i);
  }
  checkedCollection_=0;
  updateCollectionMenu (playField_->collection ());
  game_->insertItem (i18n("&Level collection"), collection_);

  game_->insertItem (i18n("&Undo"), playField_, SLOT(undo()), Key_U);
  game_->insertItem (i18n("&Redo"), playField_, SLOT(redo()), Key_R);
  game_->insertSeparator ();
  game_->insertItem (i18n("&Quit"), KApplication::getKApplication (), SLOT(quit()), Key_Q);
  menu_->insertItem(i18n("&Game"), game_);

  graphics_ = new QPopupMenu(0,"fgraphics");
  graphics_->setCheckable (true);
  connect (graphics_, SIGNAL(activated(int)), this, SLOT(changeGfx(int)));
  graphics_->insertItem (i18n("&Small"), 0);
  graphics_->insertItem (i18n("&Medium"), 1);
  graphics_->insertItem (i18n("&Large"), 2);
  checkedGfx_=playField_->resolution ();
  updateGfxMenu (checkedGfx_);
  menu_->insertItem(i18n("G&raphics"), graphics_);

  animation_ = new QPopupMenu(0,"fanimation");
  animation_->setCheckable (true);
  connect (animation_, SIGNAL(activated(int)), this, SLOT(updateAnimMenu(int)));
  connect (animation_, SIGNAL(activated(int)), playField_, SLOT(changeAnim(int)));
  animation_->insertItem (i18n("&Slow"), 3);
  animation_->insertItem (i18n("&Medium"), 2);
  animation_->insertItem (i18n("&Fast"), 1);
  animation_->insertItem (i18n("&Off"), 0);
  checkedAnim_ = playField_->animDelay ();
  updateAnimMenu (checkedAnim_);
  menu_->insertItem(i18n("&Animation"), animation_);

  bookmarkMenu_ = new QPopupMenu(0,"fbookmarks");
  setBM_ = new QPopupMenu(0, "fsetbm");
  setBM_->insertItem (i18n("(unused)"), 1);
  setBM_->setAccel (CTRL+Key_1, 1);
  setBM_->insertItem (i18n("(unused)"), 2);
  setBM_->setAccel (CTRL+Key_2, 2);
  setBM_->insertItem (i18n("(unused)"), 3);
  setBM_->setAccel (CTRL+Key_3, 3);
  setBM_->insertItem (i18n("(unused)"), 4);
  setBM_->setAccel (CTRL+Key_4, 4);
  setBM_->insertItem (i18n("(unused)"), 5);
  setBM_->setAccel (CTRL+Key_5, 5);
  setBM_->insertItem (i18n("(unused)"), 6);
  setBM_->setAccel (CTRL+Key_6, 6);
  setBM_->insertItem (i18n("(unused)"), 7);
  setBM_->setAccel (CTRL+Key_7, 7);
  setBM_->insertItem (i18n("(unused)"), 8);
  setBM_->setAccel (CTRL+Key_8, 8);
  setBM_->insertItem (i18n("(unused)"), 9);
  setBM_->setAccel (CTRL+Key_9, 9);
  setBM_->insertItem (i18n("(unused)"), 10);
  setBM_->setAccel (CTRL+Key_0, 10);
  connect (setBM_, SIGNAL(activated(int)), this, SLOT(setBookmark(int)));
  bookmarkMenu_->insertItem (i18n("&Set bookmark"), setBM_);

  goToBM_ = new QPopupMenu(0, "fgotobm");
  goToBM_->insertItem (i18n("(unused)"), 1);
  goToBM_->setAccel (Key_1, 1);
  goToBM_->insertItem (i18n("(unused)"), 2);
  goToBM_->setAccel (Key_2, 2);
  goToBM_->insertItem (i18n("(unused)"), 3);
  goToBM_->setAccel (Key_3, 3);
  goToBM_->insertItem (i18n("(unused)"), 4);
  goToBM_->setAccel (Key_4, 4);
  goToBM_->insertItem (i18n("(unused)"), 5);
  goToBM_->setAccel (Key_5, 5);
  goToBM_->insertItem (i18n("(unused)"), 6);
  goToBM_->setAccel (Key_6, 6);
  goToBM_->insertItem (i18n("(unused)"), 7);
  goToBM_->setAccel (Key_7, 7);
  goToBM_->insertItem (i18n("(unused)"), 8);
  goToBM_->setAccel (Key_8, 8);
  goToBM_->insertItem (i18n("(unused)"), 9);
  goToBM_->setAccel (Key_9, 9);
  goToBM_->insertItem (i18n("(unused)"), 10);
  goToBM_->setAccel (Key_0, 10);
  connect (goToBM_, SIGNAL(activated(int)), this, SLOT(goToBookmark(int)));
  bookmarkMenu_->insertItem (i18n("&Go to bookmark"), goToBM_);
  menu_->insertItem (i18n("&Bookmarks"), bookmarkMenu_);

  for (i=1; i<=10; i++) {
    bookmarks_[i-1] = new Bookmark (i);
    updateBookmark (i);
  }

  KApplication *app = KApplication::getKApplication ();
  QString aboutMsg = "ksokoban 0.2.2 - ";
  aboutMsg += i18n("\
a Sokoban game for KDE\n\
\n\
ksokoban is copyright (c) 1998 Anders Widell <d95-awi@nada.kth.se>\n\
See http://hem.passagen.se/awl/ksokoban/ for more info on ksokoban\n\
\n\
The levels are copyrighted by their authors:\n\
\n\
Original - the first 50 original levels from xsokoban\n\
Extra - some additional levels from xsokoban\n\
Still more - by J. Franklin Mentzer <wryter@aol.com>\n\
MacTommy inventions - by Tommy in Pennsylvania\n\
Yoshio's autogenerated - by Yoshio Murase <yoshio@asahi.email.ne.jp>\n\
For the kids - by Ross (W.R.) Brown <ross@bnr.ca>\n\
Dimitri & Yorick - by Jacques Duthen <duthen@club-internet.fr>\n\
Simple Sokoban - by Phil Shapiro <pshapiro@his.com>\n\
");
  help_ = app->getHelpMenu (true, aboutMsg.data ());
  menu_->insertSeparator ();
  menu_->insertItem(i18n("&Help"), help_);

  setMenu(menu_);
  menu_->show();

  statusBar_ = new KStatusBar (this, "statusBar");
  statusBar_->setInsertOrder (KStatusBar::RightToLeft);
  statusBar_->setBorderWidth (3);
  //collectionLabel_ = new QLabel ("None           ", statusBar_);
  //statusBar_->insertWidget (collectionLabel_, 20, 2);
  //levelLabel_ = new QLabel ("   3", statusBar_);
  //statusBar_->insertWidget (levelLabel_, 20, 1);
  QString str = i18n("Pushes:");
  statusBar_->insertItem ((const char *) (str +" 00000"), 4);
  str = i18n("Moves:");
  statusBar_->insertItem ((const char *) (str + " 00000"), 3);
  str = i18n("Level:");
  statusBar_->insertItem ((const char *) (str + " 0000"), 2);
  statusBar_->insertItem ("#########################", 1);

  connect (playField_, SIGNAL(levelChanged(const char *)), this, SLOT(changeLevel(const char *)));
  connect (playField_, SIGNAL(collectionChanged(const char *)), this, SLOT(changeCollection(const char *)));
  connect (playField_, SIGNAL(movesChanged(const char *)), this, SLOT(changeMoves(const char *)));
  connect (playField_, SIGNAL(pushesChanged(const char *)), this, SLOT(changePushes(const char *)));

  setStatusBar (statusBar_);
  playField_->emitAll ();

  //resize (playField_->size ());
  //playField_->setSize ();
  updateRects ();
  playField_->setSize ();
  updateRects ();
}

MainWindow::~MainWindow()
{
  delete help_;

  for (int i=1; i<=10; i++) {
    delete bookmarks_[i-1];
  }

  delete goToBM_;
  delete setBM_;
  delete bookmarkMenu_;
  delete animation_;
  delete graphics_;
  delete collection_;
  delete game_;
  delete menu_;
  delete playField_;
}



void
MainWindow::focusInEvent (QFocusEvent *) {
  playField_->setFocus ();
}

void
MainWindow::updateGfxMenu (int id) {
  graphics_->setItemChecked (checkedGfx_, false);
  checkedGfx_ = id;
  graphics_->setItemChecked (checkedGfx_, true);
}

void
MainWindow::updateAnimMenu (int id) {
  animation_->setItemChecked (checkedAnim_, false);
  checkedAnim_ = id;
  animation_->setItemChecked (checkedAnim_, true);
}

void
MainWindow::updateBookmark (int num) {
  int col = bookmarks_[num-1]->collection ();
  int lev = bookmarks_[num-1]->level ();
  int mov = bookmarks_[num-1]->moves ();

  if (col < 0 || lev < 0) return;

  QString name;
  if (col >= 0 && col < playField_->noOfCollections ())
    name = playField_->collectionName (col);
  else
    name = i18n("(invalid)");
  QString l;
  l.setNum (lev+1);
  name += " #" + l;
  l.setNum (mov);
  name += " (" + l + ")";

  setBM_->changeItem (name, num);
  goToBM_->changeItem (name, num);
}

void
MainWindow::setBookmark (int id) {
  assert (id >= 1 && id <= 10);
  playField_->setBookmark (bookmarks_[id-1]);
  updateBookmark (id);
}

void
MainWindow::goToBookmark (int id) {
  assert (id >= 1 && id <= 10);
  playField_->goToBookmark (bookmarks_[id-1]);
}

void
MainWindow::changeGfx (int id) {
  updateGfxMenu (id);
  playField_->resolution (id);
  updateRects ();
}

void
MainWindow::updateCollectionMenu (int id)
{
  collection_->setItemChecked (checkedCollection_, false);
  checkedCollection_ = id;
  collection_->setItemChecked (checkedCollection_, true);
}

void
MainWindow::changeCollection (const char *text) {
  statusBar_->changeItem (text, 1);
}

void
MainWindow::changeLevel (const char *text) {
  statusBar_->changeItem (text, 2);
}

void
MainWindow::changeMoves (const char *text) {
  statusBar_->changeItem (text, 3);
}

void
MainWindow::changePushes (const char *text) {
  statusBar_->changeItem (text, 4);
}

