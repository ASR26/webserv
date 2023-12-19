/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationParser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 12:17:28 by gromero-          #+#    #+#             */
/*   Updated: 2023/12/19 15:34:55 by gromero-         ###   ########.fr       */
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
		LocationParser(std::string);
		~LocationParser();
		void		getInfo(void);
		size_t		getMethodsSize() const;
		void		setMethods(std::vector<std::string>);
		std::string	getCGI() const;
};

#endif
