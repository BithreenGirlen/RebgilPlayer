

#include "text_utility.h"

namespace text_utility
{
	void TextToLines(const std::string& strText, std::vector<std::string>& lines)
	{
		std::string strTemp;
		for (size_t nRead = 0; nRead < strText.size(); ++nRead)
		{
			if (strText[nRead] == '\r' || strText[nRead] == '\n')
			{
				if (!strTemp.empty())
				{
					lines.push_back(strTemp);
					strTemp.clear();
				}
				continue;
			}

			strTemp.push_back(strText[nRead]);
		}

		if (!strTemp.empty())
		{
			lines.push_back(strTemp);
			strTemp.clear();
		}
	}

	void TextToLines(const std::wstring& wstrText, std::vector<std::wstring>& lines)
	{
		std::wstring wstrTemp;
		for (size_t nRead = 0; nRead < wstrText.size(); ++nRead)
		{
			if (wstrText[nRead] == L'\r' || wstrText[nRead] == L'\n')
			{
				if (!wstrTemp.empty())
				{
					lines.push_back(wstrTemp);
					wstrTemp.clear();
				}
				continue;
			}

			wstrTemp.push_back(wstrText[nRead]);
		}

		if (!wstrTemp.empty())
		{
			lines.push_back(wstrTemp);
			wstrTemp.clear();
		}
	}

	void SplitTextBySeparator(const std::wstring& wstrText, const wchar_t cSeparator, std::vector<std::wstring>& splits)
	{
		for (size_t nRead = 0; nRead < wstrText.size();)
		{
			const wchar_t* p = wcschr(&wstrText[nRead], cSeparator);
			if (p == nullptr)
			{
				size_t nLen = wstrText.size() - nRead;
				splits.emplace_back(wstrText.substr(nRead, nLen));
				break;
			}

			size_t nLen = p - &wstrText[nRead];
			splits.emplace_back(wstrText.substr(nRead, nLen));
			nRead += nLen + 1;
		}
	}

	void SplitTextBySeparator(const std::string& strText, const char cSeparator, std::vector<std::string>& splits)
	{
		for (size_t nRead = 0; nRead < strText.size();)
		{
			const char* p = strchr(&strText[nRead], cSeparator);
			if (p == nullptr)
			{
				size_t nLen = strText.size() - nRead;
				splits.emplace_back(strText.substr(nRead, nLen));
				break;
			}

			size_t nLen = p - &strText[nRead];
			splits.emplace_back(strText.substr(nRead, nLen));
			nRead += nLen + 1;
		}
	}

	void ReplaceAll(std::wstring& src, const std::wstring& strOld, const std::wstring& strNew)
	{
		if (strOld == strNew)return;

		for (size_t nRead = 0;;)
		{
			size_t nPos = src.find(strOld, nRead);
			if (nPos == std::wstring::npos)break;
			src.replace(nPos, strOld.size(), strNew);
			nRead = nPos + strNew.size();
		}
	}

	void ReplaceAll(std::string& strText, const std::string& strOld, const std::string& strNew)
	{
		if (strOld == strNew)return;

		for (size_t nRead = 0;;)
		{
			size_t nPos = strText.find(strOld, nRead);
			if (nPos == std::string::npos)break;
			strText.replace(nPos, strOld.size(), strNew);
			nRead = nPos + strNew.size();
		}
	}

	void EliminateTag(std::wstring& wstr)
	{
		std::wstring wstrResult;
		wstrResult.reserve(wstr.size());
		int iCount = 0;
		for (const auto& c : wstr)
		{
			if (c == L'<')
			{
				++iCount;
				continue;
			}
			else if (c == L'>')
			{
				--iCount;
				continue;
			}

			if (iCount == 0)
			{
				wstrResult.push_back(c);
			}
		}
		wstr = wstrResult;
	}

	void ExtractTagAttrbutes(const std::string& strText, const char* tagName, const char* attributeName, std::vector<std::string>& attributes)
	{
		if (tagName == nullptr || attributeName == nullptr)return;

		std::string strStart = "<";
		strStart += tagName;

		for (size_t nRead = 0;;)
		{
			size_t nPos = strText.find(strStart, nRead);
			if (nPos == std::string::npos)break;
			nRead = nPos + strStart.size() - 1;

			size_t nEnd = strText.find(">", nRead);
			if (nEnd == std::string::npos)break;

			nPos = strText.find(attributeName, nRead);
			if (nPos == std::string::npos)break;
			if (nPos > nEnd)continue;
			nRead = nPos;

			nPos = strText.find("=", nRead);
			if (nPos == std::string::npos)break;
			nRead = nPos;

			nPos = strText.find("\"", nRead);
			if (nPos == std::string::npos)break;
			++nPos;
			nRead = nPos;

			size_t nPos2 = strText.find("\"", nRead);
			if (nPos2 == std::string::npos)break;

			nRead = nPos2;

			attributes.push_back(strText.substr(nPos, nPos2 - nPos));
		}
	}

	void ToXmlTags(const std::string& strText, const char* tagName, std::vector<std::string>& tags)
	{
		std::string strStart = "<";
		strStart += tagName;

		for (size_t nRead = 0;;)
		{
			size_t nPos = strText.find(strStart, nRead);
			if (nPos == std::string::npos)break;
			nRead = nPos + strStart.size() - 1;

			size_t nEnd = strText.find(">", nRead);
			if (nEnd == std::string::npos)break;
			++nEnd;

			tags.push_back(strText.substr(nPos, nEnd - nPos));

			nRead = nEnd;
		}
	}

	void GetXmlAttributes(const std::string& strTag, std::vector<std::pair<std::string, std::string>> &attributes, bool bSingleQuote)
	{
		const char cQuote = bSingleQuote ? '\'' : '"';

		size_t nPos = strTag.find('<');
		if (nPos == std::string::npos)return;
		++nPos;

		size_t nEnd = strTag.find('>', nPos);
		if (nEnd == std::string::npos)return;

		attributes.clear();

		for (; nPos < nEnd && strTag[nPos] != ' '; ++nPos);

		size_t nRead = ++nPos;
		for (; nPos < nEnd ; ++nPos)
		{
			const char &c = strTag[nPos];

			if (c == '=')
			{
				std::string strName = strTag.substr(nRead, nPos - nRead);

				size_t nValueStart = strTag.find(cQuote, nPos);
				if (nValueStart == std::string::npos)break;
				++nValueStart;

				nPos = strTag.find(cQuote, nValueStart);
				if (nPos == std::string::npos)break;

				std::string strValue = strTag.substr(nValueStart, nPos - nValueStart);
				attributes.push_back({ strName, strValue });

				for (; nPos < nEnd && strTag[nPos] != ' '; ++nPos);
				nRead = ++nPos;
			}
		}
	}
}
