/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   monteInc/RuleCheck.h
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
#ifndef RuleCheck_h
#define RuleCheck_h

/*!
  \class RuleCheck
  \brief Simplifies rules
  \version 1.0
  \date May 2013
  \author S. Ansell
*/

class RuleCheck
{
 private:
  
  Rule* TopRule;   ///< Base rule

  static int isKey(const Rule*,const int);
  static void findKeyRule(const int,Rule*,
		   std::map<Rule*,int>&,
		   std::map<Rule*,int>&);

  static std::string checkString(const Rule*,const std::set<int>&);

 public:
  
  RuleCheck();
  RuleCheck(const RuleCheck&);
  RuleCheck& operator=(const RuleCheck&);
  ~RuleCheck() {}  ///< Destructor
  
  void setTopRule(Rule*);

  Rule* removeDuplicate
    (const std::set<int>&,const int);
  
  static const Geometry::Surface* 
    checkSurfPoint(std::set<int>&,const Rule*);



};


#endif
