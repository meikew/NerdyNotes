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
    
#ifndef STATEMENT_HH
#define STATEMENT_HH
#include "item.h"
#include "relation.h"
#include "quizitem.h"
#include <wx/wx.h>

class model;//forward declaration, necessary to allow for circular dependency between model and statement class
class Knomegui;

class statement : public quizitem {
public:
  statement(std::string idstring, item * sub_id, relation * rel_id, item * obj_id, int qtype, bool fm, bool bm);
  statement(std::string idstring, item * sub_id, relation * rel_id, item * obj_id, int qtype, bool fm, bool bm, std::vector<double> Ef, std::vector<mytime> nrd, std::vector<double> is);
  ~statement();
  void print(int level);
  std::vector<item *> print_self();
  void print_gui(int level,wxScrolled<wxPanel>* firstpanel,wxFlexGridSizer *fgs,Knomegui * kg);
  void edit(model & M);
  void editgui(int choice,model & M, Knomegui * kg);//choice options: 1: change subject, 2: change relation, 3: change object
  void filewrite(std::fstream& outfile);
  void quiz_print(int direction,Knomegui * kg);
  void hint_print(int direction,Knomegui * kg);
  void answer_print(int direction,Knomegui * kg);
  void answer_item_print(int direction, Knomegui * kg);
  void add_grading_buttons(Knomegui * kg,wxScrolled<wxPanel>* firstpanel,wxBoxSizer * pagevbox);
  std::vector<std::string> get_text();
  void setname(std::string newname,model * m);
  void printquizsettings(Knomegui * kg, wxBoxSizer * pagevbox, wxScrolled<wxPanel>* firstpanel);
  void updateforward(bool check);
  void updatebackward(bool check);
  void updatevocabforward(bool check);
  void updatevocabbackward(bool check);
protected:
  item * subjectid;
  relation * relationid;
  item * objectid;
  bool forwardmode; //0: state missing item, 1: state all matching items
  bool backwardmode;//same as for forward mode
};

#endif
