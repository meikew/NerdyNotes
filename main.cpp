/*  This code is part of NerdyNotes, a software for personal knowledge 
    management and spaced repetition.
    Copyright (C) 2024  Meike Wittmann (meike.wittmann@uni-bielefeld.de)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include "main.h"
#include "knomegui.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include "nameitem.h"
#include "relation.h"
#include "statement.h"
#include "model.h"
#include "mytime.h"
#include <fstream>
#include <boost/random.hpp>

boost::mt19937 rng; //global random number generator, declared extern in all other .cpp files that need a random number generator

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{

  rng.seed(std::time(0));//initialize random number generator

  Knomegui *kg = new Knomegui(wxT("NerdyNotes"));
  kg->Show(true);

  return true;
}

