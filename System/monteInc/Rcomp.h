/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Rcomp.h
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
#ifndef MonteCarlo_Rcomp_h
#define MonteCarlo_Rcomp_h

namespace Geometry
{
  template<typename T> class MatrixBase;
  template<typename T> class Matrix;
}

namespace MonteCarlo 
{


/// Enumberation of intersection / union
enum JoinForm { Inter=0,Union=1 };

/*!
  \class Rcomp
  \brief Group decision tree for HeadRule/rules
  \version 1.0
  \author S. Ansell
  \date November 2015

  Holds the state of a logical unit. Either a 
  state point and intersection or a unit
  A component, either a union or intersection.
  It has to have the ability to change the type
  from union/intersection in an instant. Hence the type flag.
  This currently works with forming disjunction form.
  The disjunction form is "or" [ v symbol ]. Thus is is
  a set of unions.
  - intersection : (logical and) : ^ : e.g abcd
  - union : (logical or) : v : e.g a+b+c+d
*/
class Rcomp
{
 private:
  
  int Intersect;                ///< Union/Intersection (0,1)
  std::vector<int> Units;       ///< Surf Units in list 
  std::vector<Rcomp> Comp;      ///< Components in list

  void deleteComp();            ///< delete all of the Comp list
  
  void addComp(const Rcomp&);      ///< add a Component intellegently
  //  void addCompPtr(Rcomp*);      ///< add a Component intellegently
  void addUnitItem(const int);      ///< add an Unit intellgently


  int joinDepth();                      
  int removeEqComp();                   
  int copySimilar(const Rcomp&);        

  /// Adds a Binary state to the Component

  int makeReadOnce();                   ///< Factorize into a read once function

 public:

  Rcomp(const JoinForm);   
  Rcomp(const Rcomp&);
  Rcomp& operator=(const Rcomp&); 
  int operator==(const Rcomp&) const; 
  int operator!=(const Rcomp&) const;         ///< Complementary operator
  int operator<(const Rcomp&) const;       
  int operator>(const Rcomp&) const;
  Rcomp& operator+=(const Rcomp&);
  Rcomp& operator-=(const Rcomp&);
  Rcomp& operator*=(const Rcomp&);
  ~Rcomp();

  const Rcomp* itemC(const size_t) const;
  int itemN(const size_t) const;   

  std::pair<int,int> size() const; ///< get the size of the units and the Rcomp sub-comp
  int isSimple() const;            ///< true if only Units
  int isNull() const;              
  int isSingle() const;            
  int contains(const Rcomp&) const;  
  int isInter() const { return Intersect; }
  int isTrue(const std::map<int,int>&) const;
  
  void getLiterals(std::set<int>& literal) const;
  void getLiteralsFreq(std::map<int,int>& literalMap) const;

  void Sort();                                     
  void getLiterals(std::map<int,int>&) const; 
  std::vector<int> getKeys() const;      
  int logicalEqual(const Rcomp&) const;

  void complement();                              
  std::string display() const;                    
  std::string displayDepth(const int =0) const;   

  void orString(const std::string&);

  void writeFull(std::ostream&,const size_t) const;

};

std::ostream& operator<<(std::ostream&,const Rcomp&); 

}  // NAMESPACE MonteCarlo


#endif
