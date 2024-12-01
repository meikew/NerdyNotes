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
    
#ifndef RELATION_HH
#define RELATION_HH
#include "nameitem.h"

class relation : public nameitem {
public:
  relation(std::string n, std::string idstring);
  relation(std::string n, relation * inv_id, std::string idstring);
  void set_inverse(relation * inv_id);
  relation * get_inverse() const {return inverseid;};
  void filewrite(std::fstream& outfile);
  void print_gui(int level,wxScrolled<wxPanel>* firstpanel,wxFlexGridSizer *fgs,Knomegui * kg);

protected:
  relation * inverseid;
 
};

#endif
