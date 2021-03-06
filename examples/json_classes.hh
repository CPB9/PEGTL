// Copyright (c) 2014-2015 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/ColinH/PEGTL/

#ifndef PEGTL_EXAMPLES_JSON_CLASSES_HH
#define PEGTL_EXAMPLES_JSON_CLASSES_HH

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace examples
{
   enum class json_type
   {
      ARRAY,
      BOOLEAN,
      NULL_,
      NUMBER,
      OBJECT,
      STRING
   };

   class json_base
   {
   public:
      const json_type type;

      virtual void stream( std::ostream & ) const = 0;

   protected:
      explicit
      json_base( const json_type type )
            : type( type )
      { }

      ~json_base()
      { }
   };

   inline std::ostream & operator<< ( std::ostream & o, const json_base & j )
   {
      j.stream( o );
      return o;
   }

   inline std::ostream & operator<< ( std::ostream & o, const std::shared_ptr< json_base > & j )
   {
      return j ? ( o << * j ) : ( o << "[nullptr]" );
   }

   struct array_json
         : public json_base
   {
      array_json()
            : json_base( json_type::ARRAY )
      { }

      std::vector< std::shared_ptr< json_base > > data;

      virtual void stream( std::ostream & o ) const override
      {
         o << '[';
         if ( ! data.empty() ) {
            auto iter = data.begin();
            o << * iter;
            while ( ++iter != data.end() ) {
               o << ',' << * iter;
            }
         }
         o << ']';
      }
   };

   struct boolean_json
         : public json_base
   {
      explicit
      boolean_json( const bool data)
            : json_base( json_type::BOOLEAN ),
              data( data )
      { }

      bool data;

      virtual void stream( std::ostream & o ) const override
      {
         o << ( data ? "true" : "false" );
      }
   };

   struct null_json
         : public json_base
   {
      null_json()
            : json_base( json_type::NULL_ )
      { }

      virtual void stream( std::ostream & o ) const override
      {
         o << "null";
      }
   };

   struct number_json
         : public json_base
   {
      explicit
      number_json( const double data )
            : json_base( json_type::NUMBER ),
              data( data )
      { }

      double data;

      virtual void stream( std::ostream & o ) const override
      {
         o << data;
      }
   };

   inline std::string json_escape( const std::string & data )
   {
      std::string r;

      static const char * h = "0123456789abcdef";

      for ( auto c : data ) {
         if ( ( c < 32 ) || ( c == 127 ) ) {
            r += "\\u00";
            r += h[ ( c & 0xf0 ) >> 4 ];
            r += h[   c & 0x0f        ];
         }
         else {
            r += c;  // Assume valid UTF-8.
         }
      }
      return r;
   }

   struct string_json
         : public json_base
   {
      explicit
      string_json( const std::string & data )
            : json_base( json_type::STRING ),
              data( data )
      { }

      std::string data;

      virtual void stream( std::ostream & o ) const override
      {
         o << '"' << json_escape( data ) << '"';
      }
   };

   struct object_json
         : public json_base
   {
      object_json()
            : json_base( json_type::OBJECT )
      { }

      std::multimap< std::string, std::shared_ptr< json_base > > data;

      virtual void stream( std::ostream & o ) const override
      {
         o << '{';
         if ( ! data.empty() ) {
            auto iter = data.begin();
            o << '"' << json_escape( iter->first ) << "\":" << iter->second;
            while ( ++iter != data.end() ) {
               o << ",\"" << json_escape( iter->first ) << "\":" << iter->second;
            }
         }
         o << '}';
      }
   };

} // examples

#endif
