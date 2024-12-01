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

#include "item.h"
#include <string>
#include <iostream>
#include <fstream>
#include "model.h"
#include "statement.h"
#include "knomegui.h"

item::item(std::string idstring)
  : id(idstring)
{
}

void item::destructor(model& M){
  M.remove_from_itemmap(id);
  for (std::map<item *,std::vector<item *> >::iterator it=statements.begin(); it != statements.end(); ++it){
    while  ((it->second).size()){//can never be circular (I hope) because only statements are deleted, not the items themselves
      (it->second)[0]->destructor(M);//vector entries are deleted by the remove_statement function
    }
  }
  delete this;
}

void item::displaygui(Knomegui * kg, bool firstpage){//kg is the model object; if it is the first page shown firtpage = true, otherwise it is false
  //determine number of rows needed
  int rownum = 1;//one row for the focal concept itself
  for (std::map<item *,std::vector<item *> >::iterator it=statements.begin(); it != statements.end(); ++it){//loop over all relations
    rownum += 1+ (it->second).size(); //one row for each relation and one for each concept linked via the respective relation
  }
  wxScrolled<wxPanel>* firstpanel = new wxScrolled<wxPanel>(kg->GetBook(),wxID_ANY);
  firstpanel->SetScrollRate(10,10);//no horizontal, just vertical scrolling

  wxBoxSizer * pagevbox = new wxBoxSizer(wxVERTICAL);
  pointervec = print_self();
  bool is_statement = pointervec.size()>0;
  wxFlexGridSizer *fgs = new wxFlexGridSizer(rownum, 3 + pointervec.size(), 9, 25); //wxFlexGridSizer(int rows, int cols, int vgap, int hgap); for a statement the pointervec has two elements and two extra columns are needed for the 
  print_gui(0,firstpanel,fgs,kg);
  if (is_statement){//add buttons for going to the subject and object
    wxButton *subj = new wxButton(firstpanel,101,wxT("Subject"));
    wxButton *obj = new wxButton(firstpanel,102,wxT("Object"));
    fgs->Add(subj,0,wxALIGN_RIGHT | wxRIGHT | wxALIGN_CENTER_VERTICAL);
    fgs->Add(obj,0,wxALIGN_RIGHT | wxRIGHT | wxALIGN_CENTER_VERTICAL);
    kg->Connect(3, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnNavigate));
    kg->Connect(4, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnNavigate));
  }
  fgs->Add(new wxStaticText(firstpanel, -1, wxT("")));//add empty text to fill the slot
  fgs->Add(new wxStaticText(firstpanel, -1, wxT("")));//add empty text to fill the slot

  for (std::map<item *,std::vector<item *> >::iterator it=statements.begin(); it != statements.end(); ++it){
    item * relationpointer = it->first;
    relationpointer->print_gui(1,firstpanel,fgs,kg);
    //wxButton *addrelstatement = new wxButton(firstpanel,-1,wxT("Add"));//add a statement for this relation
    //fgs->Add(addrelstatement);
    fgs->Add(new wxStaticText(firstpanel, -1, wxT("")));//add empty text to fill the slot
    fgs->Add(new wxStaticText(firstpanel, -1, wxT("")));//add empty text to fill the slot
    if (is_statement){
      fgs->Add(new wxStaticText(firstpanel, -1, wxT("")));//add empty text to fill the slot
      fgs->Add(new wxStaticText(firstpanel, -1, wxT("")));//add empty text to fill the slot
    }

    for (unsigned i=0; i<(it->second).size(); i++){
      
      (objects[relationpointer][i])->print_gui(1,firstpanel,fgs,kg);
      pointervec.push_back((it->second)[i]);
      int newbuttonID = pointervec.size()+100;//button for going to the statement, +100 because some IDs are needed for the buttons in the toolbar
      wxButton *gotorel = new wxButton(firstpanel, newbuttonID, wxT("S"));
      kg->Connect(newbuttonID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnNavigate));
      pointervec.push_back(objects[relationpointer][i]);
      newbuttonID +=1 ;
      wxButton *gotocon = new wxButton(firstpanel, newbuttonID, wxT("C"));//button for going to the linked concept
      kg->Connect(newbuttonID, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnNavigate));
      fgs->Add(gotorel, 0, wxALIGN_CENTER);
      fgs->Add(gotocon, 0, wxALIGN_CENTER);
        if (is_statement){
        fgs->Add(new wxStaticText(firstpanel, -1, wxT("")));//add empty text to fill the slot
        fgs->Add(new wxStaticText(firstpanel, -1, wxT("")));//add empty text to fill the slot
      }
    }
  }
  
  fgs->AddGrowableCol(0, 1);//make the first column expand if the window is expanded
  pagevbox->Add(fgs, 1, wxALL | wxEXPAND, 15);
  if (is_statement){ //insert box with quiz settings
    statement* sp = static_cast<statement*>(this);
    sp->printquizsettings(kg,pagevbox,firstpanel);
  }

  firstpanel->SetSizer(pagevbox);
  if (firstpage){
    kg->DisplayFirstPage(firstpanel);
  }
  else {
    kg->DisplayNextPage(firstpanel);
  }
}


item * item::display(){
  system("clear");
  std::cout << "#######################" << std::endl;
  std::vector<item *> pointervec = print_self(); //item prints itself
  unsigned choicenum = pointervec.size();
  if (choicenum > 0) std::cout << " [subject: 1, object: 2]";
  std::cout << std::endl << "--------------------" << std::endl;
  for (std::map<item *,std::vector<item *> >::iterator it=statements.begin(); it != statements.end(); ++it){
    item * relationpointer = it->first;
    std::cout << "[";
    relationpointer->print(1);
    std::cout << "]" << std::endl;
    for (unsigned i=0; i<(it->second).size(); i++){
      std::cout << "\t[";
      (objects[relationpointer][i])->print(1);
      std::cout << "] [statement: " << choicenum+1 << ", object: " << choicenum+2 << "]" << std::endl;
      choicenum +=2;
      pointervec.push_back((it->second)[i]);
      pointervec.push_back(objects[relationpointer][i]);
    }
  }
  std::cout << "Do something else: " << choicenum + 1 << std::endl;
  unsigned choice;
  std::cin >> choice;
  std::cin.clear();
  std::cin.ignore();
  item * nextitem = 0;
  if ((choice > 0) && (choice <= pointervec.size())) nextitem = pointervec[choice-1];
  if (choice > choicenum) nextitem = this;
  std::cout << std::endl;
  return nextitem;
}

void item::display_hint(item * connectorpointer, std::vector<item *> answervec,Knomegui *  kg, wxScrolled<wxPanel>* firstpanel, wxBoxSizer * pagevbox){//print all item information except the one specified by the input information, which can also be empty
  int rownum = 1;//one row for the focal concept itself
  for (std::map<item *,std::vector<item *> >::iterator it=statements.begin(); it != statements.end(); ++it){//loop over all relations
    rownum += 1+ (it->second).size(); //one row for each relation and one for each concept linked via the respective relation
  }
  wxFlexGridSizer *fgs = new wxFlexGridSizer(rownum, 1, 9, 25); //wxFlexGridSizer(int rows, int cols, int vgap, int hgap); for a statement the pointervec has two elements and two extra columns are needed for the 
  
  print_gui(-2,firstpanel,fgs,kg);
  for (std::map<item *,std::vector<item *> >::iterator it=statements.begin(); it != statements.end(); ++it){
    item * relationpointer = it->first;
    relationpointer->print_gui(1,firstpanel,fgs,kg);
    for (unsigned i=0; i<(it->second).size(); i++){
      if ((relationpointer == connectorpointer)&&((answervec.size()>1)||(objects[relationpointer][i] == answervec[0]))){//If this is one of the requested answers, add a question mark instead of the concept
        wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,wxT("?"),wxPoint(-1, -1),  wxSize(400, 60),wxTE_READONLY); 
        tc1->SetBackgroundColour(wxColour("#FFC300"));//make the text box orange
        fgs->Add(tc1,0,wxALIGN_RIGHT);
      }
      else {
	      (objects[relationpointer][i])->print_gui(1,firstpanel,fgs,kg);
      }
    }
  }
  fgs->AddGrowableCol(0, 1);//make the first column expand if the window is expanded
  pagevbox->Add(fgs, 1, wxALL | wxEXPAND, 15);
}

void item::add_info(item * stat, item * rel, item * obj){
  statements[rel].push_back(stat);
  objects[rel].push_back(obj);
}

void item::remove_statement(item * relpointer, item * statpointer){
  bool found = false;
  unsigned index = 0;
  while (!found){
    found = (statements[relpointer][index] == statpointer);
    if (found){
      statements[relpointer].erase(statements[relpointer].begin()+index);
      objects[relpointer].erase(objects[relpointer].begin()+index);
    }
    index ++;
  }
}

void item::filewrite(std::fstream& outfile){
  std::string idnumber = id;
  idnumber.erase(idnumber.begin());//remove the s at the beginning of the id string
  outfile << idnumber << '\t';
}

item * item::getpointervectorentry(int target_item_counter){
  return(pointervec[target_item_counter]);
}
