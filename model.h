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
#ifndef MODEL_HH
#define MODEL_HH
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include "item.h"
#include "relation.h"
#include "quizitem.h"
#include "mytime.h"

class statement;//forward declarations, necessary to allow for circular dependency between model and statement class
class fileitem;
class Knomegui;

class model {
private:
  std::map<std::string,item *> itemmap; //lookup table: id string -> item pointer
  std::map<std::string,relation *> relationmap; //lookup table: id string -> relation pointer
  std::map<std::string,item *> conceptdictionary; //lookup table: concept name -> item pointer
  std::map<std::string,relation *> relationdictionary; //lookup table: relation name -> item pointer
  std::map<std::string,item *> filedictionary; //lookup table: file name -> item pointer
  int conceptcounter;
  int relationcounter;
  int statementcounter;
  int filecounter;
  std::fstream conceptfile;
  std::fstream relationfile;
  std::fstream statementfile;
  std::fstream filefile;
  item * markeditem; //items can be marked while browsing and used in statements later
  item * currentitem; //current item for browsing
  std::vector<std::pair<std::string, int> > quizqueue;//vector of all quizitems that are due for quiz in this session, with direction of quiz
  quizitem * currentquizitem;
  quizitem * previousquizitem;
  int currentquizdirection;
  int currentquizindex;//index of the current quiz item in the quiz queue
  unsigned recover; //whether or not the quiz is done in recovery mode

public:
  model();
  void browse(std::string startitem, Knomegui * kg);
  //functions for adding the various model components, counters are passed as arguments rather than iterated inside the functions because when reading from file, some counter values may be missing, for example due to deleted items
  void add_concept(std::string name, std::string prefix, int counter);
  void add_fileitem(std::string fname, std::string prefix, int counter);
  void add_relation(std::string name, std::string inv_name, std::string prefix, int counter);//for new relations only, information written to file
  void add_relation(std::string name, std::string prefix, int counter);
  item * add_statement(item * subpointer, relation * relpointer, item * objpointer, int quiztype, bool forwardmode, bool backwardmode, std::string prefix, int counter);
  bool add_statement(std::string subid, std::string relid, std::string objid, int quiztype, bool forwardmode, bool backwardmode, std::string prefix, int counter); //returns true/false depending on whether or not adding the statement was successful
  void add_statement(item * subpointer, relation * relpointer, item * objpointer, int quiztype, bool forwardmode, bool backwardmode, std::vector<double> EF, std::vector<mytime> RD, std::vector<double> Is, std::string prefix, int counter);
  bool add_statement(std::string subid, std::string relid, std::string objid, int quiztype, bool forwardmode, bool backwardmode, std::vector<double> EF, std::vector<mytime> RD, std::vector<double> Is, std::string prefix, int counter); //returns true/false depending on whether or not adding the statement was successful
  item * add_statement(item * subpointer, Knomegui * kg);//interactively generates a new statement with the current focal item as subject; returns a pointer to the new statement item
  relation * assign_relation(Knomegui * kg);
  item * assign_object(Knomegui * kg);
  void save_to_file();
  void remove_from_itemmap(std::string id);
  void remove_from_conceptdictionary(std::string name);
  void remove_from_filedictionary(std::string name);
  void update_conceptdictionary(std::string oldname,std::string newname);
  void update_filedictionary(std::string oldname,std::string newname);
  void start_quiz(Knomegui * kg);
  void randomquizquestion(Knomegui * kg);
  void grading_continue_quiz(int grade, Knomegui * kg);
  void continue_quiz(Knomegui * kg);
  void hint(Knomegui * kg);
  void answer(Knomegui *kg);
  void answeritemshow(Knomegui * kg);
  void markitem(); //marks the current item
  void navigate(int target_item_counter,Knomegui * kg);
  void edit_current_item(std::string newname);
  item * get_current_item() const {return currentitem;};
  item * get_current_quizitem() const {return currentquizitem;};
  void delete_current_item(Knomegui * kg);
  void go_to_item(std::string targetitemname,Knomegui * kg);
  void go_to_marked_item(Knomegui * kg);
  void switchtobrowsing(Knomegui * kg);
  bool find_in_file_dictionary(std::string fname);
  bool find_in_concept_dictionary(std::string cname);
  void search(std::string query,Knomegui * kg);
  void remove_quizitems(std::string statementid);//called if the whole statement is deleted; both directions are removed from the quizqueue if present
  void remove_quizitems(std::string statementid,int direction);//called if a quiz direction is unticked; only one direction is removed from the quizqueue if present
  void open_image(std::string fileid);
};

#endif
