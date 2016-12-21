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

#ifndef YAML_OUTPUT_H
#define YAML_OUTPUT_H

#include "algorithm.h"
#include "pool.h"

using namespace std;
using namespace essentia;

namespace essentiawrapper {

class YamlOutput : public standard::Algorithm
{

protected:
    standard::Input<Pool> _pool;
    string _filename;
    bool _doubleCheck;
    bool _outputJSON;
    int _indent;
    bool _writeVersion;

    void outputToStream(ostream *out);

public:

    YamlOutput();
    virtual ~YamlOutput() = default;

    virtual void declareParameters() override;
    virtual void configure() override;
    virtual void compute() override;

};

} // namespace essentiawrapper

#endif // YAML_OUTPUT_H
