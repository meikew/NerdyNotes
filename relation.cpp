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

#include "nameitem.h"
#include "relation.h"
#include <fstream>

relation::relation(std::string n, std::string idstring)
  : nameitem(n,idstring)
{
}

relation::relation(std::string n, relation * inv_id, std::string idstring)
  : nameitem(n,idstring), inverseid(inv_id)
{
}

void relation::set_inverse(relation * inv_id){
  inverseid=inv_id;
}

void relation::filewrite(std::fstream& outfile){
  item::filewrite(outfile);
  outfile << inverseid->get_id() << '\t' << nameitem::name << std::endl;
}

void relation::print_gui(int level,wxScrolled<wxPanel>* firstpanel,wxFlexGridSizer *fgs,Knomegui * kg){ 
  int boxwidth = 200;
  if (level > 1){
    boxwidth =  120;
  }
  wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,wxString::FromUTF8(name.c_str()),wxPoint(-1, -1),  wxSize(boxwidth, 30),wxTE_READONLY); //read-only relation
  fgs->Add(tc1,0,wxALIGN_CENTER);
}