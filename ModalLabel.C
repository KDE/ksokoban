#include "ModalLabel.H"

#include <qlabel.h>
#include <qfont.h>
//#include <qpainter.h>
//#include <qdrawutil.h>
#include <kapp.h>
#include <qwidcoll.h>

#include "ModalLabel.moc.C"

ModalLabel::ModalLabel (const char *text, QWidget *parent, const char * name, WFlags f)
  : QLabel (text, parent, name, f) {
  QFont font ("helvetica", 24, QFont::Bold);

  setAlignment (AlignCenter);
  setFrameStyle (QFrame::Panel | QFrame::Raised);
  setLineWidth (4);
  setFont (font);
  move (parent->width ()/2-150, parent->height ()/2-37);
  resize (300, 75);
  show ();

  QWidgetList  *list = QApplication::allWidgets();
  QWidgetListIt it( *list );
  while ( it.current() ) {
    it.current()->installEventFilter (this);
    ++it;
  }
  delete list;

  completed_ = false;
  startTimer (1000);
}

#if 0
void
ModalLabel::paintEvent (QPaintEvent *) {
  QPainter p (this);

  p.drawText (0, 0, width (), height (), AlignCenter, text_);
  qDrawShadePanel (&p, 0, 0, width (), height (), colorGroup (), false, 4, &p.brush ());
}
#endif

void
ModalLabel::timerEvent (QTimerEvent *) {
  completed_ = true;
}

bool
ModalLabel::eventFilter (QObject *, QEvent *e) {
  switch (e->type()) {
    case Event_MouseButtonPress:
    case Event_MouseButtonRelease:
    case Event_MouseButtonDblClick:
    case Event_MouseMove:
    case Event_KeyPress:
    case Event_KeyRelease:
    case Event_Accel:
      //case Event_DragEnter:
    case Event_DragMove:
    case Event_DragLeave:
    case Event_Drop:
      //case Event_DragResponse:
      //debug("Ate event");
    return true;
    break;
  }
  return false;
}

void
ModalLabel::message (const char *text, QWidget *parent) {
  KApplication *app = KApplication::getKApplication ();
  ModalLabel cl (text, parent);

  while (!cl.completed_) app->processOneEvent ();
}
