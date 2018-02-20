/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Acomp.h
 *
 * Copyright (c) 2004-2018 by Stuart Ansell
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
#ifndef Acomp_h
#define Acomp_h

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
  \class Acomp
  \brief Holds a state point in the descision tree. 
  \version 1.0
  \author S. Ansell
  \date April 2005

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
class Acomp
{
 private:
  
  int Intersect;                ///<  Union/Intersection (0,1)
  std::vector<int> Units;       ///< Units in list
  std::vector<Acomp> Comp;      ///< Components in list

  void deleteComp();            ///< delete all of the Comp list
  void addComp(const Acomp&);      ///< add a Component intellegently
  //  void addCompPtr(Acomp*);      ///< add a Component intellegently
  void addUnitItem(const int);      ///< add an Unit intellgently
  void processIntersection(const std::string&);
  void processUnion(const std::string&);
  int joinDepth();                      
  int removeEqComp();                   
  int copySimilar(const Acomp&);        

  /// Adds a Binary state to the Component
  void addUnit(const std::vector<int>&,const BnId&);   
  void assignDNF(const std::vector<int>&,const std::vector<BnId>&);  
  int getDNFobject(std::vector<int>&,std::vector<BnId>&) const;  
  int getDNFpart(std::vector<Acomp>&) const;                   

  int getCNFobject(std::vector<int>&,std::vector<BnId>&) const;  
  void assignCNF(const std::vector<int>&,const std::vector<BnId>&);   ///< Assigns the Comp with the DNF
  //  int getCNFpart(std::vector<Acomp>&) const;                   ///< get the CNF parts (as Acomp)

  /// Calculate Principle Components
  int makePI(std::vector<BnId>&) const;                        
  int makeEPI(std::vector<BnId>&,std::vector<BnId>&) const;    

  int makeReadOnce();                   ///< Factorize into a read once function

  static void removeOuterBracket(std::string&);
  static int hasPlus(const std::string&);
  static size_t checkWrongChar(const std::string&);
  void addTokens(const std::string&);
  
 public:

  Acomp(const JoinForm);
  explicit Acomp(const int);   
  Acomp(const Acomp&);
  Acomp& operator=(const Acomp&); 
  int operator==(const Acomp&) const; 
  int operator!=(const Acomp&) const;  
  int operator<(const Acomp&) const;       
  int operator>(const Acomp&) const;
  Acomp& operator+=(const Acomp&);
  Acomp& operator-=(const Acomp&);
  Acomp& operator*=(const Acomp&);
  ~Acomp();

  // AcompExtra
  void upMoveComp();
  int merge();
  Acomp componentExpand(const int,const Acomp&) const;
  void primativeAddItem(const Acomp&);

  Acomp expandIII(const Acomp&) const;
  Acomp expandIIU(const Acomp&) const;
  Acomp expandUII(const Acomp&) const;
  Acomp expandUIU(const Acomp&) const;

  Acomp expandIUI(const Acomp&) const;
  Acomp expandIUU(const Acomp&) const;
  Acomp expandUUI(const Acomp&) const;
  Acomp expandUUU(const Acomp&) const;
  
  static Acomp interCombine(const int,const int);
  static Acomp unionCombine(const int,const int);
  static Acomp interCombine(const int,const Acomp&);
  static Acomp unionCombine(const int,const Acomp&);
  static Acomp interCombine(const Acomp&,const Acomp&);
  static Acomp unionCombine(const Acomp&,const Acomp&);

  
  int getSinglet() const;   
  const Acomp* itemC(const size_t) const;
  int itemN(const size_t) const;   

  std::pair<int,int> size() const;
  int isSimple() const;
  int isDNF() const;               
  int isCNF() const;               
  int isNull() const;              
  int isSingle() const;        // one item

  int contains(const Acomp&) const;
  /// Deterimine if inter/union
  int isInter() const { return Intersect; }  
  int isTrue(const std::map<int,int>&) const;   

  void Sort();                                    
  void getLiterals(std::map<int,int>&) const;     
  void getAbsLiterals(std::map<int,int>&) const;  
  std::vector<int> getKeys() const;      
  int logicalEqual(const Acomp&) const;

  int makeDNFobject();                
  int makeCNFobject();                

  void expandBracket();                                
  void complement();
  std::pair<Acomp,Acomp> algDiv(const Acomp&); 
  void setString(const std::string&);          
  void writeFull(std::ostream&,const int =0) const; 
  std::string unitsDisplay() const;
  std::string compDisplay(std::string = "") const;
  std::string display() const;                      
  std::string displayDepth(const int =0) const;     

  void orString(const std::string&);

  void printImplicates(const std::vector<BnId>&,
		       const Geometry::MatrixBase<int>&) const;
};

std::ostream& operator<<(std::ostream&,const Acomp&); 
// int operator<(const Acomp&,const Acomp&);
//int more_than(const Acomp&,const Acomp&);

}  // NAMESPACE MonteCarlo


#endif
