/*
 * Copyright (C) 2006-2016  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 *
 * Modified by E. Mista, date 12 december 2016
 *
 */

#ifndef STREAMING_EXTRACTORLOWLEVEL_H
#define STREAMING_EXTRACTORLOWLEVEL_H

#include "streaming/sourcebase.h"
#include "pool.h"
#include "types.h"

using namespace std;
using namespace essentia;
using namespace essentia::streaming;

void LowLevelSpectral(SourceBase &input, Pool &pool, const Pool &options, const string &nspace = "");
void LowLevelSpectralEqLoud(SourceBase &input, Pool &pool, const Pool &options, const string &nspace = "");
void Level(SourceBase &input, Pool &pool, const Pool &options, const string &nspace = "");
void LevelAverage(Pool &pool, const string &nspace = "");

#endif // STREAMING_EXTRACTORLOWLEVEL_H
