/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Rules.h
 *
 * Copyright (c) 2004-2021 by Stuart Ansell
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
#ifndef Rules_h
#define Rules_h

class Token;

namespace Geometry
{
  class Surface;
}
namespace MonteCarlo
{
  class Object;
}

class CompGrp;
/*!
  \class Rule
  \brief Object generation rule tree
  \author S.Ansell
  \version 1.0
  \date April 2005
 
  Base class for a rule item in the tree. 
*/

class Rule
{
 private:

  Rule* Parent;                    ///< Parent object (for tree)

  static size_t addToKey(std::vector<int>&,const long int =-1);     

  int getBaseKeys(std::vector<int>&) const;  ///< Fills the vector with the surfaces 

  static CompGrp* procComp(Rule*);

  const Geometry::Surface*
    checkSurfPoint(std::set<int>&,const Rule*) const;

 public:

  static int makeCNFcopy(Rule*&);  ///< Make Rule into a CNF format (slow)
  static int makeFullDNF(Rule*&);  ///< Make Rule into a full DNF format
  static int makeCNF(Rule*&);      ///< Make Rule into a CNF format
  static int removeComplementary(Rule*&);       ///< NOT WORKING
  static int removeItem(Rule*&,const int);
  static Rule* procString(const std::string&);
  static Rule* procCompString(const std::string&);
  static int procPair(std::string&,std::map<int,Rule*>&,
		      int&);

  Rule();
  Rule(Rule*);
  Rule(const Rule&);  
  Rule& operator=(const Rule&);
  virtual ~Rule();
  virtual Rule* clone() const=0;  ///< abstract clone object
  bool operator==(const Rule&) const;

  /// No leaf for a base rule
  virtual Rule* leaf(const int =0) const { return 0; }  
  void setParent(Rule*);
  Rule* getParent() const;
  void makeParents();
  int checkParents() const;  ///< Debug test for parents
  size_t getKeyList(std::vector<int>&) const;
  int commonType() const;      ///< Gets a common type

  std::set<const Geometry::Surface*> 
    getOppositeSurfaces(const Rule*) const;

  virtual void setLeaves(Rule*,Rule*)=0;          ///< abstract set leaves
  virtual void setLeaf(Rule*,const int =0) =0;    ///< Abstract set 
  virtual int findLeaf(const Rule*) const =0;     ///< Abstract find
  virtual Rule* findKey(const int)  =0;           ///< Abstract key find    
  virtual int type() const { return 0; }          ///< Null rule    
  
  /// Abstract: Does the object have surfaces
  virtual bool isEmpty() const =0;
  
  // using isValid by virtual
  bool isAnyValid(const Geometry::Vec3D&,
			  const std::set<int>&) const;    
  
  virtual bool isValid(const Geometry::Vec3D&,const int) const =0;           
  /// Abstract: The point is within the object
  virtual bool isValid(const Geometry::Vec3D&) const =0;           
  /// Abstract Validity based on surface true/false map
  virtual bool isValid(const std::map<int,int>&) const =0;
  virtual bool isValid(const Geometry::Vec3D&,
		       const std::map<int,int>&) const =0;    
  
  
  /// Abstract: Can the rule be simplified 
  virtual int simplify() =0;
  /// Not complementary
  virtual int isComplementary() const { return 0; }

  void removeLeaf(const int);
  int Eliminate();   

  int substituteSurf(const int,const int,const Geometry::Surface*);  
  void populateSurf();
  
  std::set<int> getSurfSet() const;
  std::vector<Geometry::Surface*> getSurfVector() const;
  std::vector<const Geometry::Surface*> getConstSurfVector() const;

  ///\cond ABSTRACT
  virtual std::string display() const=0;
  virtual std::string display(const Geometry::Vec3D&) const=0;
  virtual std::string displayAddress() const=0;
  virtual void displayVec(std::vector<Token>&) const =0;
  virtual std::string displayFluka() const =0;
  virtual std::string displayPOVRay() const =0;
  ///\endcond ABSTRACT
};

  /*!
    \class Intersection
    \brief Combines two Rule objects in an intersection
    \author S. Ansell
    \version 1.0
    \date August 2005

    Intersection is defined as a valid Rule A and
    valid rule B 
  */

class Intersection : public Rule
{

 private:
   
  Rule* A;    ///< Rule 1 
  Rule* B;    ///< Rule 2 
  
 public:

  Intersection();
  explicit Intersection(Rule*,Rule*);
  explicit Intersection(Rule*,Rule*,Rule*);
  Intersection* clone() const override;    ///< Makes a copy of the whole downward tree

  Intersection(const Intersection&);
  Intersection& operator=(const Intersection&);
  ~Intersection() override;

  /// selects leaf component
  Rule* leaf(const int ipt) const override { return ipt ? B : A; }   
  void setLeaves(Rule*,Rule*) override;           ///< set leaves
  void setLeaf(Rule*,const int =0) override;    ///< set one leaf.
  int findLeaf(const Rule*) const override;
  Rule* findKey(const int) override;
  int isComplementary() const override;

  int type() const override { return 1; }   /// effective name

  bool isEmpty() const override;
  
  bool isValid(const Geometry::Vec3D&) const override;
  bool isValid(const Geometry::Vec3D&,const int) const override;
  bool isValid(const std::map<int,int>&) const override;
  bool isValid(const Geometry::Vec3D&,
		       const std::map<int,int>&) const override;    

  int simplify() override;      ///< apply general intersection simplification

  std::string display() const override;
  std::string display(const Geometry::Vec3D&) const override;
  std::string displayAddress() const override;
  void displayVec(std::vector<Token>&) const override;
  std::string displayFluka() const override;
  std::string displayPOVRay() const override;  
};


/*!
  \class Union
  \brief Combines two Rule objects in an union
  \author S. Ansell
  \version 1.0
  \date August 2005
  
  Union is defined as a valid Rule A or
  valid rule B 
*/

class Union : public Rule
{

 private:
  
  Rule* A;    ///< Leaf rule A
  Rule* B;    ///< Leaf rule B
  
 public:

  Union();
  explicit Union(Rule*,Rule*);
  explicit Union(Rule*,Rule*,Rule*);
  Union(const Union&);

  Union* clone() const override;          
  Union& operator=(const Union&);
  ~Union() override;

  Rule* leaf(const int ipt=0) const override { return ipt ? B : A; }      ///< Select a leaf component
  void setLeaves(Rule*,Rule*) override;           ///< set leaves
  void setLeaf(Rule*,const int =0) override;     
  int findLeaf(const Rule*) const override;
  Rule* findKey(const int) override;

  int isComplementary() const override;
  int type() const override { return -1; }   ///< effective name

  bool isEmpty() const override;

  bool isValid(const Geometry::Vec3D&) const override;
  bool isValid(const Geometry::Vec3D&,const int) const override;
  bool isValid(const std::map<int,int>&) const override;
  bool isValid(const Geometry::Vec3D&,
		       const std::map<int,int>&) const override;    

  std::string display() const override;
  std::string display(const Geometry::Vec3D&) const override;
  std::string displayAddress() const override;
  void displayVec(std::vector<Token>&) const override;
  std::string displayFluka() const override;
  std::string displayPOVRay() const override;
    
  int simplify() override;      ///< apply general intersection simplification

};

/*!
  \class SurfPoint
  \brief Surface leaf node
  \author S.Ansell
  \version 1.0 
  \date August 2005

  This class acts as an interface between a general
  surface and a rule. Allowing an Rule chain to
  be calculated
*/

class SurfPoint : public Rule
{
 private:

  const Geometry::Surface* key;     ///< Actual Surface Base Object
  int keyN;                         ///< Key Number (identifer)
  int sign;                         ///< +/- in Object unit
  
 public:
  
  SurfPoint();
  explicit SurfPoint(const Geometry::Surface*,const int);
  SurfPoint(const SurfPoint&);
  SurfPoint* clone() const override;                        
  SurfPoint& operator=(const SurfPoint&);
  ~SurfPoint() override;

  Rule* leaf(const int) const override { return 0; }   ///< No Leaves
  void setLeaves(Rule*,Rule*) override;
  void setLeaf(Rule*,const int =0) override;
  int findLeaf(const Rule*) const override;
  Rule* findKey(const int) override;

  int type() const override { return 0; }   ///< Effective name

  void setKeyN(const int);             ///< set keyNumber
  void setKey(const Geometry::Surface*);

  /// alway not empty
  bool isEmpty() const override { return 0; }

  bool isValid(const Geometry::Vec3D&) const override;
  bool isValid(const Geometry::Vec3D&,const int) const override;
  bool isValid(const std::map<int,int>&) const override;
  bool isValid(const Geometry::Vec3D&,
		       const std::map<int,int>&) const override;    

  int getSign() const { return sign; }           ///< Get Sign
  int getKeyN() const { return keyN; }           ///< Get Key
  int getSignKeyN() const { return sign*keyN; }  ///< Get Signed Key
  int simplify() override;

  /// Get Surface Ptr
  const Geometry::Surface* getKey() const 
    { return key; }
  std::string display() const override;
  std::string display(const Geometry::Vec3D&) const override;
  std::string displayAddress() const override;  
  void displayVec(std::vector<Token>&) const override;

  std::string displayFluka() const override;
  std::string displayPOVRay() const override;
};

/*!
  \class CompObj
  \brief Compemented Object
  \author S.Ansell
  \version 1.0 
  \date January 2006

  This class holds a complement object 
  of a single object group.
  Care must be taken to avoid a cyclic loop
*/

class CompObj : public Rule
{
 private:

  int objN;                   ///< Object number
  MonteCarlo::Object* key;                ///< Object Pointer
  
 public:
  
  CompObj();
  CompObj(const CompObj&);
  CompObj* clone() const override;                        
  CompObj& operator=(const CompObj&);
  ~CompObj() override;

  void setLeaves(Rule*,Rule*) override;
  void setLeaf(Rule*,const int =0) override;
  int findLeaf(const Rule*) const override;
  Rule* findKey(const int) override;

  /// Is it a branched object
  int type() const override { return 0; }   
  /// Complement is true
  int isComplementary() const override { return 1; }

  void setObjN(const int);             ///< set object Number
  void setObj(MonteCarlo::Object*);               ///< Set a Object state


  bool isEmpty() const override;

  bool isValid(const Geometry::Vec3D&) const override;
  bool isValid(const Geometry::Vec3D&,const int) const override;
  bool isValid(const std::map<int,int>&) const override;
  bool isValid(const Geometry::Vec3D&,
		       const std::map<int,int>&) const override;    

  /// Get object number of component
  int getObjN() const { return objN; } 
  int simplify() override;

  MonteCarlo::Object* getObj() const { return key; }   ///< Get Object Ptr
  std::string display() const override;      
  std::string display(const Geometry::Vec3D&) const override;
  std::string displayAddress() const override;  
  void displayVec(std::vector<Token>&) const override;
  std::string displayFluka() const override;
  std::string displayPOVRay() const override;
};

/*!
  \class CompGrp
  \brief Compemented Grup 
  \author S.Ansell
  \version 1.0 
  \date January 2006

  This class holds a complement object 
  of a single object group.
  Care must be taken to avoid a cyclic loop
*/

class CompGrp : public Rule
{
 private:

  Rule* A;     ///< Rule to take complement of 
  
 public:
  
  CompGrp();
  explicit CompGrp(Rule*,Rule*);  
  CompGrp(const CompGrp&);
  CompGrp* clone() const override;                        
  CompGrp& operator=(const CompGrp&);
  ~CompGrp() override;

  Rule* leaf(const int) const override { return A; }   ///< selects leaf component
  void setLeaves(Rule*,Rule*) override;
  void setLeaf(Rule*,const int =0) override;
  int findLeaf(const Rule*) const override;
  Rule* findKey(const int) override;

  /// Is it a branched object
  int type() const override { return 0; }       
  /// Complementary status is true
  int isComplementary() const override { return 1; }  


  /// is empty
  bool isEmpty() const override { return (A && !A->isEmpty()) ? 0 : 1; }

  bool isValid(const Geometry::Vec3D&) const override;
  bool isValid(const Geometry::Vec3D&,const int) const override;
  bool isValid(const std::map<int,int>&) const override;
  bool isValid(const Geometry::Vec3D&,
		       const std::map<int,int>&) const override;    

  int simplify() override;

  std::string display() const override;      
  std::string display(const Geometry::Vec3D&) const override;
  std::string displayAddress() const override;  
  void displayVec(std::vector<Token>&) const override;
  std::string displayFluka() const override;
  std::string displayPOVRay() const override;
    
};

/*!
  \class BoolValue 
  \brief Rule Status class
  \author S.Ansell
  \version 1.0
  \date April 2005
 
  Class to handle a rule with a truth value
  but can be true/false/unknown.
*/

class BoolValue : public Rule
{
 private:

  int status;          ///< Three values 0 False : 1 True : -1 doesn't matter

  static int procStatus(const int);

 public:
  
  explicit BoolValue(const int);
  BoolValue(const BoolValue&);
  BoolValue* clone() const override;                        
  BoolValue& operator=(const BoolValue&);
  ~BoolValue() override;

  Rule* leaf(const int =0) const override { return 0; } ///< No leaves
  void setLeaves(Rule*,Rule*) override;
  void setLeaf(Rule*,const int =0) override; 
  int findLeaf(const Rule*) const override;
  Rule* findKey(const int) override { return 0; }       ///< No key possible

  int type() const override { return 0; }   ///<  effective type

  /// is empty [alway - no surface]
  bool isEmpty() const override { return 1 ;}

  bool isValid(const Geometry::Vec3D&) const override;
  bool isValid(const Geometry::Vec3D&,const int) const override;
  bool isValid(const std::map<int,int>&) const override;
  bool isValid(const Geometry::Vec3D&,
		       const std::map<int,int>&) const override;    


  int simplify() override;                             


  std::string display() const override;
  std::string display(const Geometry::Vec3D&) const override;
  std::string displayAddress() const override;  
  void displayVec(std::vector<Token>&) const override;
  std::string displayFluka() const override;
  std::string displayPOVRay() const override;
    
};

#endif
