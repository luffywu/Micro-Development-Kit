// ConfigFile.cpp: implementation of the ConfigFile class.
//
//////////////////////////////////////////////////////////////////////

#include "mdk/ConfigFile.h"
#include <assert.h>
#include <stdlib.h>
using namespace std;
namespace mdk
{

void TrimString( string &str, string del )
{
	int nPos = 0;
	unsigned int i = 0;
	for ( ; i < del.size(); i++ )
	{
		while ( true )
		{
			nPos = str.find( del.c_str()[i], 0 );
			if ( -1 == nPos ) break;
			str.replace( str.begin() + nPos, str.begin() + nPos+1, "" );
		}
	}
}

void TrimStringLeft( string &str, string del )
{
	unsigned int i = 0;
	bool bTrim = false;
	for ( ; i < str.size(); i++ )
	{
		if ( string::npos == del.find(str.c_str()[i]) ) break;
		bTrim = true;
	}
	if ( !bTrim ) return;
	str.replace( str.begin(), str.begin() + i, "" );
}

void TrimStringRight( string &str, string del )
{
	int i = str.size() - 1;
	bool bTrim = false;
	for ( ; i >= 0; i-- )
	{
		if ( string::npos == del.find(str.c_str()[i]) ) break;
		bTrim = true;
	}
	if ( !bTrim ) return;
	str.replace( str.begin() + i + 1, str.end(), "" );
}

//ѹ���հ��ַ�
char* Trim( char *str )
{
	if ( NULL == str || '\0' == str[0] ) return str;
	int i = 0;
	char *src = str;
	char strTemp[256];
	for ( i = 0; '\0' != *src ; src++ )
	{
		if ( ' ' == *src || '\t' == *src ) continue;
		strTemp[i++] = *src;
	}
	strTemp[i++] = '\0';
	strcpy( str, strTemp );
	return str;
}

//ѹ���հ��ַ�
char* TrimRight( char *str )
{
	if ( NULL == str || '\0' == str[0] ) return str;
	int i = 0;
	char *src = str;
	for ( i = 0; '\0' != *src ; src++ )
	{
		if ( ' ' == *src || '\t' == *src ) 
		{
			i++;
			continue;
		}
		i = 0;
	}
	str[strlen(str) - i] = 0;
	return str;
}

//ѹ���հ��ַ�
char* TrimLeft( char *str )
{
	if ( NULL == str || '\0' == str[0] ) return str;
	char *src = str;
	for ( ; '\0' != *src ; src++ )
	{
		if ( ' ' != *src && '\t' != *src ) break;
	}
	char strTemp[256];
	strcpy( strTemp, src );
	strcpy( str, strTemp );
	return str;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ConfigFile::ConfigFile( const char *strName )
{
	m_strName = strName;
	m_pFile = NULL;
	assert( ReadFile() );
}

ConfigFile::~ConfigFile()
{
	Close();
}

//�ر��ļ�
void ConfigFile::Close()
{
	if ( NULL == m_pFile ) return;
	fclose( m_pFile );
	m_pFile = NULL;
}

//���ļ�
bool ConfigFile::Open( bool bRead )
{
	Close();
	if ( bRead ) m_pFile = fopen( m_strName.c_str(), "r" );
	else m_pFile = fopen( m_strName.c_str(), "w" );
	
	return NULL != m_pFile;
}

bool ConfigFile::ReadFile()
{
	if ( !Open( true ) ) return false;//�Զ���ʽ���ļ�
	
	fseek( m_pFile, SEEK_SET, 0 );
	char lpszLine[4096];
	string line;
	string name;
	string value;
	string m_description = "";
	CFGItem item;
		
	int nPos = 0;
	while ( NULL != fgets( lpszLine, 4096, m_pFile ) )
	{
		//����ע����
		if ( '#' ==  lpszLine[0] 
			||( '/' == lpszLine[0] && '/' == lpszLine[1] ) 
			||( '\\' == lpszLine[0] && '\\' == lpszLine[1] ) 
			)
		{
			m_description += lpszLine;
			continue;
		}
		line = lpszLine;
		TrimString( line, "\r\n" );//ɾ�����з�
		if ( "" == line ) continue;//��������
		
		nPos = line.find( '=', 0 );
		if ( -1 == nPos ) continue;
		int nEnd = line.size() - 1;
		if ( nPos >= nEnd ) continue;
		name.assign( line.begin(), line.begin() + nPos );
		TrimStringRight(name, " \t");
		TrimStringLeft(name, " \t");
		
		value.assign( line.begin() + nPos + 1, line.end() );
		TrimStringRight(value, " \t");
		TrimStringLeft(value, " \t");
		item = value;
		item.m_description = m_description;
		m_content.insert(ConfigMap::value_type(name,item));
		m_description = "";
	}
	Close();
	
	return true;
}

CFGItem& ConfigFile::operator []( string key )
{
	assert( "" != key );
	ConfigMap::iterator it = m_content.find( key );
	if ( it == m_content.end() ) 
	{
		CFGItem item;
		item = "";
		item.m_description = "";
		m_content.insert(ConfigMap::value_type(key,item));
		it = m_content.find( key );
		assert( it != m_content.end() );
	}
	return it->second;
}

bool ConfigFile::Save()
{
	if ( !Open( false ) ) return false;//��д��ʽ���ļ�
	
	fseek( m_pFile, SEEK_SET, 0 );
	string line;
	CFGItem item;
	ConfigMap::iterator it = m_content.begin();
	for ( ; it != m_content.end(); it++ )
	{
		fprintf( m_pFile, "%s", it->second.m_description.c_str() );
		fprintf( m_pFile, "%s=%s\r\n\r\n", it->first.c_str(), it->second.m_value.c_str() );
	}
	Close();

	return true;
}

CFGItem::CFGItem()
{
}

CFGItem::~CFGItem()
{
}

CFGItem::operator string()
{
	return m_value;
}

CFGItem& CFGItem::operator = ( double value )
{
	char strValue[128];
	sprintf( strValue, "%f", value );
	m_value = strValue;
	TrimStringRight(m_value,"0");
	return *this;
}

CFGItem& CFGItem::operator = ( int value )
{                      
	char strValue[128];
	sprintf( strValue, "%d", value );
	m_value = strValue;
	return *this;
}

CFGItem& CFGItem::operator = ( string value )
{
	m_value = value;
	return *this;
}

}//namespace mdk
