/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/mergeTemplate.h
 *
 * Copyright (c) 2004-2015 by Stuart Ansell
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
#ifndef ModelSupport_mergeTemplate_h
#define ModelSupport_mergeTemplate_h

namespace ModelSupport
{

/*!
  \class mergeTemplate
  \version 1.0
  \author S. Ansell
  \date June 2014
  \brief Fractionally splits several semi-parallel surface
  with full key templating for output control
*/

template<typename T,typename U>
class mergeTemplate : public surfDBase
{
 private:

  int surfN;                     ///< Surface number

  HeadRule InTemplate;           ///< Inner template
  HeadRule OutTemplate;          ///< Outer template

  std::vector<int> pSurf;        ///< Primary surfaces 
  std::vector<int> sSurf;        ///< Secondary surfaces [signed]

  std::vector<const T*> primSPtr;       ///< Base surface
  std::vector<const U*> secSPtr;        ///< Second planes
  
  // Created as managed [No del required]
  std::vector<Geometry::Surface*> PSPtr;   ///< Previous surfaces 
  std::vector<Geometry::Surface*> OSPtr;   ///< Output surfaces 

  void clearRules();
  void addRules();

  HeadRule makeOuter() const;
  HeadRule makeOuterComp() const;
  
  bool containInnerRules(const Rule*) const;
  bool containOuterRules(const Rule*) const;


 public:
  
  mergeTemplate();
  mergeTemplate(const mergeTemplate<T,U>&);
  mergeTemplate<T,U>& operator=(const mergeTemplate<T,U>&);
  virtual mergeTemplate<T,U>* clone() const;
  virtual ~mergeTemplate();

  /// set surface number
  virtual void setOutSurfNumber(const int I) { surfN=I; }
  // SETTING METHODS
  void setSurfPair(const int,const int);
  void setInnerRule(const std::string&);
  void setOuterRule(const std::string&);

  void setDirection(const std::vector<int>&);

  virtual void populate();
  int createSurf(const double,int&) { return 0; };
  int createSurfaces(const double);
  virtual void processInnerOuter(const int,std::vector<Token>&) const;
  virtual void process(const double,const double,HeadRule&);

  virtual void write(std::ostream&) const;

};

}

#endif
