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

#ifndef ITEM_HH
#define ITEM_HH
#include <vector>
#include <map>
#include <string>
#include <wx/wx.h>

class model;//forward decleration, necessary to allow for circular dependency between model and statement class
class Knomegui;

class item {
public:
  item(std::string idstring);
  virtual ~item(){};
  virtual void destructor(model& M);
  virtual void print(int level)=0; //what is shown in the display view of this or another item, level 0 if it is the focal item, level 1 if the item is part of a statement directly involving the focal item, 2 if it is part of a statement, which is part of a statement involving the focal item, and so on
  virtual std::vector<item *> print_self()=0;//when the item is the focal item
  virtual void print_gui(int level,wxScrolled<wxPanel>* firstpanel,wxFlexGridSizer *fgs,Knomegui * kg)=0; //what is shown in the display view of this or another item, level 0 if it is the focal item, level 1 if the item is part of a statement directly involving the focal item, 2 if it is part of a statement, which is part of a statement involving the focal item, and so on, -1 if answer in a quiz, -3 if subject in the original question in a quiz, and -2 for the subject in a quiz hint or answer
  item * display();//what is shown when this item is the focal item
  void displaygui(Knomegui * kg, bool firstpage);//what is shown when this item is the focal item
  void display_hint(item * connectorpointer, std::vector<item *> answervec, Knomegui * kg, wxScrolled<wxPanel>* firstpanel, wxBoxSizer * pagevbox);
  void add_info(item * stat, item * rel, item * obj);
  std::string get_id() const {return id;};
  virtual void edit(model & M)=0;
  virtual void setname(std::string newname,model * m)=0;
  void remove_statement(item * relpointer, item * statpointer);
  virtual void filewrite(std::fstream& outfile);
  std::vector<item *> get_objects(item * relpointer) {return objects[relpointer];};
  item * getpointervectorentry(int target_item_counter);

protected:
  std::string id;
  std::map<item *,std::vector<item *> > statements; //pointers to all statements in which the node is involved, organized by relation
  std::map<item *,std::vector<item *> > objects;//and the corresponding objects
  std::vector<item *> pointervec; //vector of pointers to the connected objects and statements; will be filled or updated during display; serves to connect button click ids to pointers
};

#endif
