/* Copyright 2019 Ole Kliemann, Malte Kliemann
 *
 * This file is part of DrMock.
 *
 * DrMock is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DrMock is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DrMock.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef DRMOCK_SRC_UTILITY_TYPEINFO_H
#define DRMOCK_SRC_UTILITY_TYPEINFO_H

#include <string>

#define DRMOCK_DECLARE_TYPE(Type) \
namespace drutility { namespace detail { \
template<> \
struct TypeInfo<Type> \
{ \
  static const std::string name() \
  { \
    return #Type; \
  } \
  static constexpr bool is_decayed() \
  { \
    return true; \
  } \
  static constexpr bool is_defined() \
  { \
    return true; \
  } \
}; \
}}

#define DRMOCK_DECLARE_TEMPLATE(Type) \
namespace drutility { namespace detail { \
template<> \
struct TemplateInfo<Type> \
{ \
  static const std::string name() \
  { \
    return #Type; \
  } \
  static constexpr bool is_defined() \
  { \
    return true; \
  } \
}; \
}}

namespace drutility { namespace detail {

template<template<typename...> class T>
struct TemplateInfo
{
  static const std::string name()
  {
    return "*unknown template*";
  }
  static constexpr bool is_defined()
  {
    return false;
  }
};

template<typename T, typename... Ts>
struct TypeInfo
{
  static const std::string name()
  {
    return TypeInfo<T>::name() + ", " + TypeInfo<Ts...>::name();
  }
  static constexpr bool is_decayed()
  {
    return true;
  }
  static constexpr bool is_defined()
  {
    return true;
  }
};

template<typename T>
struct TypeInfo<T>
{
  static const std::string name()
  {
    return "*unknown type*";
  }
  static constexpr bool is_decayed()
  {
    return true;
  }
  static constexpr bool is_defined()
  {
    return true;
  }
};

template<typename T>
struct TypeInfo<const T>
{
  static const std::string name()
  {
    if (TypeInfo<T>::is_decayed())
    {
      return "const " + TypeInfo<T>::name();
    }
    else
    {
      return TypeInfo<T>::name() + " const" ;
    }
  }
  static constexpr bool is_decayed()
  {
    return false;
  }
  static constexpr bool is_defined()
  {
    return true;
  }
};

template<typename T>
struct TypeInfo<T*>
{
  static const std::string name()
  {
    return TypeInfo<T>::name() + "*";
  }
  static constexpr bool is_decayed()
  {
    return false;
  }
  static constexpr bool is_defined()
  {
    return true;
  }
};

template<typename T>
struct TypeInfo<T&>
{
  static const std::string name()
  {
    return TypeInfo<T>::name() + "&";
  }
  static constexpr bool is_decayed()
  {
    return false;
  }
  static constexpr bool is_defined()
  {
    return true;
  }
};

template<typename T>
struct TypeInfo<T&&>
{
  static const std::string name()
  {
    return TypeInfo<T>::name() + "&&";
  }
  static constexpr bool is_decayed()
  {
    return false;
  }
  static constexpr bool is_defined()
  {
    return true;
  }
};

template<template<typename...> class T, typename... Ts>
struct TypeInfo<T<Ts...>>
{
  static const std::string name()
  {
    return TemplateInfo<T>::name() + "<" + TypeInfo<Ts...>::name() + ">";
  }
  static constexpr bool is_decayed()
  {
    return true;
  }
  static constexpr bool is_defined()
  {
    return true;
  }
};

}} // namespace drutility::detail

DRMOCK_DECLARE_TYPE(short)
DRMOCK_DECLARE_TYPE(unsigned short)
DRMOCK_DECLARE_TYPE(int)
DRMOCK_DECLARE_TYPE(unsigned int)
DRMOCK_DECLARE_TYPE(long)
DRMOCK_DECLARE_TYPE(unsigned long)
DRMOCK_DECLARE_TYPE(float)
DRMOCK_DECLARE_TYPE(double)
DRMOCK_DECLARE_TYPE(void)
DRMOCK_DECLARE_TYPE(std::string)

#endif /* DRMOCK_SRC_UTILITY_TYPEINFO_H */
