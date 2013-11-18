/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   include/KGroup.h
*
 * Copyright (c) 2004-2013 by Stuart Ansell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 *
 ****************************************************************************/
#ifndef KGroup_h
#define KGroup_h


/*!
  \class KGroup
  \brief Key + a range of values
  \author S. Ansell
  \date April 2008
  \version 1.0

*/

class KGroup
{
 private:
  
  std::string KeyItem;          ///< Initial key item
  NRange Values;                ///< Range sequence

 public:

  KGroup();
  KGroup(const std::string&);         ///< Initialise with a string
  KGroup(const KGroup&);  
  KGroup& operator=(const KGroup&);
  ~KGroup();

  int empty() const { return Values.empty(); }  ///< Item exists
  size_t size() const { return Values.size(); }    ///< Size of list
  void clear() { KeyItem="";Values.clear(); }   ///< Clear all

  int count() const; 

  int addComp(const std::vector<double>&);
  int addComp(const double);
  int addComp(const std::string&);

  int processString(const std::string&);      
  std::vector<double> actualItems() const;     

  void write(std::ostream&) const;            
  void writeMCNPX(std::ostream&) const;            

};

std::ostream&
operator<<(std::ostream&,const KGroup&);

#endif
