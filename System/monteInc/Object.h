/********************************************************************* 
  CombLayer : MCNP(X) Input builder
 
 * File:   monteInc/Object.h
 *
 * Copyright (c) 2004-2017 by Stuart Ansell
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
#ifndef MonteCarlo_Object_h
#define MonteCarlo_Object_h

class Token;

namespace MonteCarlo
{
  class neutron;

/*!
  \class Object
  \brief  Global object for object 
  \version 1.0
  \date July 2005
  \author S. Ansell

  An object is a collection of Rules and
  surface object
*/

class Object
{
 private:

  int ObjName;       ///< Number for the object
  int listNum;       ///< Creation number
  double Tmp;        ///< Starting temperature (if given)
  int MatN;          ///< Material Number 
  int fill;          ///< fill number
  int trcl;          ///< transform number
  int universe;      ///< universe number
  int imp;           ///< importance / 0 
  double density;    ///< Density
  int placehold;     ///< Is cell virtual (ie not in output)
  int populated;     ///< Full population

  HeadRule HRule;    ///< Top rule

  /// Set of surfaces that are logically opposite in the rule.
  std::set<const Geometry::Surface*> logicOppSurf;
 
  int checkSurfaceValid(const Geometry::Vec3D&,const Geometry::Vec3D&) const;
  int checkExteriorValid(const Geometry::Vec3D&,const Geometry::Vec3D&) const;
  /// Calc in/out 
  int calcInOut(const int,const int) const;

 protected:
  
  int objSurfValid;                 ///< Object surface valid

  /// Full surfaces (make a map including complementary object ?)
  std::vector<const Geometry::Surface*> SurList;  
  std::set<int> SurSet;              ///< set of surfaces in cell [signed]

  int trackDirection(const Geometry::Vec3D&,const Geometry::Vec3D&) const;

  bool keyUnit(std::string&,std::string&,std::string&);

 public:
  
  static int startLine(const std::string& Line);

  Object();
  Object(const int,const int,const double,const std::string&);
  Object(const Object&);
  Object& operator=(const Object&);
  virtual Object* clone() const;
  virtual ~Object();

  /// Effective typeid
  virtual std::string className() const { return "Object"; }
  /// Visitor Acceptance
  virtual void acceptVisitor(Global::BaseVisit& A) const
    {  A.Accept(*this); }
  /// Accept visitor for input
  virtual void acceptVisitor(Global::BaseModVisit& A)
    { A.Accept(*this); }

  void setName(const int nx) { ObjName=nx; }           ///< Set Name 
  void setCreate(const int lx) { listNum=lx; }         ///< Set Creation point
  void setTemp(const double A) { Tmp=A; }              ///< Set temperature [Kelvin]
  void setImp(const int A) { imp=A; }                  ///< Set imp
  int setObject(std::string);
  int setObject(const int,const int,const std::vector<Token>&);
  int procString(const std::string&);
  void setDensity(const double D) { density=D; }       ///< Set Density [Atom/A^3]
  void setMaterial(const int M) { MatN=M; }            ///< Set Material number
  void setPlaceHold(const int P) { placehold=P; }      ///< Set placeholder
  int isPlaceHold() const { return placehold; }        ///< Get placeholder

  int complementaryObject(const int,std::string&);
  int hasComplement() const;                           
  int isPopulated() const { return populated; }        ///< Is populated   
  
  int getName() const  { return ObjName; }             ///< Get Name
  int getCreate() const  { return listNum; }           ///< Get Creation point
  int getMat() const { return MatN; }                  ///< Get Material ID
  double getTemp() const { return Tmp; }               ///< Get Temperature [K]
  double getDensity() const { return density; }        ///< Get Density [Atom/A^3]
  int getImp() const { return imp; }                   ///< Get importance

  /// Return the top rule
  const Rule* topRule() const { return HRule.getTopRule(); }
  /// get head rule 
  const HeadRule& getHeadRule() const { return HRule; }
  
  void populate();
  void rePopulate();
  int createSurfaceList();
  void createLogicOpp();
  int isObjSurfValid() const { return objSurfValid; }  ///< Check validity needed
  void setObjSurfValid()  { objSurfValid=1; }          ///< set as valid
  int addSurfString(const std::string&);   
  int removeSurface(const int);        
  int substituteSurf(const int,const int,Geometry::Surface*);  
  void makeComplement();

  bool hasSurface(const int) const;
  int isValid(const Geometry::Vec3D&) const;            
  int isValid(const Geometry::Vec3D&,const int) const;            
  int isDirectionValid(const Geometry::Vec3D&,const int) const;            
  int isValid(const Geometry::Vec3D&,const std::set<int>&) const;            
  int pairValid(const int,const Geometry::Vec3D&) const;   
  int isValid(const std::map<int,int>&) const; 
  std::map<int,int> mapValid(const Geometry::Vec3D&) const;

  int isOnSide(const Geometry::Vec3D&) const;

  int surfSign(const int) const;
  /// Access surface index
  const std::set<int>& getSurfSet() const { return SurSet; }

  std::vector<int> getSurfaceIndex() const;
  /// Access the surface list [of pointers]
  const std::vector<const Geometry::Surface*>& getSurList() const
    { return SurList; }
  
  ///\cond ABSTRACT
  virtual void displace(const Geometry::Vec3D&) {}
  virtual void rotate(const Geometry::Matrix<double>&) {}
  virtual void mirror(const Geometry::Plane&) {}
  ///\endcond ABSTRACT

  // INTERSECTION
  int hasIntercept(const Geometry::Vec3D&,const Geometry::Vec3D&) const;

  std::pair<const Geometry::Surface*,double> 
    forwardInterceptInit(const Geometry::Vec3D&,
		     const Geometry::Vec3D&) const;
  std::pair<const Geometry::Surface*,double> 
    forwardIntercept(const Geometry::Vec3D&,
		     const Geometry::Vec3D&) const;

  int trackCell(const MonteCarlo::neutron&,double&,
		const int,const Geometry::Surface*&,
		const int) const;
  int trackIntoCell(const MonteCarlo::neutron&,double&,
		    const Geometry::Surface*&,const int =0) const;
  int trackOutCell(const MonteCarlo::neutron&,double&,
		   const Geometry::Surface*&,const int =0) const;

  // OUTPUT
  std::string cellCompStr() const;
  std::vector<Token> cellVec() const;
  std::string headStr() const;
  std::string str() const;
  std::string pointStr(const Geometry::Vec3D&) const;
  void write(std::ostream&) const;         
  void writePHITS(std::ostream&) const;    
  void writeFLUKA(std::ostream&) const;    
  void writeFLUKAmat(std::ostream&) const;
  void writePOVRay(std::ostream&) const;    
  void writePOVRaymat(std::ostream&) const;


  void checkPointers() const;

};

 std::ostream& operator<<(std::ostream&,const Object&);

}

#endif
