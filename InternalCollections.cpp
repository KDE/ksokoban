#include <klocale.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef USE_LIBZ
#include <zlib.h>
#endif

#include "InternalCollections.h"

#ifndef LEVELS_INCLUDED
#define LEVELS_INCLUDED 1
#include "levels/data.c"
#endif

#define BUFSIZE (128*1024)

// static const int collection_save_id[] = {
//   0, 1, 3, 5, 9, 6, 7, 8, 2, 4
// };

static const int collection_save_id[] = {
  10, 11, 12, 13, 14
};

int
InternalCollections::configCollection2Real (int collection) {
  for (int i=0; i < (int) (sizeof (collection_save_id) / sizeof (int)); i++) {
    if (collection_save_id[i] == collection) return i;
  }
  return 0;
}

int
InternalCollections::realCollection2Config(int collection) {
  assert(collection < (int) (sizeof (collection_save_id) / sizeof (int)));
  return collection_save_id[collection];
}

QString
InternalCollections::collectionName(int _level) {
  switch (_level) {
  case 0:
    return i18n("Sasquatch");
    break;

  case 1:
    return i18n("Mas Sasquatch");
    break;

  case 2:
    return i18n("Sasquatch III");
    break;

  case 3:
    return i18n("Microban (easy)");
    break;

  case 4:
    return i18n("Sasquatch IV");
    break;
  }

  assert(false);
  return QString();
}


InternalCollections::InternalCollections() {
  int datasize, levelnum=0;

#ifdef USE_LIBZ
  data_ = (char *) malloc(BUFSIZE);
  if (data_ == NULL) abort();

  datasize = BUFSIZE;
  uncompress ((unsigned char *) data_, (long unsigned int *) &datasize, level_data_, sizeof (level_data_));
  data_ = (char *) realloc(data_, datasize);
  if (data_ == NULL) abort ();
#else
  datasize = sizeof (level_data_);
  data_ = (char *) malloc(datasize);
  if (data_ == NULL) abort();
  memcpy(data_, level_data_, datasize);
#endif

  int start=0, end=0, name=0;
  enum {NAME, DATA} state=NAME;
  while (end < datasize) {
    switch (state) {
    case NAME:
      if (data_[end] == '\n') {
	data_[end] = '\0';
	state = DATA;
      }
      end++;
      start = end;
      break;

    case DATA:
      if (isalpha(data_[end])) {
// 	collections_.add(new LevelCollection(data_+start, end-start, data_+name, collection_save_id[levelnum]));
	add(new LevelCollection(data_+start, end-start, collectionName(levelnum), collection_save_id[levelnum]));
	//printf("Level found: '%s'\n", data_+name);
	levelnum++;
	name = end;
	state = NAME;
      }
      end++;
      break;

    default:
      assert(0);
    }
  }
  if (state == DATA) {
//     collections_.add(new LevelCollection(data_+start, end-start, data_+name, collection_save_id[levelnum]));
    add(new LevelCollection(data_+start, end-start, collectionName(levelnum), collection_save_id[levelnum]));
    //printf("***Level found: '%s'\n", data_+name);
  }
  //printf("numlevels: %d/%d\n", levelnum+1, collections_.size());
}

InternalCollections::~InternalCollections() {
  for (unsigned i=0; i<collections_.size(); i++) {
    delete collections_[i];
  }

  free(data_);
}

int
InternalCollections::collections() {
  return collections_.size();
}

LevelCollection *
InternalCollections::operator[](int n) {
  return collections_[n];
}

void
InternalCollections::add(LevelCollection* c) {
  unsigned s = collections_.size();
  collections_.resize(s + 1);
  collections_.insert(s, c);
}
