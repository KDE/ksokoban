#include "ModalLabel.H"

#include <qlabel.h>
#include <qfont.h>
//#include <qpainter.h>
//#include <qdrawutil.h>
#include <kapp.h>
#include <qwidcoll.h>
#include <string.h>

#include "ModalLabel.moc"

static char *
my_strchr (char *s, int c) {
  while (*s) {
    if (*s == c) return s;
    s++;
  }
  return 0;
}

void
ModalLabel::checkBounds (QFontMetrics &met, char *str)
{
  QRect bounds = met.boundingRect (str);
  if (bounds.width () > width_) width_ = bounds.width ();
  height_ += bounds.height ();
}


ModalLabel::ModalLabel (const char *text, QWidget *parent, const char * name, WFlags f)
  : QLabel (text, parent, name, f) {
  QFont font ("helvetica", 24, QFont::Bold);
  QFontMetrics fontMet (font);

  char *str = new char[strlen(text)+1];
  strcpy (str, text);
  char *str_pos=str;
  width_ = height_ = 0;
  for (;;) {
    char *next_line = my_strchr (str_pos, '\n');
    if (next_line) *next_line = '\0';
    checkBounds (fontMet, str_pos);

    if (next_line) str_pos = next_line+1;
    else break;
  }
  delete [] str;

  width_ += 32;
  height_ += 32;

  if (width_ < 300) width_ = 300;
  if (height_ < 75) height_ = 75;

  setAlignment (AlignCenter);
  setFrameStyle (QFrame::Panel | QFrame::Raised);
  setLineWidth (4);
  setFont (font);
  move (parent->width ()/2 - width_/2, parent->height ()/2 - height_/2);
  resize (width_, height_);
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
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::Accel:
      //case Event_DragEnter:
    case QEvent::DragMove:
    case QEvent::DragLeave:
    case QEvent::Drop:
      //case Event_DragResponse:
      //debug("Ate event");
    return true;
    break;
  default:
    return false;
  }
}

void
ModalLabel::message (const char *text, QWidget *parent) {
  KApplication *app = KApplication::getKApplication ();
  ModalLabel cl (text, parent);

  while (!cl.completed_) app->processOneEvent ();
}
