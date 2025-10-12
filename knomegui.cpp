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

#include "knomegui.h"
#include "model.h"
#include "statement.h"
#include "utilfunctions.h"
#include <iostream>
#include <wx/wx.h>
#include <wx/statline.h>


const int ID_STAR = 1;
const int ID_DELETE = 2;
const int ID_GOTO = 3;
const int ID_PLUS = 4;
const int ID_GOTOSTAR = 5;
const int ID_QUIZ = 6;
const int ID_ENDQUIZ = 7;
const int ID_SEARCH = 8;
const int ID_INFO = 9;

//all user-defined IDs should be positive
//other IDs currently in use
//50, 51: show answer and hint in quiz
//60-63: grading buttons for quiz
//70: display full answer item in quiz
//93-96: change quiz settings
//97, 98 and 99 for editing statements
//from 101 navigating in the main view
//from 10,000 opening files (potentially problematic if the main view has more than 5000 items or if there are more than 22,000 file items in total because apparently the highest id used should be 32765)


Knomegui::~Knomegui(){
  KM.save_to_file();//model is always saved before exiting to prevent losing progress when exiting in the middle of a quiz
}


Knomegui::Knomegui(const wxString& title)
       : wxFrame(NULL, -1, title, wxPoint(-1, -1), wxSize(900, 700)) //model should be constructed by default constructor
{ 
  wxImage::AddHandler( new wxPNGHandler );//required for png buttons
  panel = new wxPanel(this,wxID_ANY);
  vbox = new wxBoxSizer(wxVERTICAL);
  mypanelbook = new wxSimplebook(panel,wxID_ANY);
  SetBackgroundColour(wxColour(207, 235, 252));//uses rgb colour system

  wxImage::AddHandler(new wxPNGHandler);
  wxBitmap star(wxT("star3.png"), wxBITMAP_TYPE_PNG);
  wxBitmap gotostar(wxT("gotostar.png"), wxBITMAP_TYPE_PNG);
  wxBitmap trash(wxT("trash.png"), wxBITMAP_TYPE_PNG);
  wxBitmap Goto(wxT("goto.png"), wxBITMAP_TYPE_PNG);
  wxBitmap Plus(wxT("plus.png"), wxBITMAP_TYPE_PNG);
  wxBitmap Quizcard(wxT("quizcard2.png"), wxBITMAP_TYPE_PNG);
  wxBitmap endquiz(wxT("endquiz.png"), wxBITMAP_TYPE_PNG);
  wxBitmap Search(wxT("search.png"), wxBITMAP_TYPE_PNG);
  wxBitmap Info(wxT("info.png"),wxBITMAP_TYPE_PNG);

  toolbar = CreateToolBar();
  toolbar->AddTool(ID_STAR, wxT("Mark item"), star);
  toolbar->SetToolShortHelp(ID_STAR,_("Mark item"));
  toolbar->AddTool(ID_GOTOSTAR, wxT("Go to marked item"), gotostar);
  toolbar->SetToolShortHelp(ID_GOTOSTAR,_("Go to marked item"));
  toolbar->AddTool(ID_DELETE, wxT("Delete item"),trash);
  toolbar->SetToolShortHelp(ID_DELETE,_("Delete item"));
  toolbar->AddTool(ID_GOTO,wxT("Go to item"),Goto);
  toolbar->SetToolShortHelp(ID_GOTO,_("Go to item"));
  toolbar->AddTool(ID_SEARCH,wxT("Search"),Search);
  toolbar->SetToolShortHelp(ID_SEARCH,_("Perform a string search"));
  toolbar->AddTool(ID_PLUS,wxT("Add statement"),Plus);
  toolbar->SetToolShortHelp(ID_PLUS,_("Add a statement"));
  toolbar->AddTool(ID_QUIZ,wxT("Start quiz"),Quizcard);
  toolbar->SetToolShortHelp(ID_QUIZ,_("Start quiz"));
  toolbar->AddTool(ID_ENDQUIZ,wxT("End/pause quiz"),endquiz);
  toolbar->SetToolShortHelp(ID_ENDQUIZ,_("End/pause quiz and switch to browsing"));
  toolbar->EnableTool(ID_ENDQUIZ,false);//disabled because at the beginning we are not in quiz mode
  toolbar->AddTool(ID_INFO,wxT("Show license information"),Info);
  toolbar->SetToolShortHelp(ID_INFO,_("Show license information"));

  toolbar->Realize();
  Connect(ID_STAR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Knomegui::OnStar));	
  Connect(ID_DELETE,wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Knomegui::OnDelete));
  Connect(ID_GOTO,wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Knomegui::OnGoTo));
  Connect(ID_PLUS,wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Knomegui::OnAdd));
  Connect(ID_GOTOSTAR,wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Knomegui::OnGoToStar));
  Connect(ID_QUIZ,wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Knomegui::OnQuiz));
  Connect(ID_ENDQUIZ,wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Knomegui::OnEndQuiz));
  Connect(ID_SEARCH,wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Knomegui::OnSearch));
  Connect(ID_INFO,wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(Knomegui::OnInfo));

  KM.browse("bc0",this);
  
}

void Knomegui::DisplayFirstPage(wxScrolled<wxPanel> * newpanel) {
  mypanelbook->ShowNewPage(newpanel);
  vbox->Add(mypanelbook,1,wxEXPAND);
  panel->SetSizer(vbox);
  Centre();
}

void Knomegui::DisplayNextPage(wxScrolled<wxPanel> * newpanel) {
  mypanelbook->ShowNewPage(newpanel);
  Centre();
  mypanelbook->DeletePage(0);//remove previous page so that the book structure does not grow so much over the browsing history. Maybe this can later keep a few pages to allow going back.
}

void Knomegui::OnStar(wxCommandEvent& event)//bookmark item
{
 KM.markitem();
}

void Knomegui::OnNavigate(wxCommandEvent& event){
  int target_item_counter = event.GetId()-101;
  KM.navigate(target_item_counter,this);
}

void Knomegui::OnConceptEdit(wxCommandEvent& event){
  wxTextCtrl* textCtrl = dynamic_cast<wxTextCtrl*>(event.GetEventObject());
    if (textCtrl){
      KM.edit_current_item(mywxstringtostdstring(textCtrl->GetValue()));
    }  
  
}

void Knomegui::OnCheckboxQuizforward(wxCommandEvent& event){
  statement * sp = static_cast<statement*>(KM.get_current_item());
  sp->updateforward(event.IsChecked());
  if (!event.IsChecked()){//if the forward direction just got deactivated
    KM.remove_quizitems(sp->get_id(),0);
  }
  KM.save_to_file();
}

void Knomegui::OnCheckboxQuizbackward(wxCommandEvent& event){
  statement * sp = static_cast<statement*>(KM.get_current_item());
  sp->updatebackward(event.IsChecked());
  if (!event.IsChecked()){//if the backward direction just got deactivated
    KM.remove_quizitems(sp->get_id(),1);
  }
  KM.save_to_file();
}

void Knomegui::OnCheckboxVocabforward(wxCommandEvent& event){
  statement * sp = static_cast<statement*>(KM.get_current_item());
  sp->updatevocabforward(event.IsChecked());
  KM.save_to_file();
}

void Knomegui::OnCheckboxVocabbackward(wxCommandEvent& event){
  statement * sp = static_cast<statement*>(KM.get_current_item());
  sp->updatevocabbackward(event.IsChecked());
  KM.save_to_file();
}

void Knomegui::OnEditStatement(wxCommandEvent& event){
  int id = event.GetId();
  statement * sp = static_cast<statement*>(KM.get_current_item());
  sp->editgui(id-96,KM,this);//because the button ids are 97, 98 and 99 and the choices for statement change are 1, 2, and 3
  sp->displaygui(this,false);//show the page for the new statement so that the user can select quiz settings
}

void Knomegui::OnDelete(wxCommandEvent& event){
  wxMessageDialog *dial = new wxMessageDialog(NULL,wxT("Are you sure you want to delete the current concept?"), wxT("Question"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
  if (dial->ShowModal() == wxID_YES){
    KM.delete_current_item(this);
  }
}

void Knomegui::OnGoTo(wxCommandEvent& event){
  wxTextEntryDialog dialog(this,"Which concept to you want to go to?","");
  if (dialog.ShowModal()==wxID_OK){
    std::string targetitem = mywxstringtostdstring(dialog.GetValue());
    KM.go_to_item(targetitem,this);
  }
}

void Knomegui::OnAdd(wxCommandEvent& event){
  item * newstatement = KM.add_statement(KM.get_current_item(),this);
  newstatement->displaygui(this,false);//show the page for the new statement so that the user can select quiz settings
}

void Knomegui::OnGoToStar(wxCommandEvent& event){
  KM.go_to_marked_item(this);
}

void Knomegui::OnQuiz(wxCommandEvent& event){
  //disable tools that only make sense during browsing and enable the switch to browsing button
  toolbar->EnableTool(ID_STAR,false);
  toolbar->EnableTool(ID_GOTOSTAR,false);
  toolbar->EnableTool(ID_DELETE,false);
  toolbar->EnableTool(ID_GOTO,false);
  toolbar->EnableTool(ID_SEARCH,false);
  toolbar->EnableTool(ID_PLUS,false);
  toolbar->EnableTool(ID_QUIZ,false);
  toolbar->EnableTool(ID_ENDQUIZ,true);

  KM.start_quiz(this);
}

void Knomegui::OnShowAnswer(wxCommandEvent& event){
  SetAcceleratorTable(wxNullAcceleratorTable);//deactivate keyboard shortcuts so that they cannot be used on other screens
  KM.answer(this);
}

void Knomegui::OnDisplayHint(wxCommandEvent& event){
  SetAcceleratorTable(wxNullAcceleratorTable);//deactivate keyboard shortcuts so that they cannot be used on other screens
  KM.hint(this);
}

void Knomegui::OnGradingButton(wxCommandEvent& event){
  int response = event.GetId()-60;//because the four buttons for failure, difficult, good, and easy have IDs 60, 61, 62, 63
  SetAcceleratorTable(wxNullAcceleratorTable);//deactivate keyboard shortcuts so that they cannot be used on other screens
  KM.grading_continue_quiz(response,this);
}

void Knomegui::OnGradingKeypress(wxKeyEvent& event){
  std::cout << "Key pressed" << std::endl;
  wxChar keyid = event.GetUnicodeKey();
  std::cout << keyid << std::endl;
}

void Knomegui::OnEndQuiz(wxCommandEvent& event){
  ActivateToolbar();
  KM.switchtobrowsing(this);
}

void Knomegui::OnAnswerItemShowButton(wxCommandEvent& event){
  SetAcceleratorTable(wxNullAcceleratorTable);//deactivate keyboard shortcuts so that they cannot be used on other screens
  KM.answeritemshow(this);
}

void Knomegui::ActivateToolbar(){
  toolbar->EnableTool(ID_STAR,true);
  toolbar->EnableTool(ID_GOTOSTAR,true);
  toolbar->EnableTool(ID_DELETE,true);
  toolbar->EnableTool(ID_GOTO,true);
  toolbar->EnableTool(ID_SEARCH,true);
  toolbar->EnableTool(ID_PLUS,true);
  toolbar->EnableTool(ID_QUIZ,true);
  toolbar->EnableTool(ID_ENDQUIZ,false);
}

void Knomegui::OnSearch(wxCommandEvent& event){
  wxTextEntryDialog dialog(this,"Please enter the search query.","");
  if (dialog.ShowModal()==wxID_OK){
    std::string searchquery = mywxstringtostdstring(dialog.GetValue());
    KM.search(searchquery,this);
  }
}

void Knomegui::OnInfo(wxCommandEvent& event){
    wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Copyright (C) 2024  Meike Wittmann. This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>."), wxT("License information."), wxOK);
    dial->ShowModal();
}

void Knomegui::OnOpenImage(wxCommandEvent& event){
  int id = event.GetId() - 10000;
  std::string fileitemid =  "f" + std::to_string(id);
  KM.open_image(fileitemid);
}