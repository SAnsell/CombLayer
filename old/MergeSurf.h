/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   old/MergeSurf.h
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
#ifndef ModelSupport_MergeSurf_h
#define ModelSupport_MergeSurf_h

class Rule;
class Token;

namespace ModelSupport
{

/*!
  \class MergeSurf
  \version 2.0
  \author S. Ansell
  \date February 2010
  \brief Generates a surface based on a 0 to 1 number.
*/
						
class MergeSurf
{
 protected:

  double frac;                ///< Fraction weight

  Rule* InRule;               ///< Inner Rule 
  Rule* OutRule;              ///< Outer Rule
  
  void clearRules(); 

public:
  
  MergeSurf();
  MergeSurf(const MergeSurf&);
  MergeSurf& operator=(const MergeSurf&);
  virtual ~MergeSurf();  

  ///\cond ABSTRACT
  virtual MergeSurf* clone() const =0;

  virtual int createSurf(int&,const double) =0;  
  virtual std::vector<int> getInnerRemove() const =0; 
  virtual std::vector<int> getOuterRemove() const =0;
  ///\endcond ABSTRACT
  
  /// Access points
  std::string getInner() const;
  std::string getOuter() const;

  void getInner(std::vector<Token>&) const;
  void getOuter(std::vector<Token>&) const;

};


/*!
  \class mergeMulti
  \version 1.0
  \author S. Ansell
  \date June 2011
  \brief Fractionally splits several semi-parallel surface
*/
template<typename T,typename U>
class mergeMulti : public MergeSurf
{
 private:
  
  double frac;                ///< Fraction weight

  Rule* InRule;               ///< Inner Rule 
  Rule* OutRule;              ///< Outer Rule
  
  void clearRules(); 

  const T* PB;                  ///< Base surface
  std::vector<int> secDir;      ///< Second direction
  std::vector<const U*> PS;      ///< Second planes

  std::vector<Geometry::Surface*> OSPtr;   ///< Output surface 

  void addRules();

 public:
  
  mergeMulti();
  mergeMulti(const int,const Geometry::Surface*,
	    const std::vector<const Geometry::Surface*>&);
  mergeMulti(const mergeMulti<T,U>&);
  mergeMulti<T,U>& operator=(const mergeMulti<T,U>&);
  virtual mergeMulti<T,U>* clone() const;
  virtual ~mergeMulti() {}      ///< Destructor
    
  virtual void initSurfaces(const Geometry::Surface*,
			    const std::vector<const Geometry::Surface*>&);
  virtual void setDirection(const std::vector<int>&);
  /// Specialized by template:
  virtual int createSurf(int&,const double);
  virtual std::vector<int> getInnerRemove() const; 
  virtual std::vector<int> getOuterRemove() const;

};

/*!
  \class mergeAdd
  \version 1.0
  \author S. Ansell
  \date July 2011
  \brief Adds a surface group to the system at various points
*/
class mergeAdd : public MergeSurf
{
 private:
  
  int position;            ///< Inner/outer/all   [1:2:3]

  void addRules();

 public:
  
  mergeAdd();
  mergeAdd(const int,const int,const Geometry::Surface*);
  mergeAdd(const mergeAdd&);
  mergeAdd& operator=(const mergeAdd&);
  virtual mergeAdd* clone() const;
  virtual ~mergeAdd() {}      ///< Destructor
    

  /// Specialized by template:
  virtual int createSurf(int&,const double);

  virtual std::vector<int> getInnerRemove() const; 
  virtual std::vector<int> getOuterRemove() const;

};


}

#endif
