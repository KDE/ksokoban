#include "LevelCollection.h"

#include "Map.h"

#include <QFile>
#include <stdio.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <ksharedconfig.h>
#include <kglobal.h>
#include <kconfiggroup.h>

static inline unsigned
forward(unsigned a, unsigned b, unsigned c, unsigned d)
{
  unsigned x=(a^b)&0xffffffffu;
  return (((x<<c)|(x>>((32u-c)&31u)))*d)&0xffffffffu;
}

static inline unsigned
backward(unsigned a, unsigned b, unsigned c, unsigned d)
{
  unsigned x=(a*b)&0xffffffffu;
  return (((x<<c)|(x>>((32u-c)&31u)))^d)&0xffffffffu;
}


void
LevelCollection::indexTextCollection() {
  enum states {
    BEFORE_NONE, BEFORE_VALID, BEFORE_INVALID,
    DURING_NONE, DURING_VALID, DURING_INVALID
  } state = BEFORE_NONE;

  int levelstart=0, levelend=0;
  for (int pos=0; pos<(data_.size()-1); pos++) {
    switch (state) {
    case BEFORE_NONE:
      switch (data_[pos]) {
      case '#': case '.': case '$': case '+': case '*': case '@':
	state = BEFORE_VALID;
	break;

      case ' ': case '\t': case '\r':
	break;

      case '\n':
	levelstart = pos + 1;
	break;

      default:
	state = BEFORE_INVALID;
	break;
      }
      break;

    case BEFORE_VALID:
      switch (data_[pos]) {
      case '#': case '.': case '$': case '+': case '*': case '@':
      case ' ': case '\t': case '\r':
	break;

      case '\n':
	addLevel(data_.constData() + levelstart);
	levelend = levelstart;
	state = DURING_NONE;
	break;

      default:
	state = BEFORE_INVALID;
	break;
      }
      break;

    case BEFORE_INVALID:
      switch (data_[pos]) {
      case '\n':
	levelstart = pos + 1;
	state = BEFORE_NONE;
	break;
      }
      break;

    case DURING_NONE:
      switch (data_[pos]) {
      case '#': case '.': case '$': case '+': case '*': case '@':
	state = DURING_VALID;
	break;

      case ' ': case '\t': case '\r':
	break;

      case '\n':
	data_[levelend] = '\0';
	levelstart = pos + 1;
	state = BEFORE_NONE;
	break;

      default:
	state = DURING_INVALID;
	break;
      }
      break;

    case DURING_VALID:
      switch (data_[pos]) {
      case '#': case '.': case '$': case '+': case '*': case '@':
      case ' ': case '\t': case '\r':
	break;

      case '\n':
	levelend = pos;
	state = DURING_NONE;
	break;

      default:
	state = DURING_INVALID;
	break;
      }
      break;

    case DURING_INVALID:
      switch (data_[pos]) {
      case '\n':
	data_[levelend] = '\0';
	levelstart = pos + 1;
	state = BEFORE_NONE;
	break;
      }
      break;

    default:
      assert(0);
    }
  }

  if (state==DURING_NONE || state==DURING_INVALID) {
    data_[levelend] = '\0';
  }
}

void
LevelCollection::loadPrefs() {
  if (id_ >= 0) {
    KConfigGroup cg(KGlobal::config(), "settings");

    QString key;
    key.sprintf("level%d", id_);
    level_ = cg.readEntry(key, 0);

    key.sprintf("status%d", id_);
    unsigned x = cg.readEntry(key, QVariant::fromValue(unsigned(0))).toUInt();

    x = backward(x, 0xc1136a15u, 0x12u, 0x80ff0b94u);
    x = backward(x, 0xd38fd2ddu, 0x01u, 0xd4d657b4u);
    x = backward(x, 0x59004eefu, 0x1eu, 0xf6c75e2cu);
    x = backward(x, 0x366c3e25u, 0x0au, 0x61ebc208u);
    x = backward(x, 0x20a784c9u, 0x15u, 0x207d488bu);
    x = backward(x, 0xc02864abu, 0x09u, 0x709e62a3u);
    x = backward(x, 0xe2a60f19u, 0x0eu, 0x8bb02c07u);
    x = backward(x, 0x3b0e11f3u, 0x13u, 0x608aef3fu);

    completedLevels_ = x>>16 & 0x3ff;
    if (!cg.hasKey(key)) completedLevels_ = 0;
    if (((x>>26) & 0x3fu) != static_cast<unsigned>(id_)) completedLevels_ = 0;
    if ((x & 0xffffu) != static_cast<unsigned>(getuid())) completedLevels_ = 0;
    if (completedLevels_ > noOfLevels_) completedLevels_ = 0;

    if (level_ > completedLevels_) level_ = completedLevels_;
    if (level_ >= noOfLevels_) level_ = noOfLevels_-1;
    if (level_ < 0) level_ = 0;
  } else {
    level_ = 0;
    completedLevels_ = noOfLevels_;
  }
}

void
LevelCollection::addLevel(const QString& _level) {
  index_.append(_level);
}

void
LevelCollection::addSeparator() {
  data_.append('\0');
}

LevelCollection::LevelCollection(const QByteArray& _def,
				 const QString &_name, int _id) :
  level_(0), completedLevels_(0), noOfLevels_(0),
  name_(_name), id_(_id) {

  data_.append(_def);
  addSeparator();

  indexTextCollection();

  noOfLevels_ = index_.size();

  loadPrefs();
}

LevelCollection::LevelCollection(const QString &_path, const QString &_name,
				 int _id) :
  level_(0), completedLevels_(0), noOfLevels_(0),
  name_(_name), path_(_path), id_(_id) 
{

  QFile file(path_);
  if (file.open(QIODevice::ReadOnly))
  {
    data_ = file.readAll();
    file.close();
    addSeparator();
  }

  indexTextCollection();

  noOfLevels_ = index_.size();

  loadPrefs();

}

LevelCollection::~LevelCollection() {
  if (id_ >= 0) {
    KConfigGroup cg(KGlobal::config(), "settings");

    QString key;
    key.sprintf("level%d", id_);
    cg.writeEntry(key, level_, KConfigBase::Normal);
  }
}


void
LevelCollection::levelCompleted() {
  if (completedLevels_ < (level_+1)) completedLevels_ = level_+1;

  if (id_ >= 0) {
    unsigned x=((static_cast<unsigned>(getuid())) & 0xffffu);
    x |= (static_cast<unsigned>(id_))<<26;
    x |= (static_cast<unsigned>(completedLevels_))<<16;

    x = forward(x, 0x608aef3fu, 0x0du, 0xfb00ef3bu);
    x = forward(x, 0x8bb02c07u, 0x12u, 0x2a37dd29u);
    x = forward(x, 0x709e62a3u, 0x17u, 0x23607603u);
    x = forward(x, 0x207d488bu, 0x0bu, 0xc31fd579u);
    x = forward(x, 0x61ebc208u, 0x16u, 0xbcffadadu);
    x = forward(x, 0xf6c75e2cu, 0x02u, 0xa2baa00fu);
    x = forward(x, 0xd4d657b4u, 0x1fu, 0x7e129575u);
    x = forward(x, 0x80ff0b94u, 0x0eu, 0x92fc153du);

    QString key;
    key.sprintf("status%d", id_);

    KConfigGroup cg(KGlobal::config(), "settings");
    cg.writeEntry(key, unsigned(x), KConfigBase::Normal);
  }
}


void
LevelCollection::level(int _level) {
  assert(_level >= 0 && _level < noOfLevels_);

  level_ = _level;
  if (level_ > completedLevels_) level_ = completedLevels_;
  if (level_ >= noOfLevels_) level_ = noOfLevels_ - 1;
  if (level_ < 0) level_ = 0;
}

static int
minX(const QString& def) {
  int min_x = 10000;

  int x=0;
  for (int pos=0; pos < def.length(); pos++) {
    if(def[pos] == '\n')
      x = 0;
    else if(def[pos] == ' ')
      x++;
    else if(def[pos] == '\t')
      x = (x+8) & ~7;
    else if(def[pos] == '\r')
    {
        //no-op
    }
    else
    {
      if (x < min_x) 
          min_x = x;
    }
  }

  return min_x == 10000 ? -1 : min_x;
}


bool
LevelCollection::loadLevel(Map *_map) {
  _map->clearMap();

  QString def = index_[level_];
  bool goodMap = true;
  int x=0, y=0, goalsLeft=0;

  int min_x = minX(def);
  if (min_x < 0) {
    min_x = 0;
    goodMap = false;
  }


  _map->xpos_ = -1;
  _map->ypos_ = -1;

  for (int pos=0; pos<def.length(); pos++) {
    if(def[pos] == '\n')
    {
        y++;
        x = 0;
    }
    else if(def[pos] == ' ')
    {
        x++;
    }
    else if(def[pos] == '\t')
    {
        x = (x+8) & ~7;
    }
    else if(def[pos] == '@')
    {
        if (x-min_x > MAX_X || y > MAX_Y) goodMap = false;
        else {
            _map->xpos_ = x-min_x;
            _map->ypos_ = y;
        }
        x++;
    }
    else if(def[pos] == '$')
    {
        if (x-min_x > MAX_X || y > MAX_Y) goodMap = false;
        else _map->map(x-min_x, y, OBJECT);
        x++;
    }
    else if(def[pos] == '.')
    {
      if (x-min_x > MAX_X || y > MAX_Y) goodMap = false;
      else {
          _map->map(x-min_x, y, GOAL);
          goalsLeft++;
      }
      x++;
    }
    else if(def[pos] == '#')
    {
        if (x-min_x > MAX_X || y > MAX_Y) goodMap = false;
        else _map->map(x-min_x, y, WALL);
        x++;
    }
    else if(def[pos] == '+')
    {
        if (x-min_x > MAX_X || y > MAX_Y) goodMap = false;
        else {
            _map->xpos_ = x-min_x;
            _map->ypos_ = y;
            _map->map(x-min_x, y, GOAL);
            goalsLeft++;
        }
        x++;
    }
    else if(def[pos] == '*')
    {
        if (x-min_x > MAX_X || y > MAX_Y) goodMap = false;
        else _map->map(x-min_x, y, OBJECT|GOAL);
        x++;
    }
    else if(def[pos] == '\r')
    {
        //no-op
    }
    else
    {
        goodMap = false;
    }
  }

  if (_map->objectsLeft() != goalsLeft) goodMap = false;
  if (_map->completed()) goodMap = false;

  if (_map->badCoords(_map->xpos_, _map->ypos_)) goodMap = false;
  else {
    if (!_map->empty(_map->xpos_, _map->ypos_)) goodMap = false;
    else if (!_map->fillFloor(_map->xpos_, _map->ypos_)) goodMap = false;
  }

  return goodMap;
}

