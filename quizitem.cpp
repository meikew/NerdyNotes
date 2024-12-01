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
#include "quizitem.h"
#include "item.h"
#include "model.h"
#include <boost/random.hpp>

extern boost::mt19937 rng;

quizitem::quizitem(std::string idstring, int qtype)
  : item(idstring), quiztype(qtype)
{
  if (quiztype>0){
    Efactors.push_back(2.5);
    mytime tomorrow;//default constructor initializes to today
    tomorrow.movedays(1);
    nextreviewdates.push_back(tomorrow);
    intervals.push_back(1);
 
    Efactors.push_back(2.5);
    nextreviewdates.push_back(tomorrow);
    intervals.push_back(1);
 
  }
}

quizitem::quizitem(std::string idstring, int qtype, std::vector<double> Ef, std::vector<mytime> nrd, std::vector<double> is)
  : item(idstring), quiztype(qtype), Efactors(Ef), nextreviewdates(nrd), intervals(is)
{
}


void quizitem::processgrade(int response,int direction){
  mytime today;
  nextreviewdates[direction]=today;
  if (response){
    response = std::min(response, 3);
    response +=2; //to bring it to the SuperMemo scale
    Efactors[direction] += 0.28 * response - 0.02 * pow(response,2) -0.8;
    Efactors[direction] = std::max(Efactors[direction],1.3);
    intervals[direction]*=Efactors[direction];
    intervals[direction]=std::max(intervals[direction],double(1));
    //slightly perturb the next review date to prevent synchronisation of cohorts of cards
    boost::normal_distribution<> normdist(0,0.1*intervals[direction]);
    boost::variate_generator<boost::mt19937&, boost::normal_distribution<> > perturbation(rng, normdist);   
    nextreviewdates[direction].movedays(rint(intervals[direction]+perturbation()));//interval rounded to the next integer
  }
  else {
    Efactors[direction]=std::max(Efactors[direction],2.5);
    intervals[direction]=1;
  }
}

void quizitem::filewrite(std::fstream& outfile){
  outfile  << quiztype;
  if (quiztype){
    std::stringstream d1, d2;
    std::string d1s, d2s;
    nextreviewdates[0].write_date(d1);
    nextreviewdates[1].write_date(d2);
    d1 >> d1s;
    d2 >> d2s;
    outfile << '\t' << Efactors[0] << '\t' << Efactors[1] << '\t' << d1s << '\t' << d2s << '\t' << intervals[0] << '\t' << intervals[1];
  }
}

void quizitem::initialize_metrics(){
  Efactors.push_back(2.5);
  mytime tomorrow;//default constructor initializes to today
  tomorrow.movedays(1);
  nextreviewdates.push_back(tomorrow);
  intervals.push_back(1);
 
  Efactors.push_back(2.5);
  nextreviewdates.push_back(tomorrow);
  intervals.push_back(1);
}
