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

#include <iostream>
#include "nameitem.h"
#include "item.h"
#include "model.h"
#include "knomegui.h"
#include "statement.h"


nameitem::nameitem(std::string n, std::string idstring)
  : item(idstring), name(n)
{
}


void nameitem::destructor(model& M){
  M.remove_from_conceptdictionary(name);
  item::destructor(M);
}

void nameitem::print(int level){
  std::cout << name;
}

std::vector<item *> nameitem::print_self(){
  print(0);
  std::vector<item *> emptyvec;
  return emptyvec;
}

void nameitem::print_gui(int level,wxScrolled<wxPanel>* firstpanel,wxFlexGridSizer *fgs,Knomegui * kg){
  if (level==0){//if the item is the subject in browsing mode
    wxTextCtrl *tc1;
    if (id != "bc0"){
      tc1 = new wxTextCtrl(firstpanel, -1,wxString::FromUTF8(name.c_str()),wxPoint(-1, -1), wxSize(240, 80), wxTE_MULTILINE | wxTE_PROCESS_ENTER | wxTE_AUTO_URL);//editable text box for the focal concept, the conversion to C string and then wxString is necessary to correctly process international characters
      kg->Connect(wxEVT_TEXT_ENTER, wxCommandEventHandler(Knomegui::OnConceptEdit));
    }
    else {
      tc1 = new wxTextCtrl(firstpanel, -1,wxString::FromUTF8(name.c_str()),wxPoint(-1, -1), wxSize(240, 80), wxTE_MULTILINE | wxTE_READONLY  | wxTE_AUTO_URL);//the basic concept "NerdyNotes" cannot be edited
    }
    tc1->SetBackgroundColour(wxColour("#fbfac3"));//make the text box yellow
    fgs->Add(tc1,0,wxALIGN_LEFT);
  }
  else if ((level==-2)||(level==-3)){//subject in a quiz answer, hint or question
    wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,wxString::FromUTF8(name.c_str()),wxPoint(-1, -1), wxSize(240, 80), wxTE_MULTILINE | wxTE_READONLY | wxTE_AUTO_URL);//editable text box for the focal concept, the conversion to C string and then wxString is necessary to correctly process international characters
    tc1->SetBackgroundColour(wxColour("#fbfac3"));//make the text box yellow
    fgs->Add(tc1,0,wxALIGN_LEFT);
  }
  else if ((level==1) || (level==-1) || (level==-4)) {//if the item is the object in browsing mode or in a quiz
    wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,wxString::FromUTF8(name.c_str()),wxPoint(-1, -1), wxSize(400, 60), wxTE_MULTILINE | wxTE_READONLY | wxTE_AUTO_URL);//read-only text box when it is not the focal concept
    tc1->SetBackgroundColour(wxColour("#e7fbc3"));//make the text box light green
    fgs->Add(tc1,0,wxALIGN_RIGHT);
  }
  else {//if the item is part of a statement that is as a whole a subject or object
    wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,wxString::FromUTF8(name.c_str()),wxPoint(-1, -1), wxSize(122, 100), wxTE_MULTILINE | wxTE_READONLY | wxTE_AUTO_URL);//read-only text box when it is not the focal concept
    tc1->SetBackgroundColour(wxColour("#e7fbc3"));//make the text box light green
    fgs->Add(tc1,0,wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
  }
}

void nameitem::edit(model & M){
  std::string oldname = name;
  std::cout << "new name: ";
  getline(std::cin,name);
  M.update_conceptdictionary(oldname,name);

}

void nameitem::setname(std::string newname, model * m){
  bool namealreadyinuse = m->find_in_concept_dictionary(newname);
  if (namealreadyinuse){
    wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Name already in use. Please try again."), wxT("Error"), wxOK | wxICON_ERROR);
    dial->ShowModal();
  }
  else {
    std::string oldname = name;
    name = newname;
    m->update_conceptdictionary(oldname,name);
  }
}

void nameitem::filewrite(std::fstream& outfile){
  item::filewrite(outfile);
  outfile << name << std::endl;
}
