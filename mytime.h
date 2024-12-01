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

#ifndef MYTIME_HH
#define MYTIME_HH
#include "boost/date_time/gregorian/gregorian.hpp" 

class mytime {
public:
  mytime();
  mytime(std::stringstream& datestream);//for some reason, converting to string directly requires linking the date_time library, which I had problems with. Interaction with stringstream works with headers only.
  void movedays(int numdays);
  void write_date(std::stringstream& datestream) const {datestream << tp;};
  friend bool operator== (const mytime &t1, const mytime &t2);
  friend bool operator!=(const mytime &t1, const mytime &t2);
  friend bool operator>= (const mytime &t1, const mytime &t2);
  friend bool operator<= (const mytime &t1, const mytime &t2);
  friend int operator- (const mytime &t1, const mytime &t2);
  //friend std::ostream& operator<< (std::ostream &out, const mytime &t);
  //friend std::stringstream& operator>> (const mytime &t,std::stringstream &in);
private:
  boost::gregorian::date tp;
};



#endif
