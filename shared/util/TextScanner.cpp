#include "PlatformPrecomp.h"
#include "TextScanner.h"
#include "util/ResourceUtils.h"
#include "util/MiscUtils.h"
#include "FileSystem/FileManager.h"

TextScanner::TextScanner()
{
	m_lastLine = 0;
}

TextScanner::TextScanner(const string &fName )
{
	m_lastLine = 0;
	LoadFile(fName);
}

TextScanner::TextScanner(const string &fName, bool bAddBasePath )
{
	m_lastLine = 0;
	LoadFile(fName, bAddBasePath);
}

TextScanner::TextScanner( const char *pCharArray )
{
	m_lastLine = 0;
	SetupFromMemoryAddress(pCharArray);
}


bool TextScanner::LoadFile( const string &fName, bool bAddBasePath )
{
	Kill();

	FileInstance f(fName, bAddBasePath);
	if (!f.IsLoaded()) return false;

	return SetupFromMemoryAddress(f.GetAsChars());
}

bool TextScanner::SetupFromMemoryAddress(const char *pCharArray)
{
	m_lines = StringTokenize(pCharArray, "\n");

	for (unsigned int i=0; i < m_lines.size(); i++)
	{
		StringReplace("\r", "", m_lines[i]);

	}
	return true;
}

std::string TextScanner::GetParmString( string label, int index,  string token)
{
	if (m_lines.empty())
	{
		LogError("Load a file first");
		return "";
	}

	for (unsigned int i=0; i < m_lines.size(); i++)
	{
		if (m_lines[i].empty()) continue;
		vector<string> line = StringTokenize(m_lines[i], token);
		if (line[0] == label)
		{
			//found it
			return line[index];
		}
	}

	return "";
}

TextScanner::~TextScanner()
{
	Kill();
}

void TextScanner::Kill()
{
	m_lines.clear();
	m_lastLine = 0;
}

std::string TextScanner::GetMultipleLineStrings( string label, string token )
{
	for (unsigned int i=m_lastLine; i < m_lines.size(); i++)
	{
		if (m_lines[i].empty()) continue;
		vector<string> line = StringTokenize(m_lines[i], token);
		if (line[0] == label)
		{
			//found it
			m_lastLine = i+1;
			return m_lines[i];
		}
	}
	m_lastLine = 0; //reset it
	return "";
}

void TextScanner::StripLeadingSpaces()
{
	for (unsigned int i=0; i < m_lines.size(); i++)
	{
		m_lines[i] = StripWhiteSpace(m_lines[i]);

	}

}

string TextScanner::GetLine( int lineNum )
{
	if ((int)m_lines.size() > lineNum && lineNum >= 0)
	{
		return m_lines[lineNum];
	}

	//invalid line
	return "";
}

string TextScanner::GetAll()
{
	string s;

	for (unsigned int i=0; i < m_lines.size(); i++)
	{
		s += StripWhiteSpace(m_lines[i])+"\n";
	}

	return s;
}

std::string TextScanner::GetParmStringFromLine( int lineNum, int index, string token /*= "|"*/ )
{
	assert(lineNum >= 0 && (uint32)lineNum < m_lines.size());
	assert(token.size() == 1 && "We don't actually support a non char delim yet");
	return SeparateStringSTL(m_lines[lineNum], index, token[0]);
}

int TextScanner::GetParmIntFromLine( int lineNum, int index, string token /*= "|"*/ )
{
	assert(lineNum >= 0 && (uint32)lineNum < m_lines.size());
	assert(token.size() == 1 && "We don't actually support a non char delim yet");
	return atoi(SeparateStringSTL(m_lines[lineNum], index, token[0]).c_str());
}

float TextScanner::GetParmFloatFromLine( int lineNum, int index, string token /*= "|"*/ )
{
	assert(lineNum >= 0 && (uint32)lineNum < m_lines.size());
	assert(token.size() == 1 && "We don't actually support a non char delim yet");
	return (float)atof(SeparateStringSTL(m_lines[lineNum], index, token[0]).c_str());
}


void TextScanner::Replace( const string &thisStr, const string &thatStr )
{
	for (unsigned int i=0; i < m_lines.size(); i++)
	{
		StringReplace(thisStr, thatStr, m_lines[i]);
	}

}

void TextScanner::DumpToLog()
{
	for (int i=0; i < GetLineCount(); i++)
	{
		string tmp = m_lines[i].c_str();
	    StringReplace("%", "%%", tmp);
		LogMsg(tmp.c_str());
	}
}

bool TextScanner::SaveFile( const string &fName, bool bAddBasePath /*= true*/ )
{
	string f;

	string lineFeed = "\r\n";
	if (bAddBasePath)
	{
		f = GetSavePath()+fName;
	} else
	{
		f = fName;
	}
	FILE *fp = fopen( f.c_str(), "wb");

	if (!fp)
	{
		LogError("Unable to save data");
		return false;
	}

	for (uint32 i=0; i < m_lines.size(); i++)
	{
		fwrite(m_lines[i].c_str(), m_lines[i].size(), 1, fp);
		fwrite(lineFeed.c_str(), lineFeed.size(), 1, fp);
	}
	
	fclose(fp);
	return true;
}

void TextScanner::DeleteLine( int lineNum )
{
	if (m_lastLine && m_lastLine >= lineNum) m_lastLine--;
	m_lines.erase(m_lines.begin()+lineNum);
}

vector<string> TextScanner::TokenizeLine( int lineNum, const string &theDelimiter /*= "|"*/ )
{
	return StringTokenize(m_lines[lineNum], theDelimiter);
}