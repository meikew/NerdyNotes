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

#ifndef QUIZITEM_HH
#define QUIZITEM_HH
#include "item.h"
#include <string>
#include "mytime.h"

class model;//forward decleration, necessary to allow for circular dependency between model and statement class

class quizitem : public item {
public:
  quizitem(std::string idstring, int qtype);//constructor for new quiz items
  quizitem(std::string idstring, int qtype, std::vector<double> Ef, std::vector<mytime> nrd, std::vector<double> is);//construtor for old quiz items reconstructed from file
  void processgrade(int response,int direction);
  virtual void quiz_print(int direction,Knomegui * kg) = 0;
  virtual void hint_print(int direction,Knomegui * kg) = 0;
  virtual void answer_print(int direction,Knomegui * kg) = 0;
  virtual void answer_item_print(int direction,Knomegui * kg) = 0;
  virtual void filewrite(std::fstream& outfile);
  mytime get_reviewdates(int direction) const {return nextreviewdates[direction];};
  virtual std::vector<std::string> get_text() = 0;
  void initialize_metrics();
protected:
  int quiztype;//0: no quiz, 1: forward and backward, 2: only forward, 3: only backward, 4: yes/no
  std::vector<double> Efactors; //this vector and the following vectors have one item for each question, e.g. one for the forward direction and one for the backward direction
  std::vector<mytime> nextreviewdates;
  std::vector<double> intervals;
};

#endif
