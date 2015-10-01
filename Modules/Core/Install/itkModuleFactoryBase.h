/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkModuleFactoryBase_h
#define itkModuleFactoryBase_h

#include "itkObjectFactoryBase.h"
#include "itkModuleIOBase.h"

namespace itk
{
/** \class ModuleFactoryBase
 * \brief Create instances of MetaImageIO objects using an object factory.
 * \ingroup ITKIOMeta
 */
class ModuleFactoryBase:public ObjectFactoryBase
{
public:
  /** Standard class typedefs. */
  typedef ModuleFactoryBase         Self;
  typedef ObjectFactoryBase          Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;



  /** Class methods used to interface with the registered factories. */
  //virtual const char * GetITKSourceVersion() const { return ITK_SOURCE_VERSION; }

  /** Run-time type information (and related methods). */
  itkTypeMacro(ModuleFactoryBase, ObjectFactoryBase);
  
  /** Convenient typedefs. */
  typedef ModuleIOBase::Pointer ModuleIOBasePointer;
  typedef ModuleIOBase::CriteriaType CriteriaType;

  /** Create the appropriate ModuleIO depending on
  *  the particulars of the file.
  */
  static ModuleIOBasePointer
    CreateModuleIO(const CriteriaType &criteria);

protected:
  ModuleFactoryBase() {};
  ~ModuleFactoryBase() {};

private:
  ModuleFactoryBase(const Self &); //purposely not implemented
  void operator=(const Self &);     //purposely not implemented
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkModuleFactoryBase.hxx"
#endif

#endif