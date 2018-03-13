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

  /// Type of set for units
  typedef std::set<int,AcompTools::unitsLessOrder> UTYPE; 

  int trueFlag;                 ///< -1 : False : 0 [variable] : 1 True
  int Intersect;                ///<  Union/Intersection (0,1)
  UTYPE Units;       ///< Units in set
  std::vector<Acomp> Comp;      ///< Components in list

  void deleteComp();            ///< delete all of the Comp list
  void addComp(const Acomp&);      ///< add a Component intellegently
  //  void addCompPtr(Acomp*);      ///< add a Component intellegently
  void addUnitItem(const int);      ///< add an Unit intellgently
  void processIntersection(const std::string&);
  void processUnion(const std::string&);
  int joinDepth();                      
  int removeEqComp();
  bool removeFalseComp();
  bool removeUnionPair();
  bool removeEqUnion();                   
  int copySimilar(const Acomp&);        

  bool hasUnitInUnit(const int) const;
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

  void upMoveComp();
  void primativeAddItem(const Acomp&);
  void processChange();

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
  
 public:

  static int unitStr(std::string);
  static std::string strUnit(const int);
  static std::string mapLogic(const std::map<int,int>&);
  
  Acomp(const JoinForm);
  explicit Acomp(const int);   
  Acomp(const Acomp&);
  Acomp& operator=(const Acomp&); 
  bool operator==(const Acomp&) const; 
  bool operator!=(const Acomp&) const;  
  bool operator<(const Acomp&) const;       
  bool operator>(const Acomp&) const;
  Acomp& operator+=(const Acomp&);
  Acomp& operator-=(const Acomp&);
  Acomp& operator*=(const Acomp&);
  ~Acomp();

  Acomp& addIntersect(const int);
  Acomp& addUnion(const int);
  
  // AcompExtra
  Acomp componentExpand(const int,const Acomp&) const;
  int merge();
  bool clearNulls(); 
  void clear();     //  this delete all
  bool isEmpty() const;
  bool isFalse() const { return trueFlag==-1; }
  bool isTrue() const { return trueFlag==1; }
  
  
  int getSinglet() const;
  const Acomp* itemC(const size_t) const;

  size_t countComponents() const;
  std::pair<size_t,size_t> size() const;
  bool isSinglet() const;
  bool isSimple() const;
  bool isDNF() const;               
  bool isCNF() const;               
  bool isNull() const;              
  bool isSingle() const;        // one item

  void resolveTrue(const int);
  void removeNonCandidate(const int);
  bool removeLiteral(const int);
  bool removeSignedLiteral(const int);
  
  int contains(const int) const;
  int contains(const Acomp&) const;
  /// Deterimine if inter/union
  int isInter() const { return Intersect; }  
  int isTrue(const std::map<int,int>&) const;   
  
  void Sort();

  void getLiterals(std::set<int>&) const;     
  void getAbsLiterals(std::set<int>&) const;
  bool hasLiteral(const int) const;
  bool hasAbsLiteral(const int) const;
  
  std::vector<int> getKeys() const;      
  bool logicalEqual(const Acomp&) const;
  int logicalCover(const Acomp&) const;
  int logicalIntersectCover(const Acomp&) const;
  
  int makeDNFobject();                
  int makeCNFobject();                

  void expandBracket();
  void expandCNFBracket();

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
