#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef USE_LIBZ
#include <zlib.h>
#endif

#include "InternalCollections.H"

#include "levels/data.c"

#define BUFSIZE (128*1024)

static const int collection_save_id[] = {
  0, 1, 3, 5, 9, 6, 7, 8, 2, 4
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


InternalCollections::InternalCollections() {
  char *data;
  int datasize, levelnum=0;

#ifdef USE_LIBZ
  data = (char *) malloc(BUFSIZE);
  if (data == NULL) abort();

  datasize = BUFSIZE;
  uncompress ((unsigned char *) data, (long unsigned int *) &datasize, level_data_, sizeof (level_data_));
  data = (char *) realloc(data, datasize);
  if (data == NULL) abort ();
#else
  datasize = sizeof (level_data_);
  data = (char *) malloc(datasize);
  if (data == NULL) abort();
  memcpy(data, level_data_, datasize);
#endif

  int start=0, end=0, name=0;
  enum {NAME, DATA} state=NAME;
  while (end < datasize) {
    switch (state) {
    case NAME:
      if (data[end] == '\n') {
	data[end] = '\0';
	state = DATA;
      }
      end++;
      start = end;
      break;

    case DATA:
      if (isalpha(data[end])) {
	collections_.add(new LevelCollection(data+start, end-start, data+name, collection_save_id[levelnum]));
	//printf("Level found: '%s'\n", data+name);
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
    collections_.add(new LevelCollection(data+start, end-start, data+name, collection_save_id[levelnum]));
    //printf("***Level found: '%s'\n", data+name);
  }
  //printf("numlevels: %d/%d\n", levelnum+1, collections_.size());
}

InternalCollections::~InternalCollections() {
  for (int i=0; i<collections_.size(); i++) {
    delete collections_[i];
  }
}

int
InternalCollections::collections() {
  return collections_.size();
}

LevelCollection *
InternalCollections::operator[](int n) {
  return collections_[n];
}


