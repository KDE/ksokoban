/*
    SPDX-FileCopyrightText: 1998 Anders Widell <d95-awi@nada.kth.se>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef STATICIMAGE_H
#define STATICIMAGE_H

#include "ImageData.h"

class
StaticImage : public ImageData {
public:
  StaticImage ();
  ~StaticImage () override;

};

#endif  /* STATICIMAGE_H */
