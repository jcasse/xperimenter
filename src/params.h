///////////////////////////////////////////////////////////////////////////////
// Name:        params.h
// Purpose:     Declares Params class
// Author:      Juan Casse
// License:     wxWindows License
///////////////////////////////////////////////////////////////////////////////

#ifndef _PARAMS_H_
#define _PARAMS_H_

#include "boost/filesystem.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include "boost/archive/xml_oarchive.hpp"
#include "boost/archive/xml_iarchive.hpp"
#include "boost/serialization/serialization.hpp"
#include "boost/serialization/tracking.hpp"
#include "boost/serialization/base_object.hpp"
#include "boost/serialization/nvp.hpp"
#include "boost/serialization/export.hpp"
#include "boost/serialization/string.hpp"
#include "boost/serialization/shared_ptr.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/serialization/set.hpp"
#include "boost/serialization/map.hpp"

#include <string>
#include <vector>
#include <set>
#include <map>
#include <array>

#include "wx/string.h"
#include "wx/arrstr.h"

#include "utilities.h"

#define SERIALIZATION_NVP(name, T)							\
	boost::serialization::make_nvp(name, T)

#define SERIALIZATION_BASE_OBJECT_NVP(name, T)				\
	boost::serialization::make_nvp(							\
		name,												\
		boost::serialization::base_object<T >(*this)		\
	)

template<typename T> using shptr = boost::shared_ptr<T>;

	class Datum: public std::pair<std::string, size_t> {
	public:
		Datum(const wxString& value, const size_t priority):
		std::pair<std::string, size_t>(value.ToStdString(), priority) {}
		void set_priority(const size_t priority) {second = priority;}
		const size_t get_priority() const {return second;}
		operator wxString() const {return first;}
		bool operator <(const Datum& rhs) const {return second > rhs.second;}
		bool operator >(const Datum& rhs) const {return second < rhs.second;}
		bool operator ==(const Datum& rhs) const {return second == rhs.second;}
	private:
		Datum() = default;
		typedef std::pair<std::string, size_t> datum;
		friend class boost::serialization::access;
		template<typename Ar>
		void serialize(Ar & ar, const unsigned int ver) {
			//ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(datum);
			ar & SERIALIZATION_BASE_OBJECT_NVP("Datum", datum);
		}
	};

	class Value: public std::vector<Datum> {
	public:
		Value() = default;
		Value(const wxString& value) {emplace(value);}
		Value(const std::vector<wxString>::const_iterator first,
				const std::vector<wxString>::const_iterator last) {
			for (auto it = first; it != last; it++) emplace(*it);
			std::make_heap(begin(), end());
		}
		void set(const wxString& value);
		void set(const wxArrayString& values);
		operator wxString() const
		{if (empty()) return wxEmptyString; return *cbegin();}
		const wxArrayString get_values() const {
			wxArrayString ret;
			for (const wxString& value : *this) ret.Add(value);
			return ret;
		}
	private:
		iterator find(const wxString& value) {
			iterator it = begin();
			while (it != end() && wxString(*it) != value) it++;
			return it;
		}
		const_iterator find(const wxString& value) const {
			const_iterator it = cbegin();
			while (it != end() && wxString(*it) != value) it++;
			return it;
		}
		void emplace(const wxString& value) {
			if (find(value) != end()) return;
			emplace_back(value, size() + 1);
		}
		typedef std::vector<Datum> value;
		friend class boost::serialization::access;
		template<typename Ar>
		void serialize(Ar & ar, const unsigned int ver) {
			//ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(value);
			ar & SERIALIZATION_BASE_OBJECT_NVP("Value", value);
		}
	};

	class Name: public std::array<std::string, 3> {
	public:
		Name() = default;
		Name(const wxString& name, const wxString& parent,
				const wxString& type) {
			operator[](0) = name.ToStdString();
			operator[](1) = parent.ToStdString();
			operator[](2) = type.ToStdString();
		}
		void set_type(const wxString& type) {operator[](2) = type;}
		operator wxString() const {return operator[](0);}
		const wxString get_value() const {return operator[](0);}
		const wxString get_parent() const {return operator[](1);}
		const wxString get_type() const {return operator[](2);}
		bool operator <(const Name& rhs) const
		{return *cbegin() < *(rhs.cbegin());}
		bool operator >(const Name& rhs) const
		{return *cbegin() > *(rhs.cbegin());}
	private:
		typedef std::array<std::string, 3> name;
		friend class boost::serialization::access;
		template<typename Ar>
		void serialize(Ar & ar, const unsigned int ver) {
			//ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(name);
			ar & SERIALIZATION_BASE_OBJECT_NVP("Name", name);
		}
	};

	class Param: public std::pair<Name, Value> {
	public:
		Param() = default;
		Param(const std::pair<Name, Value>& param):
			std::pair<Name, Value>(param) {}
		Param(const std::vector<wxString>& line);
		Param(const wxString& name, const wxString& parent,
			const wxString& type, const wxString& value):
			std::pair<Name, Value>(Name(name, parent, type),
			Value(value)) {}
		Param(const Name& name, const Value& value):
		std::pair<Name, Value>(name, value) {}
		void set(const Value& value) {second = value;}
		void set(const wxString& value) {second.set(value);}
		void set(const wxString& type, const wxString& value)
		{first.set_type(type); second.set(value);}
		void set(const wxArrayString& value) {second.set(value);}
		const wxString get_name() const {return first;}
		const wxString get_parent() const {return first.get_parent();}
		const wxString get_type() const {return first.get_type();}
		const wxString get_value() const {return second;}
		const wxArrayString get_values() const {return second.get_values();}
		void write_value(std::fstream& fs) const;
		void write_values(std::fstream& fs) const;
	private:
		typedef std::pair<Name, Value> param;
		friend class boost::serialization::access;
		template<typename Ar>
		void serialize(Ar & ar, const unsigned int ver) {
			//ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(param);
			ar & SERIALIZATION_BASE_OBJECT_NVP("Param", param);
		}
	};

	// Originally implemented as map<Name, Value>.
	// Now as vector of pairs so that parameters can be
	// maintained in arbitrary order.
	// This makes easy keeping a hierarchical structure of parameters
	// for nicer display.
	class Params: public std::vector<Param> {
	public:
		class Exception: public std::exception {
		public:
			Exception(const wxString& msg = "Params Exception"): msg(msg) {}
			virtual const char* what() const throw() {return msg.c_str();}
		private:
			std::string msg;
		};
		Params() = default;
		void set_value(const Name& name, const Value& value);
		void set_value(const wxString& name, const wxString& value);
		void set_values(const wxString& name, const wxArrayString& values);
		void set_param(const wxString& name, const wxString& parent,
			const wxString& type, const wxString& value);
		void insert(const wxString& name, const wxString& parent,
			const wxString& type, const wxString& value);
		const Param get_param(const wxString& name) const;
		const wxString get_type(const wxString& name) const;
		const wxString get_value(const wxString& name) const;
		void add(const std::vector<wxString>& line);
		void load(const wxString& pathname);
	private:
		void add(Param& param);
		void add(Param&& param);
		iterator find(const Name& name) {return find(name.get_value());}
		iterator find(const wxString& name) {
			iterator it = begin();
			while (it != end() && it->get_name() != name) it++;
			return it;
		}
		const_iterator find(const Name& name) const
		{return find(name.get_value());}
		const_iterator find(const wxString& name) const {
			const_iterator it = cbegin();
			while (it != end() && it->get_name() != name) it++;
			return it;
		}
		typedef std::vector<Param> params;
		friend class boost::serialization::access;
		template<typename Ar>
		void serialize(Ar & ar, const unsigned int ver) {
			//ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(params);
			ar & SERIALIZATION_BASE_OBJECT_NVP("Params", params);
		}
	};

BOOST_CLASS_EXPORT_KEY(Datum)
BOOST_CLASS_EXPORT_KEY(Value)
BOOST_CLASS_EXPORT_KEY(Params)

#endif
// _PARAMS_H_
