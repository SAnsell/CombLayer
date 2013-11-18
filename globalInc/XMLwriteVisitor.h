/********************************************************************* 
  CombLayer : MNCPX Input builder
 
 * File:   globalInc/XMLwriteVisitor.h
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
#ifndef XMLwriteVisitor_h
#define XMLwriteVisitor_h

namespace XML
{

class XMLcollect;

/*!
  \class XMLwriteVisitor
  \version 1.0
  \author S. Ansell
  \date April 2008
  \brief XMLwriter (global)

  This is the global output class to XML via the BaseVisit
  system. The .h file is included everywhere so should be
  light, i.e. all classes are forward declared.
*/ 

class XMLwriteVisitor : public Global::BaseVisit
{
 private: 
  
  int managed;                 ///< Is the reference a managed object
  XML::XMLcollect& XOut;       ///< Storage space 
  int spectraData;             ///< Write out spectra data

  XMLwriteVisitor& operator=(const XMLwriteVisitor&);  ///< Inaccessable assignment operator
  
 public:

  XMLwriteVisitor();
  XMLwriteVisitor(XML::XMLcollect&);
  XMLwriteVisitor(const XMLwriteVisitor&);
  ~XMLwriteVisitor();

  /// Access Collection
  XML::XMLcollect& getCollect() const { return XOut; }
  void writeStream(std::ostream&) const;
  void writeFile(const std::string&) const;

  // Geometry stuff:
  virtual void Accept(const Geometry::Surface&);
  virtual void Accept(const Geometry::Quadratic&);
  virtual void Accept(const Geometry::ArbPoly&);
  virtual void Accept(const Geometry::CylCan&);
  virtual void Accept(const Geometry::Cone&);
  virtual void Accept(const Geometry::Cylinder&);
  virtual void Accept(const Geometry::General&);
  virtual void Accept(const Geometry::MBrect&);
  virtual void Accept(const Geometry::NullSurface&);
  virtual void Accept(const Geometry::Plane&);
  virtual void Accept(const Geometry::Sphere&);
  virtual void Accept(const Geometry::Torus&);
  virtual void Accept(const Geometry::Line&);

  // MonteCarlo stuff:
  virtual void Accept(const MonteCarlo::Material&);
  virtual void Accept(const MonteCarlo::Object&);

  // ModelSupport stuff:
  virtual void Accept(const ModelSupport::surfIndex&);
    
  // Functions:
  virtual void Accept(const FuncDataBase&);  
};

}  // NAMESPACE XML

#endif
