/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   processInc/mergeMulti.h
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
#ifndef ModelSupport_mergeMulti_h
#define ModelSupport_mergeMulti_h

namespace ModelSupport
{

/*!
  \class mergeMulti
  \version 1.0
  \author S. Ansell
  \date June 2011
  \brief Fractionally splits several semi-parallel surface
*/

template<typename T,typename U>
class mergeMulti : public surfDBase
{
 private:

  int InOutFlag;              ///< Swap sense of systems
  int signSurfReplace;        ///< Replace surface as a signed surface
  
  Rule* InRule;               ///< Inner Rule 
  Rule* OutRule;              ///< Outer Rule

  int pSurf;                     ///< Primary Surf [signed]
  std::vector<int> sSurf;        ///< Secondary surfaces [signed]
  std::vector<int> secDir;       ///< Second direction 

  const T* PB;                  ///< Base surface
  std::vector<const U*> PS;     ///< Second planes
  
  // Created as managed [No del required]
  std::vector<Geometry::Surface*> OSPtr;   ///< Output surface 

  void clearRules();
  void addRules();

  void getInner(std::vector<Token>&) const;
  void getOuter(std::vector<Token>&) const;
  std::vector<int> getInnerRemove() const; 
  std::vector<int> getOuterRemove() const;

  void processInnerOuterWithSign(const int,std::vector<Token>&) const;

 public:
  
  mergeMulti();
  mergeMulti(const mergeMulti<T,U>&);
  mergeMulti<T,U>& operator=(const mergeMulti<T,U>&);
  virtual mergeMulti<T,U>* clone() const;
  virtual ~mergeMulti();

  // SETTING METHODS
  void setPrimarySurf(const int,const int);
  void addSecondarySurf(const int);

  void setDirection(const std::vector<int>&);
  /// Set sign replacement 
  void setSignReplace(const int F) { signSurfReplace=F; }

  virtual void populate();
  virtual int createSurf(const double,int&);
  virtual void processInnerOuter(const int,std::vector<Token>&) const;

};

}

#endif
