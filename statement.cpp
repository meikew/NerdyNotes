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
#include "model.h"
#include "knomegui.h"
#include "statement.h"
#include "item.h"
#include <wx/wx.h>
#include <wx/checkbox.h>


statement::statement(std::string idstring, item * sub_id, relation * rel_id, item * obj_id, int qtype, bool fm, bool bm)
  : quizitem(idstring,qtype), subjectid(sub_id), relationid(rel_id), objectid(obj_id), forwardmode(fm), backwardmode(bm)
{
}

statement::statement(std::string idstring, item * sub_id, relation * rel_id, item * obj_id, int qtype, bool fm, bool bm, std::vector<double> Ef, std::vector<mytime> nrd, std::vector<double> is)
  : quizitem(idstring,qtype,Ef,nrd,is), subjectid(sub_id), relationid(rel_id), objectid(obj_id), forwardmode(fm), backwardmode(bm)
{
}


statement::~statement(){
  subjectid->remove_statement(relationid,this);
  objectid->remove_statement(relationid->get_inverse(),this);
  //item destructor gets automatically called at the end
}

void statement::print(int level){
  std::cout << "[";
  subjectid->print(level+1);
  std::cout << "] [";
  relationid->print(level+1);
  std::cout << "] [";
  objectid->print(level+1);
  std::cout << "]";
  if (level == 0){
    std::cout << std::endl;
  }
}

std::vector<item *> statement::print_self(){
  //print(0);
  std::vector<item *> returnvec;
  returnvec.push_back(subjectid);
  returnvec.push_back(objectid);
  return returnvec;
}

void statement::print_gui(int level,wxScrolled<wxPanel>* firstpanel,wxFlexGridSizer *fgs,Knomegui * kg){
  int numrows = 1;
  if (level==0) numrows = 2;
  wxFlexGridSizer * statementhbox = new wxFlexGridSizer(numrows,5,9,9);
  statementhbox->Add(new wxStaticText(firstpanel, -1, wxT("[")),0,wxALIGN_CENTER_VERTICAL);//provide a "box" for the statement
  subjectid->print_gui(2,firstpanel,statementhbox,kg);
  relationid->print_gui(2,firstpanel,statementhbox,kg);
  objectid->print_gui(2,firstpanel,statementhbox,kg);
  statementhbox->Add(new wxStaticText(firstpanel, -1, wxT("]")),0,wxALIGN_CENTER_VERTICAL);
  if (level == 0){//if statement is subject during browsing
    wxImage::AddHandler(new wxPNGHandler);
    statementhbox->Add(new wxStaticText(firstpanel, -1, wxT("")));//add empty text to fill the slot
    wxBitmapButton *editsubject = new wxBitmapButton(firstpanel, 97, wxBitmap(wxT("edit2.png"), wxBITMAP_TYPE_PNG), wxPoint(40, 10));
    wxBitmapButton *editrelation = new wxBitmapButton(firstpanel, 98, wxBitmap(wxT("edit2.png"), wxBITMAP_TYPE_PNG), wxPoint(40, 10));
    wxBitmapButton *editobject = new wxBitmapButton(firstpanel, 99, wxBitmap(wxT("edit2.png"), wxBITMAP_TYPE_PNG), wxPoint(40, 10));
    statementhbox->Add(editsubject, 0, wxALIGN_CENTER);
    statementhbox->Add(editrelation, 0, wxALIGN_CENTER);
    statementhbox->Add(editobject, 0, wxALIGN_CENTER);
    kg->Connect(97, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnEditStatement));
    kg->Connect(98, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnEditStatement));
    kg->Connect(99, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnEditStatement));
    statementhbox->Add(new wxStaticText(firstpanel, -1, wxT("")));//add empty text to fill the slot
    fgs->Add(statementhbox,0,wxALL | wxEXPAND,10);
  }
  else if ((level==-2)||(level==-3)){//if statement is subject during quiz
    fgs->Add(statementhbox,0,wxALL | wxEXPAND,10);
  }
  else {
    fgs->Add(statementhbox,0,wxALIGN_RIGHT);
  }
}

void statement::edit(model & M){

}

void statement::editgui(int choice,model & M, Knomegui * kg){
  subjectid->remove_statement(relationid,this);
  objectid->remove_statement(relationid->get_inverse(),this);
  if (choice == 1) {
    item * newsubjectid = M.assign_object(kg);
    if (newsubjectid != nullptr){
      subjectid = newsubjectid;
    }
  }
  if (choice == 2){
    relation * newrelationid = M.assign_relation(kg);
    if (newrelationid != nullptr){
      relationid = newrelationid;
    }
  } 
  if (choice == 3){
    item * newobjectid = M.assign_object(kg);
    if (newobjectid != nullptr){
      objectid = newobjectid;
    }
  } 
  subjectid->add_info(this,relationid,objectid);
  objectid->add_info(this,relationid->get_inverse(),subjectid);
  M.save_to_file();
}

void statement::filewrite(std::fstream& outfile){
  item::filewrite(outfile);
  outfile << subjectid->get_id() << '\t' << relationid->get_id() << '\t' << objectid->get_id() << '\t' << forwardmode << '\t' << backwardmode << '\t';
  quizitem::filewrite(outfile);
  outfile << std::endl;
}

void statement::quiz_print(int direction, Knomegui * kg){
  item * questionpointer;
  item * answerpointer;
  item * connectorpointer;
  if (direction == 0){
    questionpointer = subjectid;
    answerpointer = objectid;
    connectorpointer = relationid;
  }
  else {
    questionpointer = objectid;
    answerpointer = subjectid;
    connectorpointer = relationid->get_inverse();
  }
  std::vector<item *> allanswers = questionpointer->get_objects(connectorpointer);

  wxScrolled<wxPanel>* firstpanel = new wxScrolled<wxPanel>(kg->GetBook(),wxID_ANY);
  firstpanel->SetScrollRate(10,10);//no horizontal, just vertical scrolling
  wxBoxSizer * pagevbox = new wxBoxSizer(wxVERTICAL);
  
  if ((direction == 0 && forwardmode == 1) || (direction !=0 && backwardmode == 1)){//user needs to state all matching items (vocabulary mode)
    wxFlexGridSizer *fgs = new wxFlexGridSizer(3, 1, 9, 25);
    questionpointer->print_gui(-3,firstpanel,fgs,kg);
    connectorpointer->print_gui(1,firstpanel,fgs,kg);
    std::string prompt = "?";
    if (allanswers.size()>1){
      prompt = prompt + " [" + std::to_string(allanswers.size()) + "]";
    }
    wxString questionmarktext(prompt);
    wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,questionmarktext,wxPoint(-1, -1),  wxSize(400, 60),wxTE_READONLY); 
    tc1->SetBackgroundColour(wxColour("#FFC300"));//make the text box orange
    fgs->Add(tc1,0,wxALIGN_RIGHT);
    fgs->AddGrowableCol(0, 1);//make the first column expand if the window is expanded
    pagevbox->Add(fgs, 1, wxALL | wxEXPAND, 15);
  }
  else {
    wxFlexGridSizer *fgs = new wxFlexGridSizer(2+allanswers.size(), 1, 9, 25);
    questionpointer->print_gui(-3,firstpanel,fgs,kg);
    connectorpointer->print_gui(1,firstpanel,fgs,kg);
    for (unsigned a=0; a<allanswers.size(); a++){
      if (allanswers[a]!=answerpointer){
	      allanswers[a]->print_gui(1,firstpanel,fgs,kg);
      }
    }
    wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,wxT("?"),wxPoint(-1, -1),  wxSize(400, 60),wxTE_READONLY); 
    tc1->SetBackgroundColour(wxColour("#FFC300"));//make the text box orange
    fgs->Add(tc1,0,wxALIGN_RIGHT);
    fgs->AddGrowableCol(0, 1);//make the first column expand if the window is expanded
    pagevbox->Add(fgs, 1, wxALL | wxEXPAND, 15);
  }
  wxBoxSizer * buttonhbox = new wxBoxSizer(wxHORIZONTAL);
  wxButton * showanswer = new wxButton(firstpanel, 50, wxT("Show answer (SPACE)"));//button for going to the linked concept
  wxButton * showhint = new wxButton(firstpanel, 51, wxT("Show hint (H)"));//button for going to the linked concept
  kg->Connect(50, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnShowAnswer));
  kg->Connect(51, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnDisplayHint));

  //set corresponding keyboard shortcuts
  wxAcceleratorEntry entries[2];
  entries[0].Set(wxACCEL_NORMAL, WXK_SPACE, 50);
  entries[1].Set(wxACCEL_NORMAL, (int) 'H',51);
  wxAcceleratorTable accel(2, entries);
  kg->SetAcceleratorTable(accel);

  buttonhbox->Add(showanswer, 0, wxALL | wxALIGN_TOP, 15);
  buttonhbox->Add(showhint, 0, wxALL | wxALIGN_TOP, 15);
  pagevbox->Add(buttonhbox, wxALL | wxALIGN_TOP, 15);
  firstpanel->SetSizer(pagevbox);
  kg->DisplayNextPage(firstpanel);
}

void statement::hint_print(int direction, Knomegui * kg){
  wxScrolled<wxPanel>* firstpanel = new wxScrolled<wxPanel>(kg->GetBook(),wxID_ANY);
  firstpanel->SetScrollRate(10,10);//no horizontal, just vertical scrolling
  wxBoxSizer * pagevbox = new wxBoxSizer(wxVERTICAL);

  item * questionpointer;
  item * answerpointer;
  item * connectorpointer;
  if (direction == 0){
    questionpointer = subjectid;
    answerpointer = objectid;
    connectorpointer = relationid;
  }
  else {
    questionpointer = objectid;
    answerpointer = subjectid;
    connectorpointer = relationid->get_inverse();
  }
  std::vector<item *> allanswers = questionpointer->get_objects(connectorpointer);

  if ((direction == 0 && forwardmode == 1) || (direction !=0 && backwardmode == 1)){//user needs to state all matching items (vocabulary mode)
    questionpointer->display_hint(connectorpointer,allanswers,kg,firstpanel,pagevbox);
  }
  else {
    std::vector<item *> answervec;
    answervec.push_back(answerpointer);
    questionpointer->display_hint(connectorpointer,answervec,kg,firstpanel,pagevbox);
  }
  wxButton * showanswer = new wxButton(firstpanel, 50, wxT("Show answer"));//button for going to the linked concept
  kg->Connect(50, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnShowAnswer));
  //set corresponding keyboard shortcuts
  wxAcceleratorEntry entries[1];
  entries[0].Set(wxACCEL_NORMAL, WXK_SPACE, 50);
  wxAcceleratorTable accel(1, entries);
  kg->SetAcceleratorTable(accel);

  pagevbox->Add(showanswer, 0, wxALL  | wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL, 15);
  firstpanel->SetSizer(pagevbox);
  kg->DisplayNextPage(firstpanel);
}

void statement::answer_print(int direction, Knomegui * kg){
  item * questionpointer;
  item * answerpointer;
  item * connectorpointer;
  if (direction == 0){
    questionpointer = subjectid;
    answerpointer = objectid;
    connectorpointer = relationid;
  }
  else {
    questionpointer = objectid;
    answerpointer = subjectid;
    connectorpointer = relationid->get_inverse();
  }
  std::vector<item *> allanswers = questionpointer->get_objects(connectorpointer);

  wxScrolled<wxPanel>* firstpanel = new wxScrolled<wxPanel>(kg->GetBook(),wxID_ANY);
  firstpanel->SetScrollRate(10,10);//no horizontal, just vertical scrolling
  wxBoxSizer * pagevbox = new wxBoxSizer(wxVERTICAL);
  
  if ((direction == 0 && forwardmode == 1) || (direction !=0 && backwardmode == 1)){//user needs to state all matching items (vocabulary mode)
    wxFlexGridSizer *fgs = new wxFlexGridSizer(2+allanswers.size(), 1, 9, 25);
    questionpointer->print_gui(-2,firstpanel,fgs,kg);
    connectorpointer->print_gui(1,firstpanel,fgs,kg);
    for (unsigned a=0; a<allanswers.size(); a++){
	    allanswers[a]->print_gui(-1,firstpanel,fgs,kg);
    }
    fgs->AddGrowableCol(0, 1);//make the first column expand if the window is expanded
    pagevbox->Add(fgs, 1, wxALL | wxEXPAND, 15);
  }
  else {
    wxFlexGridSizer *fgs = new wxFlexGridSizer(3, 1, 9, 25);
    questionpointer->print_gui(-2,firstpanel,fgs,kg);
    connectorpointer->print_gui(1,firstpanel,fgs,kg);
    answerpointer->print_gui(-1,firstpanel,fgs,kg);
    fgs->AddGrowableCol(0, 1);//make the first column expand if the window is expanded
    pagevbox->Add(fgs, 1, wxALL | wxEXPAND, 15);
  }
  wxButton * showansweritem = new wxButton(firstpanel,70,wxT("Display full answer item (A)"));
  pagevbox->Add(showansweritem,0,wxALL | wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL, 15);
  kg->Connect(70,wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnAnswerItemShowButton));
  add_grading_buttons(kg,firstpanel,pagevbox);
  firstpanel->SetSizer(pagevbox);
  kg->DisplayNextPage(firstpanel);
}
  
void statement::answer_item_print(int direction, Knomegui * kg){
  wxScrolled<wxPanel>* firstpanel = new wxScrolled<wxPanel>(kg->GetBook(),wxID_ANY);
  firstpanel->SetScrollRate(10,10);//no horizontal, just vertical scrolling
  wxBoxSizer * pagevbox = new wxBoxSizer(wxVERTICAL);

  std::vector<item *> itemstoshow;
  if (direction == 0){
    if (forwardmode == 1){//user needs to supply all answers, so we want to display all answer items below each other
      itemstoshow = subjectid->get_objects(relationid);
    }
    else {
      itemstoshow.push_back(objectid);
    }
  }
  else {
    if (backwardmode == 1){//user needs to supply all answers, so we want to display all answer items below each other
      itemstoshow = objectid->get_objects(relationid->get_inverse());
    }
    else {
      itemstoshow.push_back(subjectid);
    }
  }
  //re-use the display_hint function, but with an empty answers vector and nullptr as connectorpointer so that all the objects are shown
  std::vector<item *> allanswers;
  for (unsigned a=0; a<itemstoshow.size(); a++){
    itemstoshow[a]->display_hint(nullptr,allanswers,kg,firstpanel,pagevbox);
  }
  add_grading_buttons(kg,firstpanel,pagevbox);
  firstpanel->SetSizer(pagevbox);
  kg->DisplayNextPage(firstpanel);
}

void statement::add_grading_buttons(Knomegui * kg,wxScrolled<wxPanel>* firstpanel,wxBoxSizer * pagevbox){
  wxBoxSizer * buttonhbox = new wxBoxSizer(wxHORIZONTAL);
  wxButton * failure = new wxButton(firstpanel, 60, wxT("Failure (0)"));
  wxButton * difficult = new wxButton(firstpanel, 61, wxT("Difficult (1)"));
  wxButton * good = new wxButton(firstpanel, 62, wxT("Good (2)"));
  wxButton * easy = new wxButton(firstpanel, 63, wxT("Easy (3)"));
  kg->Connect(60, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnGradingButton));
  kg->Connect(61, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnGradingButton));
  kg->Connect(62, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnGradingButton));
  kg->Connect(63, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnGradingButton));
  kg->Connect(wxEVT_KEY_DOWN,wxKeyEventHandler(Knomegui::OnGradingKeypress));
  buttonhbox->Add(failure, 0, wxALL | wxALIGN_TOP, 15);
  buttonhbox->Add(difficult, 0, wxALL | wxALIGN_TOP, 15);
  buttonhbox->Add(good, 0, wxALL | wxALIGN_TOP, 15);
  buttonhbox->Add(easy, 0, wxALL | wxALIGN_TOP, 15);
  pagevbox->Add(buttonhbox, wxALL | wxALIGN_TOP | wxALIGN_CENTER_HORIZONTAL, 15);

  //Set up keyboard shortcuts for the grading buttons in an accelerator table
  wxAcceleratorEntry entries[9];
  entries[0].Set(wxACCEL_NORMAL, WXK_NUMPAD0, 60);
  entries[1].Set(wxACCEL_NORMAL, WXK_NUMPAD1 , 61);
  entries[2].Set(wxACCEL_NORMAL, WXK_NUMPAD2 , 62);
  entries[3].Set(wxACCEL_NORMAL, WXK_NUMPAD3 , 63);
  entries[4].Set(wxACCEL_NORMAL, (int) 'A' , 70);//display full answer item
  entries[5].Set(wxACCEL_NORMAL, (int) '0', 60);
  entries[6].Set(wxACCEL_NORMAL, (int) '1' , 61);
  entries[7].Set(wxACCEL_NORMAL, (int) '2' , 62);
  entries[8].Set(wxACCEL_NORMAL, (int) '3' , 63);
  wxAcceleratorTable accel(9, entries);
  kg->SetAcceleratorTable(accel);
}
  


//     for (unsigned a=0; a<allanswers.size(); a++){
//       //print answer
//       std::cout << "\t[";
//       allanswers[a]->print(0);
//       std::cout << "]" << std::endl << std::endl;
//     }
//     std::cout << "Directly to grading [0] or display item info [1]?" << std::endl;
//     std::cin >> choice;
//     std::cin.clear();
//     std::cin.ignore();
//     if (choice == 1){
//       std::vector<item *> answervec;
//       item * dummypointer = NULL;
//       answerpointer->display_hint(dummypointer,answervec);
//       std::cin.ignore();//wait for user to press enter
//     }
//   }
//   else {
    
    
//     std::cout << "Show answer [0] or show hint [1]?" << std::endl;
//     unsigned choice;
//     std::cin >> choice;
//     std::cin.clear();
//     std::cin.ignore();
//     if (choice == 1){
//       std::vector<item *> answervec;
//       answervec.push_back(answerpointer);
//       questionpointer->display_hint(connectorpointer,answervec);
//       std::cin.ignore();//wait for user to press enter
//     }

//     //print answer
//     std::cout << "\t[";
//     answerpointer->print(0);
//     std::cout << "]" << std::endl << std::endl;
//     std::cout << "Directly to grading [0] or display item info [1]?" << std::endl;
//     std::cin >> choice;
//     std::cin.clear();
//     std::cin.ignore();
//     if (choice == 1){
//       std::vector<item *> answervec;
//       item * dummypointer = NULL;
//       answerpointer->display_hint(dummypointer,answervec);
//       std::cin.ignore();//wait for user to press enter
//     }
//   }
// }

std::vector<std::string> statement::get_text(){
  
}

void statement::setname(std::string newname,model * m){
  
}

void statement::printquizsettings(Knomegui * kg, wxBoxSizer * pagevbox, wxScrolled<wxPanel>* firstpanel){
  wxPanel * quizsettingspanel = new wxPanel(firstpanel,-1);
  wxStaticBox *st = new wxStaticBox(quizsettingspanel, -1, wxT("Quiz settings"), wxPoint(5, 5), wxSize(400, 90));
  wxCheckBox * quizforward = new wxCheckBox(quizsettingspanel, 93, wxT("forward"), wxPoint(15, 30));
  kg->Connect(93, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Knomegui::OnCheckboxQuizforward));
  wxCheckBox * quizbackward = new wxCheckBox(quizsettingspanel, 94, wxT("backward"), wxPoint(15, 60));
  kg->Connect(94, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Knomegui::OnCheckboxQuizbackward));
  wxCheckBox * vocabforward = new wxCheckBox(quizsettingspanel, 95, wxT("ask for all answers forward"), wxPoint(120, 30));
  vocabforward->SetValue(forwardmode);
  kg->Connect(95, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Knomegui::OnCheckboxVocabforward));
  wxCheckBox * vocabbackward = new wxCheckBox(quizsettingspanel, 96, wxT("ask for all answers backward"), wxPoint(120, 60));
  vocabbackward->SetValue(backwardmode);
  kg->Connect(96, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(Knomegui::OnCheckboxVocabbackward));
  if (quiztype == 1 | quiztype == 2){
    quizforward->SetValue(true);
  }
  if (quiztype == 1 | quiztype == 3){
    quizbackward->SetValue(true);
  }
  pagevbox->Add(quizsettingspanel);
}

void statement::updateforward(bool check){
  //if true: 0->2, 3-> 1
  //if false: 1->3, 2->0
  if (check){
    if (quiztype == 0){
      quiztype = 2;
      if (!Efactors.size()){
	      initialize_metrics();
      }
    }
    else if (quiztype == 3){
      quiztype = 1;
    }
  }
  else {
    if (quiztype == 1){
      quiztype = 3;
    }
    else if (quiztype == 2){
      quiztype = 0;
    }
  }
}

void statement::updatebackward(bool check){
  //if true: 0->3, 2-> 1
  //if false: 1->2, 3->0
  if (check){
    if (quiztype == 0){
      quiztype = 3;
      if (!Efactors.size()){
	      initialize_metrics();
      }
    }
    else if (quiztype == 2){
      quiztype = 1;
    }
  }
  else {
    if (quiztype == 1){
      quiztype = 2;
    }
    else if (quiztype == 3){
      quiztype = 0;
    }
  }
}

void statement::updatevocabforward(bool check){
  forwardmode = check;
}

void statement::updatevocabbackward(bool check){
  backwardmode = check;
}