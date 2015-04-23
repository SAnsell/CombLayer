/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/mergeRule.h
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
#ifndef ModelSupport_mergeRule_h
#define ModelSupport_mergeRule_h

namespace ModelSupport
{
  class pairBase;

/*!
  \class mergeRule
  \version 1.0
  \author S. Ansell
  \date June 2011
  \brief Fractionally splits several semi-parallel surface
  \param T :: 
*/


class mergeRule : public surfDBase
{
 private:
  
  std::string innerString;             ///< Inner string
  std::string outerString;             ///< Outer string

  std::string innerReplace;            ///< Inner string
  std::string outerReplace;            ///< Outer string
    
  std::vector<pairBase*> surfItems;    ///< List of surface rules

  std::vector<Token> TVec;             ///< Token vector
  std::vector<size_t> iIndex;          ///< Inner index(s)
  std::vector<size_t> oIndex;          ///< Outer index(s)

  std::vector<size_t> locateComp(const std::vector<Token>&,
				 const std::vector<Token>&) const;
				 

 public:
  
  mergeRule();
  mergeRule(const mergeRule&);
  mergeRule& operator=(const mergeRule&);
  virtual mergeRule* clone() const;
  virtual ~mergeRule();

  // Sets input/output string
  void setInner(const std::string&);
  void setOuter(const std::string&);
  void setInnerReplace(const std::string&);
  void setOuterReplace(const std::string&);

  // adds rules:
  void addRule(const pairBase*);
  bool locateComponents(const std::vector<Token>&);



  virtual void populate();
  virtual int createSurf(const double,int&);
  virtual void processInnerOuter(const int,std::vector<Token>&) const;

};

}

#endif
