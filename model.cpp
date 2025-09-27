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

#include "model.h"
#include "statement.h"
#include "fileitem.h"
#include "knomegui.h"
#include "utilfunctions.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/random.hpp>
#include <wx/choicdlg.h>

extern boost::mt19937 rng;

model::model()
  : conceptcounter(0), relationcounter(0), statementcounter(0), filecounter(0)
{
  add_concept("NerdyNotes","bc",conceptcounter++);//bc for basic concept node that cannot be changed by the user

  //some basic asymmetric relations and their inverse relations
  add_relation("has detail","is part of","br",relationcounter);
  relationcounter +=2;
  add_relation("is a","has instance","br",relationcounter);
  relationcounter +=2;
  
  //and some basic symmetric relations
  add_relation("is equal to","is equal to","br",relationcounter++);
  add_relation("is similar to","is similar to","br",relationcounter++);

  conceptcounter =0;
  relationcounter=0;
  statementcounter=0;

  system("cp concepts.txt concepts_back.txt");
  system("cp relations.txt relations_back.txt");
  system("cp statements.txt statements_back.txt");
  system("cp files.txt files_back.txt");
  conceptfile.open("concepts.txt",std::ios::in);
  relationfile.open("relations.txt",std::ios::in);
  statementfile.open("statements.txt",std::ios::in);
  filefile.open("files.txt",std::ios::in);

  std::cout << "Reading in concept file ... " << std::endl;
  std::string line;
  while (getline(conceptfile,line)){
    std::istringstream ss(line);
    int counter;
    ss >> counter;
    ss.ignore(1);
    conceptcounter = std::max(conceptcounter,counter+1);
    std::string x;        
    getline( ss, x, '\t' ); 
    add_concept(x,"c",counter);
  }
  std::cout << "done" << std::endl;

  std::cout << "Reading in file file ... " << std::endl;
  while (getline(filefile,line)){
    std::istringstream ss(line);
    int counter;
    ss >> counter;
    ss.ignore(1);
    filecounter = std::max(filecounter,counter+1);
    std::string x;        
    getline( ss, x, '\t' ); 
    add_fileitem(x,"f",counter);
  }
  std::cout << "done" << std::endl;

  std::cout << "Reading in relation file ... " << std::endl;
  std::vector<std::string> namevec;
  std::vector<std::string> inv_id_vec;
  while (getline(relationfile,line)){
    std::istringstream ss(line);
    int counter;
    ss >> counter;
    std::string inv_id;
    ss >> inv_id;
    inv_id_vec.push_back(inv_id);
    ss.ignore(1);
    relationcounter = std::max(relationcounter,counter+1);
    std::string x;        
    getline( ss, x, '\t' ); 
    add_relation(x,"r",counter);
    namevec.push_back(x);
  }
  std::cout << "done" << std::endl;

  for (unsigned i=0; i< namevec.size(); i++){
    relationdictionary[namevec[i]]->set_inverse(relationmap[inv_id_vec[i]]);
  }

  int newquestions = 0;
  std::cout << "Reading in statement file ... " << std::endl;
  std::vector<std::string> statementqueue;
  while (getline(statementfile,line)){//read in all lines, line by line
    statementqueue.push_back(line);
  }
  unsigned i = 0;
  while (i < statementqueue.size()){//for each statement
    //std::cout << i << " " << statementqueue[i] << std::endl;
    std::istringstream ss(statementqueue[i]);
    int counter;
    ss >> counter;
    ss.ignore(1);
    statementcounter = std::max(statementcounter,counter+1);
    std::string subid;        
    getline( ss, subid, '\t' );
    std::string relid;        
    getline( ss, relid, '\t' ); 
    std::string objid;        
    getline( ss, objid, '\t' ); 
    bool forwardmode;
    ss >> forwardmode;
    bool backwardmode;
    ss >> backwardmode;
    int quiztype;
    ss >> quiztype;
    std::vector<double> EF;
    std::vector<mytime> reviewdates;
    std::vector<double> Is;
    if (quiztype){
      double ef1;
      ss >> ef1;
      double ef2;
      ss >> ef2;
      EF.push_back(ef1);
      EF.push_back(ef2);

      std::string date1;
      ss >> date1;
      std::stringstream s1;
      s1 << date1;
      mytime m1(s1);
      std::string date2;
      ss >> date2;
      std::stringstream s2;
      s2 << date2;
      mytime m2(s2);
      reviewdates.push_back(m1);
      reviewdates.push_back(m2);
    
      double i1;
      ss >> i1;
      double i2;
      ss >> i2;
      Is.push_back(i1);
      Is.push_back(i2);
    }

    bool success = add_statement(subid,relid,objid,quiztype,forwardmode,backwardmode,EF,reviewdates,Is,"s",counter);
    if (success){//this will be true if the subject and object have been added before
      mytime today;
      if ((quiztype == 1) || (quiztype == 2)){//forward quiz is active
	      if (reviewdates[0] <= today){//due or overdue
	        if ((Is[0]>1)||(newquestions < 400)){//if it is either an old card with an interval larger than one or fewer than 400 new cards have been added so far
	          std::string thisid =  "s" + std::to_string(counter);
	          std::pair <std::string,int> q(thisid,0);//0 indicates the forward quiz direction
	          quizqueue.push_back(q);
	        if (Is[0] <=1) newquestions ++;//keep track of the number of new questions already added
	        }
	      }
      }
      if ((quiztype == 1) || (quiztype == 3)){//backward quiz is active
	      if (reviewdates[1] <= today){//due or overdue
	        if ((Is[1]>1)||(newquestions < 400)){//if it is either an old card with an interval larger than one or fewer than 400 new cards have been added so far
	          std::string thisid =  "s" + std::to_string(counter);
	          std::pair <std::string,int> q(thisid,1);//1 indicates the backward quiz direction
	          quizqueue.push_back(q);
	          if (Is[1] <=1) newquestions ++;//keep track of the number of new questions already added
	        }
	      }
      }
    }
    else {//if subject or object have not been added before (this can happen if they are statements that are later in the file)
      statementqueue.push_back(statementqueue[i]);//add the line again to the end of the statement queue so that it is read again later
    }
    i++;
  }
  std::cout << "done" << std::endl;

  conceptfile.close();
  statementfile.close();
  relationfile.close();
  filefile.close();

  markeditem = itemmap["bc0"];
  currentitem = itemmap["bc0"];
  currentquizitem = nullptr;
  previousquizitem = nullptr;
}

void model::browse(std::string startitem,Knomegui * kg){
  
  item * nextitem1 = itemmap[startitem];
  nextitem1->displaygui(kg,true);
  
}

void model::add_concept(std::string name, std::string prefix, int counter){
  std::string newitemid =  prefix + std::to_string(counter);
  item * newitem = new nameitem(name,newitemid);
  conceptdictionary[name]=newitem;
  itemmap[newitemid]=newitem;
}

void model::add_fileitem(std::string fname, std::string prefix, int counter){
  std::string newitemid =  prefix + std::to_string(counter);
  item * newitem = new fileitem(fname,newitemid);
  filedictionary[fname]=newitem;
  itemmap[newitemid]=newitem;
}

void model::add_relation(std::string name, std::string inv_name, std::string prefix, int counter){
  std::string newrelationid =  prefix + std::to_string(counter);
  relation * newrelation = new relation(name,newrelationid);
  relationdictionary[name]=newrelation;
  relationmap[newrelationid]=newrelation;

  if (name == inv_name){
    newrelation->set_inverse(newrelation);
  }
  else {
    std::string newinvrelationid =  prefix + std::to_string(counter+1);
    relation * newinvrelation = new relation(inv_name,newrelation,newinvrelationid);
    relationdictionary[inv_name]=newinvrelation;
    relationmap[newinvrelationid]=newinvrelation;
    newrelation->set_inverse(newinvrelation);
  }
}

void model::add_relation(std::string name, std::string prefix, int counter){
  std::string newrelationid =  prefix + std::to_string(counter);
  relation * newrelation = new relation(name,newrelationid);
  relationdictionary[name]=newrelation;
  relationmap[newrelationid]=newrelation;
}

item * model::add_statement(item * subpointer, relation * relpointer, item * objpointer, int quiztype, bool forwardmode, bool backwardmode, std::string prefix, int counter){
  std::string newitemid =  prefix + std::to_string(counter);
  item * s1 = new statement(newitemid,subpointer,relpointer,objpointer,quiztype,forwardmode,backwardmode);
  subpointer->add_info(s1,relpointer,objpointer);
  objpointer->add_info(s1,relpointer->get_inverse(),subpointer);
  itemmap[newitemid]=s1;
  return s1;
}

bool model::add_statement(std::string subid, std::string relid, std::string objid, int quiztype, bool forwardmode, bool backwardmode, std::string prefix, int counter){
  std::map<std::string,item *>::iterator its = itemmap.find(subid);
  std::map<std::string,item *>::iterator ito = itemmap.find(objid);
  if ((its == itemmap.end()) || (ito == itemmap.end())){
    return false;
  }
  else {
    item * subpointer = its->second;
    relation * relpointer = relationmap.find(relid)->second;
    item * objpointer = ito->second;
    add_statement(subpointer, relpointer, objpointer, quiztype, forwardmode, backwardmode, prefix, counter);
    return true;
  }
}

void model::add_statement(item * subpointer, relation * relpointer, item * objpointer, int quiztype, bool forwardmode, bool backwardmode, std::vector<double> EF, std::vector<mytime> RD, std::vector<double> Is, std::string prefix, int counter){
  std::string newitemid =  prefix + std::to_string(counter);
  item * s1 = new statement(newitemid,subpointer,relpointer,objpointer,quiztype,forwardmode,backwardmode,EF,RD,Is);
  subpointer->add_info(s1,relpointer,objpointer);
  objpointer->add_info(s1,relpointer->get_inverse(),subpointer);
  itemmap[newitemid]=s1;
}

bool model::add_statement(std::string subid, std::string relid, std::string objid, int quiztype, bool forwardmode, bool backwardmode, std::vector<double> EF, std::vector<mytime> RD, std::vector<double> Is, std::string prefix, int counter){
  std::map<std::string,item *>::iterator its = itemmap.find(subid);
  std::map<std::string,item *>::iterator ito = itemmap.find(objid);
  if ((its == itemmap.end()) || (ito == itemmap.end())){
    return false;
  }
  else {
    item * subpointer = its->second;
    relation * relpointer = relationmap.find(relid)->second;
    item * objpointer = ito->second;
    add_statement(subpointer, relpointer, objpointer, quiztype, forwardmode, backwardmode, EF, RD, Is, prefix, counter);
    return true;
  }
}



item * model::add_statement(item * subpointer, Knomegui * kg){
  conceptfile.open("concepts.txt",std::ios::out | std::ios::app);
	relationfile.open("relations.txt",std::ios::out | std::ios::app);
	statementfile.open("statements.txt",std::ios::out | std::ios::app);
	filefile.open("files.txt",std::ios::out | std::ios::app);
  item * newstatement = currentitem;//because if the user cancels the dialogue, the program should stay at the current item
  relation * relpointer = assign_relation(kg);
  if (relpointer != nullptr){
    item * objpointer = assign_object(kg); 
    if (objpointer != nullptr){ 
      int quiztype = 0;
      bool forwardmode = false;
      bool backwardmode = false;
      std::string newitemid =  "s" + std::to_string(statementcounter);
      newstatement = add_statement(subpointer,relpointer,objpointer,quiztype,forwardmode,backwardmode,"s",statementcounter++);
      currentitem = newstatement;
      itemmap[newitemid]->filewrite(statementfile);
    }
  }
	conceptfile.close();
	statementfile.close();
	relationfile.close();
	filefile.close();
  return newstatement;
}

relation * model::assign_relation(Knomegui * kg){
  bool relationfound = false;
  std::string rel;
  while (!relationfound){
    wxTextEntryDialog dial1(kg,"Relation:","");
    if (dial1.ShowModal()==wxID_OK){
      rel = mywxstringtostdstring(dial1.GetValue());
      std::map<std::string,relation *>::iterator iter = relationdictionary.find(rel);
      if (iter ==relationdictionary.end()){
        wxMessageDialog * dial2 = new wxMessageDialog(NULL,wxT("Would you like to create a new relation with this name?"), wxT("Question"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION);
        if (dial2->ShowModal() == wxID_YES){
	        bool invrelassigned = false;
	        std::string inv_rel;	
	        while (!invrelassigned){
            wxTextEntryDialog dial3(kg,"Inverse relation:","");
            if (dial3.ShowModal()==wxID_OK){
              inv_rel = mywxstringtostdstring(dial3.GetValue());
	            iter = relationdictionary.find(inv_rel);
	            if (iter == relationdictionary.end()){
	              invrelassigned = true;
	            }
	            else {
	              wxMessageDialog *dial4 = new wxMessageDialog(NULL, wxT("Relation name already in use. Please try again."), wxT("Error"), wxOK | wxICON_ERROR);
                dial4->ShowModal();
	            }
            }
            else {
              return nullptr; //this happens when you cancel the dialog
            }
	        }
	        int counter = relationcounter ++;
	        add_relation(rel,inv_rel,"r",counter);
	        relationfound = true;
	        if (rel!=inv_rel){ 
	          relationcounter ++;
	          relationfile << counter << '\t' << relationdictionary[inv_rel]->get_id() << '\t' << rel << std::endl;
	          relationfile << counter+1 << '\t' << relationdictionary[rel]->get_id() << '\t' << inv_rel << std::endl;
	        }
	        else {
	          relationfile << counter << '\t' << relationdictionary[rel]->get_id() << '\t' << rel << std::endl;
	        }	
        }
        else {
          wxMessageDialog *dial5 = new wxMessageDialog(NULL, wxT("OK, please try again."), wxT("Info"), wxOK);
          dial5->ShowModal();
        }
      }
      else {
        relationfound = true;
      }
    }
    else {
      return nullptr; //if you cancel the dialog to enter the relation
    }
  }
  relation * relpointer = relationdictionary[rel];
  return relpointer;
}

item * model::assign_object(Knomegui * kg){
  wxString objectchoices[3] = {wxT("Concept"),wxT("File item"),wxT("Marked item")};
  wxSingleChoiceDialog dial1(kg,wxT("Type of object?"),wxT(""),3,objectchoices); 
  if (dial1.ShowModal()==wxID_OK){
    int choice = dial1.GetSelection();
    item * objpointer = markeditem;

    if (choice == 1){
      bool objectfound = false;  
      std::string fname;
      while (!objectfound){
        wxTextEntryDialog dial2(kg,"File name:","");
        if (dial2.ShowModal()==wxID_OK){
          fname = mywxstringtostdstring(dial2.GetValue());
          objectfound = find_in_file_dictionary(fname);
          if (!objectfound) {
            wxMessageDialog * dial3 = new wxMessageDialog(NULL,wxT("Would you like to create a new file item with this name?"), wxT("Question"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION);
            if (dial3->ShowModal() == wxID_YES){
              filefile << filecounter << '\t' << fname << std::endl;
              add_fileitem(fname,"f",filecounter++);
              objectfound = true;
            }
            else {
              wxMessageDialog *dial4 = new wxMessageDialog(NULL, wxT("OK, please try again."), wxT("Info"), wxOK);
              dial4->ShowModal();
            }
          }
        }
        else {
          return nullptr;
        }
      }
      objpointer = filedictionary[fname];
    }
    if (choice == 0){
      bool objectfound = false;  
      std::string obj;
      while (!objectfound){
        wxTextEntryDialog dial2(kg,"Object:","");
        if (dial2.ShowModal()==wxID_OK){
          obj = mywxstringtostdstring(dial2.GetValue());
          objectfound = find_in_concept_dictionary(obj);
          if (!objectfound) {
            wxMessageDialog * dial3 = new wxMessageDialog(NULL,wxT("Would you like to create a new concept with this name?"), wxT("Question"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION);
            if (dial3->ShowModal() == wxID_YES){
              conceptfile << conceptcounter << '\t' << obj << std::endl;
              add_concept(obj,"c",conceptcounter++);
              objectfound = true;
            }
            else {
              wxMessageDialog *dial4 = new wxMessageDialog(NULL, wxT("OK, please try again."), wxT("Info"), wxOK);
              dial4->ShowModal();
            }
          }
        }
        else {
          return nullptr;
        }
      }
      objpointer = conceptdictionary[obj];
    }
    return objpointer;
  }
  else {
    return nullptr;
  }
}


void model::save_to_file(){
  conceptfile.open("concepts.txt",std::ios::out | std::ios::trunc);
  relationfile.open("relations.txt",std::ios::out | std::ios::trunc);
  statementfile.open("statements.txt",std::ios::out | std::ios::trunc);
  filefile.open("files.txt",std::ios::out | std::ios::trunc);
  for (auto& x : itemmap){
    std::string id = x.first;
    if (id[0]=='c'){//basic nodes are not saved to file
      x.second->filewrite(conceptfile);
    }
    else if (id[0]=='s'){
      x.second->filewrite(statementfile);
    }
    else if (id[0]=='f'){
      x.second->filewrite(filefile);
    }
  }
  for (auto& x : relationmap){
    std::string id = x.first;
    if (id[0]=='r'){//basic nodes are not saved to file
      x.second->filewrite(relationfile);
    }
  }
  conceptfile.close();
  relationfile.close();
  statementfile.close();
  filefile.close();
}

void model::remove_from_itemmap(std::string id){
  itemmap.erase(itemmap.find(id));
}

void model::remove_from_conceptdictionary(std::string name){
  conceptdictionary.erase(conceptdictionary.find(name));
}

void model::remove_from_filedictionary(std::string name){
  filedictionary.erase(filedictionary.find(name));
}

void model::update_conceptdictionary(std::string oldname,std::string newname){
  conceptdictionary[newname]=conceptdictionary[oldname];
  remove_from_conceptdictionary(oldname);
}

void model::update_filedictionary(std::string oldname,std::string newname){
  filedictionary[newname]=filedictionary[oldname];
  remove_from_filedictionary(oldname);
}

void model::start_quiz(Knomegui * kg){
  previousquizitem = nullptr;//to make sure that this doesn't point to a quiz item from a previous quiz that has been deleted in the meantime
  currentquizitem = nullptr;//same
  int numquestions = quizqueue.size();
  if (numquestions){//if there are questions available
    wxString dialoguetext("I have " + std::to_string(numquestions) + " questions for you. \n Would you like to activate recovery mode?");
    wxMessageDialog *dial = new wxMessageDialog(NULL,dialoguetext, wxT("Select quiz mode"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
    recover = 0;
    if (dial->ShowModal() == wxID_YES){
      recover = 1;
    }
    randomquizquestion(kg);
  }
  else {
    wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("There are no questions scheduled for today. Please come back tomorrow."), wxT("You are up to date."), wxOK | wxICON_ERROR);
    dial->ShowModal();
    kg->ActivateToolbar();
    currentitem->displaygui(kg,false);
  }
}

void model::randomquizquestion(Knomegui * kg){
  mytime today;
  boost::uniform_real<> dist(0,1);
  boost::variate_generator<boost::mt19937&, boost::uniform_real<> > uni(rng, dist);
  boost::uniform_smallint<> unint(0,quizqueue.size()-1);
  boost::variate_generator<boost::mt19937&, boost::uniform_smallint<> > randomquestion(rng, unint);
  currentquizindex = randomquestion();
  quizitem * proposedquizitem = dynamic_cast<quizitem*>(itemmap[quizqueue[currentquizindex].first]);
  currentquizdirection = quizqueue[currentquizindex].second;
  int overduedays = today - proposedquizitem->get_reviewdates(currentquizdirection);
  double keepprob = 1;
  if (recover){
    keepprob = std::max(0.1,1-0.07*overduedays);
  }
  if (uni() < keepprob){
    previousquizitem = currentquizitem;
    currentquizitem = proposedquizitem;
    currentquizitem->quiz_print(currentquizdirection,kg);
  }
  else {
    quizqueue.erase(quizqueue.begin()+currentquizindex);
    continue_quiz(kg);
  }
}

void model::grading_continue_quiz(int grade, Knomegui * kg){
  if (grade > 0){
    quizqueue.erase(quizqueue.begin()+currentquizindex);
  }
  currentquizitem->processgrade(grade,currentquizdirection);
  continue_quiz(kg);
}

void model::continue_quiz(Knomegui * kg){
  if (!quizqueue.size()){
    wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Congratulations! You are done for today."), wxT("All done!"), wxOK);
    dial->ShowModal();
    save_to_file();
    kg->ActivateToolbar();
    currentitem->displaygui(kg,false);
  }
  else {
    randomquizquestion(kg);
  } 
}

void model::hint(Knomegui * kg){
  currentquizitem->hint_print(currentquizdirection,kg);
}

void model::answer(Knomegui * kg){
  currentquizitem->answer_print(currentquizdirection,kg);
}

void model::answeritemshow(Knomegui * kg){
  currentquizitem->answer_item_print(currentquizdirection,kg);
}

void model::markitem(){
  markeditem = currentitem;
}

void model::navigate(int target_item_counter,Knomegui * kg){
  item * nextitem = currentitem->getpointervectorentry(target_item_counter);
  currentitem = nextitem;
  currentitem->displaygui(kg,false);
}

void model::edit_current_item(std::string newname){
  currentitem->setname(newname,this);
  save_to_file();
}

void model::delete_current_item(Knomegui * kg){
  currentitem->destructor(*this);
  save_to_file();
	currentitem = itemmap["bc0"];//start again at the root concept
  currentitem->displaygui(kg,false);//false because it is not the first page shown
}

void model::go_to_item(std::string targetitemname,Knomegui * kg){
	std::map<std::string,item *>::iterator it = conceptdictionary.find(targetitemname);
	if (it == conceptdictionary.end()){
    wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Concept not found!"), wxT("Error"), wxOK | wxICON_ERROR);
    dial->ShowModal();
	}
	else {
	  currentitem = it->second;
    currentitem->displaygui(kg,false);//false because it is not the first page shown
	}
}

void model::go_to_marked_item(Knomegui * kg){
  currentitem = markeditem;
  currentitem->displaygui(kg,false);//false because it is not the first page shown
}

void model::switchtobrowsing(Knomegui * kg){
  save_to_file();
  if (previousquizitem != nullptr){
    currentitem = previousquizitem;//start browsing at the previous quiz question
  }
  else {
    currentitem = currentquizitem;
  }
  currentitem->displaygui(kg,false);//false because it is not the first page shown
}

bool model::find_in_file_dictionary(std::string fname){
 std::map<std::string,item *>::iterator iter = filedictionary.find(fname);
  if (iter !=filedictionary.end()){
    return true;
  }
  else {
    return false;
  }
}

bool model::find_in_concept_dictionary(std::string cname){
  std::map<std::string,item *>::iterator iter = conceptdictionary.find(cname);
  if (iter !=conceptdictionary.end()){
    return true;
  }
  else {
    return false;
  }
}

void model::search(std::string query, Knomegui * kg){
  wxArrayString choices;//structure to save all hits

  for (auto const& x : conceptdictionary){//loop over the concept dictionary
    std::size_t found = (x.first).find(query);//x.first is the string belonging to the concept
    if (found!=std::string::npos){//if the string was found
      //wxString convertedstring(x.first);//convert to wxString
      choices.Add(wxString::FromUTF8((x.first).c_str()));
    }
  }
  if (choices.GetCount()>0){
    wxSingleChoiceDialog dialog(kg,wxT("Where would you like to go?"),wxT("Search results"),choices);
    if (dialog.ShowModal()==wxID_OK){
      std::string targetitem = mywxstringtostdstring(dialog.GetStringSelection());
      go_to_item(targetitem,kg);
    }
  }
  else {
    wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("Sorry, no matches."), wxT("Search results"), wxOK | wxICON_ERROR);
    dial->ShowModal();
  }
}

//called if the whole statement is deleted; both directions are removed from the quizqueue if present
void model::remove_quizitems(std::string statementid){
  //iterate over the quizqueue and remove all items that have the statementid in the first slot of the pair; there are at most 2 (forward and backward quiz direction)
  unsigned i = 0;
  int quizzesremoved = 0;
  while ((i < quizqueue.size()) & (quizzesremoved < 2)){
    if (quizqueue[i].first == statementid){
      quizqueue.erase(quizqueue.begin()+i);
      quizzesremoved++;
    }
    else {
      i++;
    }
  }

}

//called if a quiz direction is unticked; only one direction is removed from the quizqueue if present
void model::remove_quizitems(std::string statementid,int direction){
  unsigned i = 0;
  int quizzesremoved = 0;
  while ((i < quizqueue.size()) & (quizzesremoved < 1)){
    if ((quizqueue[i].first == statementid)&(quizqueue[i].second == direction)){
      quizqueue.erase(quizqueue.begin()+i);
      quizzesremoved++;
    }
    else {
      i++;
    }
  }

}

void model::open_image(std::string fileid){
  fileitem * filepointer = dynamic_cast<fileitem*>(itemmap[fileid]); 
  std::string filename = filepointer->get_filename();
  std::string command = "xdg-open " + filename + " 2> errorfile & ";//2 means error output, if error output is printed to the console knome freezes
  system(command.c_str());   
}