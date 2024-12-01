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

#ifndef NAMEITEM_HH
#define NAMEITEM_HH
#include "item.h"
#include <wx/wx.h>
#include <string>

class model;//forward decleration, necessary to allow for circular dependency between model and statement class
class Knomegui;

class nameitem : public item {
public:
  nameitem(std::string n, std::string id);
  void destructor(model& M);
  std::string get_name() const {return name;};
  void print(int level);
  std::vector<item *> print_self();
  virtual void print_gui(int level,wxScrolled<wxPanel>* firstpanel,wxFlexGridSizer *fgs,Knomegui * kg);
  void edit(model & M);
  void setname(std::string newname,model * m);
  void filewrite(std::fstream& outfile);

protected:
  std::string name;
};

std::ostream & operator <<(std::ostream & out,const nameitem & a);

#endif
