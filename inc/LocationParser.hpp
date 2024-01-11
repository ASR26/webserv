/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationParser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 12:17:28 by gromero-          #+#    #+#             */
/*   Updated: 2024/01/11 11:49:36 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONPARSER_HPP
# define LOCATIONPARSER_HPP

# include "webserv.hpp"
class LocationParser
{
	private:
		std::string							location;
		std::vector<std::string>			methods;
		std::string							root;
		int									c_size;
		bool								auto_index;
		std::string							upload;
		std::string							index;
		std::pair<std::string, std::string>	redirec;
		std::string							cgi_pass;
		std::map<int, std::string>			error;
			
	public:
		LocationParser();
		LocationParser(const LocationParser &);
		LocationParser(std::string);
		~LocationParser();

		LocationParser &operator=(const LocationParser &);
		void		getInfo(void);
		size_t		getMethodsSize() const;
		void		setMethods(std::vector<std::string>);
		std::string getLocation();
		std::string	getCGI() const;
		bool 		isAllowedMethod(std::string meth);
		std::string getRoot() const;
		std::string getRedirpath() const;
		std::string getRedircode() const;
		bool		getAutoIndex() const;
		std::string getIndex() const;
		std::string getUpload() const;
		std::vector<std::string> getMethods() const;
		int getCSize() const;
		void completeRoot(std::string server_root);
};

#endif
