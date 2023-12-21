/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   miscellaneous.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asolano- <asolano-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/15 08:30:43 by ysmeding          #+#    #+#             */
/*   Updated: 2023/12/21 08:59:31 by asolano-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/webserv.hpp"

size_t nbrEqualChar(std::string str1, std::string str2)
{
	size_t count = 0;
	while (str1[count] && str2[count] && str1[count] == str2[count])
		count++;
	return count;
}

size_t nbrEqualStr(std::vector<std::string> vecstr1, std::vector<std::string> vecstr2)
{
	size_t count = 0;
	while (count < vecstr1.size() && count < vecstr2.size() && vecstr1[count] == vecstr2[count])
		count++;
	return count;
}

std::vector<std::string> split(std::string str, std::string c)
{
	std::vector<std::string> all_words;
	
	std::string word;
	size_t len;
	while (str[0] && str[0] == c[0])
	{
		str.erase(0, 1);
	}
	while (!str.empty())
	{
		if (str.substr(0, std::string::npos).find(c) == std::string::npos)
		{
			all_words.push_back(str);
			str.clear();
		}
		else
		{
			len = str.substr(0, std::string::npos).find(c);
			word = str.substr(0, len);
			all_words.push_back(word);
			str = str.substr(len, std::string::npos);
			while (str[0] && str[0] == c[0])
				str.erase(0, 1);
		}
	}
	return all_words;
}

std::string trimSpaces(std::string str)
{
	while (str.back() == ' ')
		str.pop_back();
	return str;
}

std::string intToStr(const int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str();
}

int	intLen(const int n)
{
	std::stringstream ss;
	ss << n;
	return ss.str().size();
}