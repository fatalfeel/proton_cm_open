#ifndef NetUtils_h__
#define NetUtils_h__

class URLEncoder 
{
public:
	void encodeData(const byte *pData, int len, string &finalReturn);

private:
	static bool isOrdinaryChar(char c);
};

class URLDecoder 
{
public:
	static string decode(string str);
	vector<byte> decodeData(const string str);

private:
	static int convertToDec(const char* hex);
	static void getAsDec(char* hex);
};
void BreakDownURLIntoPieces(string url, string &domainOut, string &requestOut, int &port);
string GetDomainFromURL(string url);
char *Base64Encode(const char *data, size_t input_length,size_t *output_length); //up to you to free what it gives ya


#endif // NetUtils_h__