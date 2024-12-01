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

//#pragma once
#include <wx/wx.h>
#include <wx/simplebook.h>
#include "model.h"


class Knomegui : public wxFrame
{

private:
  wxBoxSizer *vbox;
  //wxScrolled<wxPanel>* panel;
  wxPanel* panel;
  wxSimplebook* mypanelbook;
  model KM;
  wxToolBar * toolbar;
  
public:
  Knomegui(const wxString& title);
  wxSimplebook* GetBook() {return mypanelbook;};
  void DisplayFirstPage(wxScrolled<wxPanel> * newpanel);
  void DisplayNextPage(wxScrolled<wxPanel> * newpanel);

  void OnStar(wxCommandEvent& event);
  void OnNavigate(wxCommandEvent& event);
  void OnConceptEdit(wxCommandEvent& event);
  void OnCheckboxQuizforward(wxCommandEvent& event);
  void OnCheckboxQuizbackward(wxCommandEvent& event);
  void OnCheckboxVocabforward(wxCommandEvent& event);
  void OnCheckboxVocabbackward(wxCommandEvent& event);
  void OnEditStatement(wxCommandEvent& event);
  void OnDelete(wxCommandEvent& event);
  void OnGoTo(wxCommandEvent& event);
  void OnAdd(wxCommandEvent& event);
  void OnGoToStar(wxCommandEvent& event);
  void OnQuiz(wxCommandEvent& event);
  void OnShowAnswer(wxCommandEvent& event);
  void OnDisplayHint(wxCommandEvent& event);
  void OnGradingButton(wxCommandEvent& event);
  void OnGradingKeypress(wxKeyEvent& event);
  void OnEndQuiz(wxCommandEvent& event);
  void OnAnswerItemShowButton(wxCommandEvent& event);
  void ActivateToolbar();
  void OnSearch(wxCommandEvent& event);
  void OnInfo(wxCommandEvent& event);
};
