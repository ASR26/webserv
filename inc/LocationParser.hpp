/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationParser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 12:17:28 by gromero-          #+#    #+#             */
/*   Updated: 2023/12/19 10:09:54 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONPARSER_HPP
# define LOCATIONPARSER_HPP

# include "webserv.hpp"
class LocationParser
{
	private:
		std::string					location;
		std::vector<std::string>	methods;
		std::string					root;
		int							c_size;
		std::string					redir_path;
		std::string					redir_code;
		bool						auto_index;
		std::string					index;
			
	public:
		LocationParser();
		LocationParser(std::string);
		~LocationParser();
		void	getInfo(void);
		std::string getLocation() const;

		bool isAllowedMethod(std::string meth);
		std::string getRoot() const;
		std::string getRedirpath() const;
		std::string getRedircode() const;
		bool getAutoIndex() const;
		
};

#endif
