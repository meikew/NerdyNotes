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
#include "fileitem.h"
#include "item.h"
#include "model.h"
#include "knomegui.h"
#include "statement.h"
#include "utilfunctions.h"
#include <wx/filename.h>
#include "bufferedbitmap.h"

const int ID_STAR = 1;

fileitem::fileitem(std::string fname, std::string id)
  : item(id), filename(fname)
{
}

void fileitem::destructor(model& M){
  M.remove_from_filedictionary(filename);
  std::string command = "rm " + filename;
  system(command.c_str());
  item::destructor(M);
}

void fileitem::print(int level){
  if (level){
    std::cout << filename;
  }
  else {//level 0: used for print_self and for quizzes
    std::cout << filename;
    std::string command = "xdg-open " + filename + " 2> errorfile & ";//2 means error output, if error output is printed to the console knome freezes
    system(command.c_str());
  }
}

std::vector<item *> fileitem::print_self(){
  //print(0);
  std::vector<item *> emptyvec;
  return emptyvec;
}

void fileitem::print_gui(int level,wxScrolled<wxPanel>* firstpanel,wxFlexGridSizer *fgs,Knomegui * kg){
  wxFileName fn (filename);
  wxString extension = fn.GetExt();
  bool validimagetype = false;
  if (extension == wxT("png") || extension == wxT("jpg") || extension == wxT("jpeg") || extension == wxT("gif")){
    validimagetype = true;
  }
  bool imageshown = false;
  switch (level){
  case 0:
  {
    std::string command = "xdg-open " + filename + " 2> errorfile & ";//2 means error output, if error output is printed to the console knome freezes
    system(command.c_str());
    wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,filename,wxPoint(-1, -1), wxSize(240, 40), wxTE_MULTILINE | wxTE_PROCESS_ENTER);//editable text box for the focal concept
    kg->Connect(wxEVT_TEXT_ENTER, wxCommandEventHandler(Knomegui::OnConceptEdit));
    tc1->SetBackgroundColour(wxColour("#fbfac3"));//make the text box yellow
    fgs->Add(tc1,0,wxALIGN_LEFT);
    break;
  }
  case 1:
  {
    if (validimagetype){
      imageshown = displayimage(firstpanel,fgs,400,true,kg,false);
    }
    if (!imageshown){//if the file is not of the selected types or if the file cannot be loaded
      wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,filename,wxPoint(-1, -1), wxSize(400, 60), wxTE_MULTILINE | wxTE_READONLY);
      tc1->SetBackgroundColour(wxColour("#e7fbc3"));//make the text box light green
      fgs->Add(tc1,0,wxALIGN_RIGHT);
    }
    break;
  }
  case -1://for quiz answers
  {
    if (validimagetype){
      imageshown = displayimage(firstpanel,fgs,600,true,kg,true);
    }
    if (!imageshown){//if the file is not of the selected types or if the file cannot be loaded
      std::string command = "xdg-open " + filename + " 2> errorfile & ";//2 means error output, if error output is printed to the console knome freezes
      system(command.c_str());
      wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,filename,wxPoint(-1, -1), wxSize(400, 60), wxTE_MULTILINE | wxTE_READONLY);
      tc1->SetBackgroundColour(wxColour("#e7fbc3"));//make the text box light green
      fgs->Add(tc1,0,wxALIGN_RIGHT);
    }
    break;
  }
  case -2: //for the subject in quiz hints and answers so that the file is not opened multiple times
  {
    if (validimagetype){
      imageshown = displayimage(firstpanel,fgs,600,false,kg,true);
    }
    if (!imageshown){//if the file is not of the selected types or if the file cannot be loaded
      //file is not shown again
      wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,filename,wxPoint(-1, -1), wxSize(240, 40), wxTE_MULTILINE | wxTE_READONLY);
      tc1->SetBackgroundColour(wxColour("#fbfac3"));//make the text box yellow
      fgs->Add(tc1,0,wxALIGN_LEFT);
    }
    break;
  }
  case -3: //for the subject in quiz original questions
  {
    if (validimagetype){
      imageshown = displayimage(firstpanel,fgs,600,false,kg,true);
    }
    if (!imageshown){//if the file is not of the selected types or if the file cannot be loaded
      std::string command = "xdg-open " + filename + " 2> errorfile & ";//2 means error output, if error output is printed to the console knome freezes
      system(command.c_str());
      wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,filename,wxPoint(-1, -1), wxSize(240, 40), wxTE_MULTILINE | wxTE_READONLY);
      tc1->SetBackgroundColour(wxColour("#fbfac3"));//make the text box yellow
      fgs->Add(tc1,0,wxALIGN_LEFT);
    }
    break;
  }
  default://for parts of statements
  {
    if (validimagetype){
      imageshown = displayimage(firstpanel,fgs,122,true,kg,false);
    }
    if (!imageshown){//if the file is not of the selected types or if the file cannot be loaded
      wxTextCtrl *tc1 = new wxTextCtrl(firstpanel, -1,filename,wxPoint(-1, -1), wxSize(122, 100), wxTE_MULTILINE | wxTE_READONLY);//editable text box for the focal concept
      tc1->SetBackgroundColour(wxColour("#e7fbc3"));//make the text box light green
      fgs->Add(tc1,0,wxALIGN_RIGHT);
    }
  }
  }
}



void fileitem::edit(model & M){
  std::string oldname = filename;
  std::cout << "new filename: ";
  std::cin >> filename;
  std::cin.ignore();
  M.update_filedictionary(oldname,filename);
}

void fileitem::filewrite(std::fstream& outfile){
  item::filewrite(outfile);
  outfile << filename << std::endl;
}

void fileitem::setname(std::string newname,model * m){
  bool namealreadyinuse = m->find_in_file_dictionary(newname);
  if (namealreadyinuse){
    wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("File name already in use. Please try again."), wxT("Error"), wxOK | wxICON_ERROR);
    dial->ShowModal();
  }
  else {
    std::string oldname = filename;
    filename = newname;
    m->update_filedictionary(oldname,filename);
  }
}


bool fileitem::displayimage(wxScrolled<wxPanel>* firstpanel, wxFlexGridSizer * fgs, int desiredwidth, bool rightaligned, Knomegui * kg, bool quiz){
  wxImage::AddHandler(new wxPNGHandler);
  wxImage::AddHandler(new wxJPEGHandler);
  wxBitmap image(filename, wxBITMAP_TYPE_ANY);
  if (image.IsOk()){
    const wxSize bmpSize = image.GetSize();
    BufferedBitmap * staticBitmap = new BufferedBitmap(firstpanel, wxID_ANY, image, wxDefaultPosition, wxSize(desiredwidth, desiredwidth*bmpSize.GetHeight()/bmpSize.GetWidth()));
    if (quiz){
      wxFlexGridSizer * imagehbox = new wxFlexGridSizer(1,2,9,9);//a horizontal sizer with 1 row and 2 columns to which we can add the image and a button that allows to open the image
      imagehbox->Add(staticBitmap, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
      //get id of the file item as a number
      std::string idnumberstring = id;
      idnumberstring.erase(idnumberstring.begin());
      int idnumber = std::stoi(idnumberstring);
      //add a button via which the image file can be opened
      wxBitmapButton *openimage = new wxBitmapButton(firstpanel, 1000 + idnumber, wxBitmap(wxT("open_image.png"), wxBITMAP_TYPE_PNG), wxPoint(40, 10));
      imagehbox->Add(openimage, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
      kg->Connect(1000 + idnumber, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Knomegui::OnOpenImage));
      if (rightaligned){
        fgs->Add(imagehbox, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
      }
      else {
        fgs->Add(imagehbox, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
      }
    }
    else {
      if (rightaligned){
        fgs->Add(staticBitmap, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
      }
      else {
        fgs->Add(staticBitmap, 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
      }
    }
    return true; 
  }
  else {
    return false;
  }
}