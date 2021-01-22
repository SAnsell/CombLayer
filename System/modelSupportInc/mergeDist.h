/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/mergeDist.h
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
#ifndef ModelSupport_mergeDist_h
#define ModelSupport_mergeDist_h

namespace ModelSupport
{

/*!
  \class mergeDist
  \version 1.0
  \author S. Ansell
  \date June 2011
  \brief Fractionally splits several semi-parallel surface
*/

class mergeDist : public surfDBase
{
 private:

  ModelSupport::surfRegister SMap; ///< Surface register
  
  Rule* InRule;               ///< Inner Rule 
  Rule* OutRule;              ///< Outer Rule

  std::vector<int> sSurf;                 ///< Surface numbers [signed]
  std::vector<double> expandValue;        ///< Expand values [signed]
  std::vector<const Geometry::Surface*> bSurf;  ///< base Surf

  std::vector<Token> innerToken;            ///< Inner Object
  std::vector<Token> outerToken;            ///< Outer Object

  void clearRules();
  void addRules();

 public:
  
  mergeDist();
  mergeDist(const mergeDist&);
  mergeDist& operator=(const mergeDist&);
  virtual mergeDist* clone() const;
  virtual ~mergeDist();

  // SETTING METHODS
  void addSurf(const int,const double);

  virtual void populate();
  virtual int createSurf(const double,int&);
  virtual void processInnerOuter(const int,std::vector<Token>&) const;

};

}

#endif
