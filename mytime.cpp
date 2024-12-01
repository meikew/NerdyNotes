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
    
#include "mytime.h"
#include <iostream>
#include "boost/date_time/gregorian/gregorian.hpp" 

mytime::mytime()
  : tp(boost::gregorian::day_clock::local_day())
{
}

mytime::mytime(std::stringstream& datestream){
  datestream >> tp;
}

void mytime::movedays(int numdays){
  tp += boost::gregorian::date_duration(numdays);
}

bool operator== (const mytime &t1, const mytime &t2){
  return (t1.tp == t2.tp);
}
 
bool operator!=(const mytime &t1, const mytime &t2){
  return !(t1 == t2);
}

bool operator>= (const mytime &t1, const mytime &t2){
  return (t1.tp >= t2.tp);
}

bool operator<= (const mytime &t1, const mytime &t2){
  return (t1.tp <= t2.tp);
}

int operator- (const mytime &t1, const mytime &t2){
  boost::gregorian::date_duration dd = t1.tp - t2.tp;
  return (dd.days());
}

//std::ostream& operator<< (std::ostream &out, const mytime &t){
//  out << t.tp;
//  return out;
//}

//std::stringstream& operator>> (const mytime &t,std::stringstream &inp){
//  t.tp >> inp;
//  return inp;
//}
