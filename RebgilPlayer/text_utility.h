#ifndef TEXT_UTILITY_H_
#define TEXT_UTILITY_H_

#include <string>
#include <vector>

namespace text_utility
{
	void TextToLines(const std::string& strText, std::vector<std::string>& lines);
	void SplitTextBySeparator(const std::string& strText, const char cSeparator, std::vector<std::string>& splits);

	void ReplaceAll(std::wstring& wstrText, const std::wstring& wstrOld, const std::wstring& wstrNew);

	void ExtractTagAttrbutes(const std::string& strText, const char* tagName, const char* attributeName, std::vector<std::string>& attributes);

	void ToXmlTags(const std::string& strText, const char* tagName, std::vector<std::string>& tags);
	void GetXmlAttributes(const std::string& strTag, std::vector<std::pair<std::string, std::string>>& attributes, bool bSingleQuote = false);
}

#endif // !TEXT_UTILITY_H_
