/*

   Copyright [2008] [Trevor Hogan]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   CODE PORTED FROM THE ORIGINAL GHOST PROJECT: http://ghost.pwner.org/

*/

#ifndef UTIL_H
#define UTIL_H



// byte arrays

GAPI BYTEARRAY UTIL_CreateByteArray( unsigned char *a, int size );
GAPI BYTEARRAY UTIL_CreateByteArray( unsigned char c );
GAPI BYTEARRAY UTIL_CreateByteArray( uint16_t i, bool reverse );
GAPI BYTEARRAY UTIL_CreateByteArray( uint32_t i, bool reverse );
GAPI uint16_t UTIL_ByteArrayToUInt16( BYTEARRAY b, bool reverse, unsigned int start = 0 );
GAPI uint32_t UTIL_ByteArrayToUInt32( BYTEARRAY b, bool reverse, unsigned int start = 0 );
GAPI string UTIL_ByteArrayToDecString( BYTEARRAY b );
GAPI string UTIL_ByteArrayToHexString( BYTEARRAY b );
GAPI void UTIL_AppendByteArray( BYTEARRAY &b, BYTEARRAY append );
GAPI void UTIL_AppendByteArrayFast( BYTEARRAY &b, BYTEARRAY &append );
GAPI void UTIL_AppendByteArray( BYTEARRAY &b, unsigned char *a, int size );
GAPI void UTIL_AppendByteArray( BYTEARRAY &b, string append, bool terminator = true );
GAPI void UTIL_AppendByteArrayFast( BYTEARRAY &b, string &append, bool terminator = true );
GAPI void UTIL_AppendByteArray( BYTEARRAY &b, uint16_t i, bool reverse );
GAPI void UTIL_AppendByteArray( BYTEARRAY &b, uint32_t i, bool reverse );
GAPI BYTEARRAY UTIL_ExtractCString( BYTEARRAY &b, unsigned int start );
GAPI unsigned char UTIL_ExtractHex( BYTEARRAY &b, unsigned int start, bool reverse );
GAPI BYTEARRAY UTIL_ExtractNumbers( string s, unsigned int count );
GAPI BYTEARRAY UTIL_ExtractHexNumbers( string s );

// conversions

GAPI string UTIL_ToString( unsigned long i );
GAPI string UTIL_ToString( unsigned short i );
GAPI string UTIL_ToString( unsigned int i );
GAPI string UTIL_ToString( long i );
GAPI string UTIL_ToString( short i );
GAPI string UTIL_ToString( int i );
GAPI string UTIL_ToString( float f, int digits );
GAPI string UTIL_ToString( double d, int digits );
GAPI string UTIL_ToHexString( uint32_t i );
GAPI uint16_t UTIL_ToUInt16( string &s );
GAPI uint32_t UTIL_ToUInt32( string &s );
GAPI int16_t UTIL_ToInt16( string &s );
GAPI int32_t UTIL_ToInt32( string &s );
GAPI double UTIL_ToDouble( string &s );
GAPI string UTIL_MSToString( uint32_t ms );

// files

GAPI bool UTIL_FileExists( string file );
GAPI string UTIL_FileRead( string file, uint32_t start, uint32_t length );
GAPI string UTIL_FileRead( string file );
GAPI bool UTIL_FileWrite( string file, unsigned char *data, uint32_t length );
GAPI string UTIL_FileSafeName( string fileName );
GAPI string UTIL_AddPathSeperator( string path );

// stat strings

GAPI BYTEARRAY UTIL_EncodeStatString( BYTEARRAY &data );
GAPI BYTEARRAY UTIL_DecodeStatString( BYTEARRAY &data );

// other

GAPI bool UTIL_IsLanIP( BYTEARRAY ip );
GAPI bool UTIL_IsLocalIP( BYTEARRAY ip, vector<BYTEARRAY> &localIPs );
GAPI void UTIL_Replace( string &Text, string Key, string Value );
GAPI vector<string> UTIL_Tokenize( string s, char delim );

// math

GAPI uint32_t UTIL_Factorial( uint32_t x );

#define nCr(n, r) (UTIL_Factorial(n) / UTIL_Factorial((n)-(r)) / UTIL_Factorial(r))
#define nPr(n, r) (UTIL_Factorial(n) / UTIL_Factorial((n)-(r)))

#endif
