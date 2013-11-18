/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/surfDRule.h
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
#ifndef ModelSupport_surfDRule_h
#define ModelSupport_surfDRule_h

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

  int signSurfReplace;        ///< Replace surface as a signed surface
  
  Rule* InRule;               ///< Inner Rule 
  Rule* OutRule;              ///< Outer Rule

  int pSurf;                     ///< Primary Surf [signed]
  std::vector<int> sSurf;        ///< Secondary surfaces [signed]
  std::vector<int> secDir;       ///< Second direction 

  const T* PB;                  ///< Base surface
  std::vector<const U*> PS;     ///< Second planes
  
  // Created an managed
  std::vector<Geometry::Surface*> OSPtr;   ///< Output surface 

  void clearRules(); 
  void addRules();

  void processInnerOuterWithSign(const int,std::vector<Token>&) const;

 public:
  
  mergeMulti();
  mergeMulti(const int,const Geometry::Surface*,
	    const std::vector<const Geometry::Surface*>&);
  mergeMulti(const mergeMulti<T,U>&);
  mergeMulti<T,U>& operator=(const mergeMulti<T,U>&);
  virtual mergeMulti<T,U>* clone() const;
  virtual ~mergeMulti() {}      ///< Destructor

  // SETTING METHODS
  void setPrimarySurf(const int,const int);
  void addSecondarySurf(const int);

  // populate all the surface sets
  virtual void initSurfaces(const Geometry::Surface*,
			    const std::vector<const Geometry::Surface*>&);
  virtual void setDirection(const std::vector<int>&);
  /// Specialized by template:
  virtual int createSurf(int&,const double);
  virtual std::vector<int> getInnerRemove() const; 
  virtual std::vector<int> getOuterRemove() const;

  ///\cond ABSTRACT
  void populate() =0;
  void createDividedSurf(const int,const double,int&) =0;
  void processInnerOuter(const int,std::vector<Token>&) const =0;
  ///\endcond ABSTRACT


};



 
 public:  

  surfDRule();
  surfDRule(const surfDRule&);
  surfDRule& operator=(const surfDRule&);  
  ~surfDRule();
  
  void setMergeRule(MergeSurf*);

  // GETTERS FOR POPULATION
  int getInner() const { return pSurf; }  ///< Get Inner
  /// Get Outer
  const std::vector<int>& getOuter() const   
    { return sSurf; }

  // ACTION METHODS
  void createDividedSurf(const double,int&);

  void addToken(std::vector<Token>&,std::vector<Token>&) const;
  void addString(std::string&,std::string&) const;


  void processInnerOuter(const int,std::vector<Token>&) const;

  /// Set signed surface
  void setSignReplace(const int F) { signSurfReplace=F; } 
  void write(std::ostream&) const;
};

std::ostream&
operator<<(std::ostream&,const surfDRule&);

}

#endif
