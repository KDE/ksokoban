#include "LevelCollection.h"

#include "Map.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include <QFile>

#include <unistd.h>

#include <cassert>
#include <cstdio>
#include <cstring>

#if 1
#define GETUID() 501
#else
#ifndef WIN32
#define GETUID() getuid()
#else
#define GETUID() 1000
#endif
#endif

static inline unsigned long forward(unsigned long a, unsigned long b, unsigned long c, unsigned long d)
{
    unsigned long x = (a ^ b) & 0xfffffffful;
    return (((x << c) | (x >> ((32ul - c) & 31ul))) * d) & 0xfffffffful;
}

static inline unsigned long backward(unsigned long a, unsigned long b, unsigned long c, unsigned long d)
{
    unsigned long x = (a * b) & 0xfffffffful;
    return (((x << c) | (x >> ((32ul - c) & 31ul))) ^ d) & 0xfffffffful;
}

void LevelCollection::indexTextCollection()
{
    enum states { BEFORE_NONE, BEFORE_VALID, BEFORE_INVALID, DURING_NONE, DURING_VALID, DURING_INVALID } state = BEFORE_NONE;

    int levelstart = 0, levelend = 0;
    for (int pos = 0; pos < (data_.size() - 1); pos++) {
        switch (state) {
        case BEFORE_NONE:
            switch (data_[pos]) {
            case '#':
            case '.':
            case '$':
            case '+':
            case '*':
            case '@':
                state = BEFORE_VALID;
                break;

            case ' ':
            case '\t':
            case '\r':
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
            case '#':
            case '.':
            case '$':
            case '+':
            case '*':
            case '@':
            case ' ':
            case '\t':
            case '\r':
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
            case '#':
            case '.':
            case '$':
            case '+':
            case '*':
            case '@':
                state = DURING_VALID;
                break;

            case ' ':
            case '\t':
            case '\r':
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
            case '#':
            case '.':
            case '$':
            case '+':
            case '*':
            case '@':
            case ' ':
            case '\t':
            case '\r':
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

    if (state == DURING_NONE || state == DURING_INVALID) {
        data_[levelend] = '\0';
    }
}

void LevelCollection::loadPrefs()
{
    if (id_ >= 0) {
        KSharedConfigPtr cfg = KSharedConfig::openConfig();
        KConfigGroup settingsGroup(cfg, QStringLiteral("settings"));

        QString key = QString::asprintf("level%d", id_);
        level_ = settingsGroup.readEntry(key, QStringLiteral("0")).toInt();

        key = QString::asprintf("status%d", id_);
        unsigned long x = settingsGroup.readEntry(key, QStringLiteral("0")).toULong();

        x = backward(x, 0xc1136a15ul, 0x12ul, 0x80ff0b94ul);
        x = backward(x, 0xd38fd2ddul, 0x01ul, 0xd4d657b4ul);
        x = backward(x, 0x59004eeful, 0x1eul, 0xf6c75e2cul);
        x = backward(x, 0x366c3e25ul, 0x0aul, 0x61ebc208ul);
        x = backward(x, 0x20a784c9ul, 0x15ul, 0x207d488bul);
        x = backward(x, 0xc02864abul, 0x09ul, 0x709e62a3ul);
        x = backward(x, 0xe2a60f19ul, 0x0eul, 0x8bb02c07ul);
        x = backward(x, 0x3b0e11f3ul, 0x13ul, 0x608aef3ful);

        completedLevels_ = x >> 16 & 0x3ff;
        if (!settingsGroup.hasKey(key))
            completedLevels_ = 0;
        if (((x >> 26) & 0x3ful) != (unsigned long)id_)
            completedLevels_ = 0;
        if ((x & 0xfffful) != (unsigned long)GETUID())
            completedLevels_ = 0;
        if (completedLevels_ > noOfLevels_)
            completedLevels_ = 0;

        if (level_ > completedLevels_)
            level_ = completedLevels_;
        if (level_ >= noOfLevels_)
            level_ = noOfLevels_ - 1;
        if (level_ < 0)
            level_ = 0;
    } else {
        level_ = 0;
        completedLevels_ = noOfLevels_;
    }
}

void LevelCollection::addLevel(const char *_level)
{
    index_.append(_level);
}

void LevelCollection::addData(const char *_data, unsigned _len)
{
    unsigned pos = data_.size();
    data_.resize(pos + _len);
    memcpy(data_.data() + pos, _data, _len);
}

void LevelCollection::addSeparator()
{
    unsigned pos = data_.size();
    data_.resize(pos + 1);
    data_[pos] = '\0';
}

LevelCollection::LevelCollection(const char *_def, int _len, const QString &_name, int _id)
    : name_(_name)
    , id_(_id)
{
    addData(_def, _len);
    addSeparator();

    indexTextCollection();

    noOfLevels_ = index_.size();

    loadPrefs();
}

LevelCollection::LevelCollection(const QString &_path, const QString &_name, int _id)
    : name_(_name)
    , path_(_path)
    , id_(_id)
{
    char buf[1024];
    int len;

    QFile file(path_);
    if (file.open(QIODevice::ReadOnly)) {
        while ((len = file.read(buf, 1024)) > 0) {
            addData((const char *)buf, len);
        }
        file.close();
        addSeparator();
    }

    indexTextCollection();

    noOfLevels_ = index_.size();

    loadPrefs();
}

LevelCollection::~LevelCollection()
{
    if (id_ >= 0) {
        KSharedConfigPtr cfg = KSharedConfig::openConfig();
        KConfigGroup settingsGroup(cfg, QStringLiteral("settings"));

        const QString key = QString::asprintf("level%d", id_);
        settingsGroup.writeEntry(key, QStringLiteral("%1").arg(level_));
    }
}

void LevelCollection::levelCompleted()
{
    if (completedLevels_ < (level_ + 1))
        completedLevels_ = level_ + 1;

    if (id_ >= 0) {
        unsigned long x = (((unsigned long)GETUID()) & 0xfffful);
        x |= ((unsigned long)id_) << 26;
        x |= ((unsigned long)completedLevels_) << 16;

        x = forward(x, 0x608aef3ful, 0x0dul, 0xfb00ef3bul);
        x = forward(x, 0x8bb02c07ul, 0x12ul, 0x2a37dd29ul);
        x = forward(x, 0x709e62a3ul, 0x17ul, 0x23607603ul);
        x = forward(x, 0x207d488bul, 0x0bul, 0xc31fd579ul);
        x = forward(x, 0x61ebc208ul, 0x16ul, 0xbcffadadul);
        x = forward(x, 0xf6c75e2cul, 0x02ul, 0xa2baa00ful);
        x = forward(x, 0xd4d657b4ul, 0x1ful, 0x7e129575ul);
        x = forward(x, 0x80ff0b94ul, 0x0eul, 0x92fc153dul);

        const QString key = QString::asprintf("status%d", id_);

        KSharedConfigPtr cfg = KSharedConfig::openConfig();
        KConfigGroup settingsGroup(cfg, QStringLiteral("settings"));
        settingsGroup.writeEntry(key, QStringLiteral("%1").arg(x));
        cfg->sync();
    }
}

void LevelCollection::level(int _level)
{
    assert(_level >= 0 && _level < noOfLevels_);

    level_ = _level;
    if (level_ > completedLevels_)
        level_ = completedLevels_;
    if (level_ >= noOfLevels_)
        level_ = noOfLevels_ - 1;
    if (level_ < 0)
        level_ = 0;
}

static int minX(const char *def)
{
    int min_x = 10000;

    int x = 0;
    for (int pos = 0; def[pos]; pos++) {
        switch (def[pos]) {
        case '\n':
            x = 0;
            break;

        case ' ':
            x++;
            break;

        case '\t':
            x = (x + 8) & ~7;
            break;

        case '\r':
            break;

        default:
            if (x < min_x)
                min_x = x;
            break;
        }
    }

    return min_x == 10000 ? -1 : min_x;
}

bool LevelCollection::loadLevel(Map *_map)
{
    _map->clearMap();

    const char *def = index_[level_];
    bool goodMap = true;
    int x = 0, y = 0, goalsLeft = 0;

    int min_x = minX(def);
    if (min_x < 0) {
        min_x = 0;
        goodMap = false;
    }

    _map->resetPos();

    for (int pos = 0; def[pos]; pos++) {
        switch (def[pos]) {
        case '\n':
            y++;
            x = 0;
            break;

        case ' ':
            x++;
            break;

        case '\t':
            x = (x + 8) & ~7;
            break;

        case '@':
            if (x - min_x > Map::MAX_X || y > Map::MAX_Y)
                goodMap = false;
            else {
                _map->setPos(x - min_x, y);
            }
            x++;
            break;

        case '$':
            if (x - min_x > Map::MAX_X || y > Map::MAX_Y)
                goodMap = false;
            else
                _map->map(x - min_x, y, Map::OBJECT);
            x++;
            break;

        case '.':
            if (x - min_x > Map::MAX_X || y > Map::MAX_Y)
                goodMap = false;
            else {
                _map->map(x - min_x, y, Map::GOAL);
                goalsLeft++;
            }
            x++;
            break;

        case '#':
            if (x - min_x > Map::MAX_X || y > Map::MAX_Y)
                goodMap = false;
            else
                _map->map(x - min_x, y, Map::WALL);
            x++;
            break;

        case '+':
            if (x - min_x > Map::MAX_X || y > Map::MAX_Y)
                goodMap = false;
            else {
                _map->setPos(x - min_x, y);
                _map->map(x - min_x, y, Map::GOAL);
                goalsLeft++;
            }
            x++;
            break;

        case '*':
            if (x - min_x > Map::MAX_X || y > Map::MAX_Y)
                goodMap = false;
            else
                _map->map(x - min_x, y, Map::OBJECT | Map::GOAL);
            x++;
            break;

        case '\r':
            break;

        default:
            goodMap = false;
            break;
        }
    }

    if (_map->objectsLeft() != goalsLeft)
        goodMap = false;
    if (_map->completed())
        goodMap = false;

    if (_map->badCoords(_map->xpos(), _map->ypos()))
        goodMap = false;
    else {
        if (!_map->empty(_map->xpos(), _map->ypos()))
            goodMap = false;
        else if (!_map->fillFloor(_map->xpos(), _map->ypos()))
            goodMap = false;
    }

    return goodMap;
}
