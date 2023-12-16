/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationParser.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/28 12:17:28 by gromero-          #+#    #+#             */
/*   Updated: 2023/12/16 10:14:52 by ysmeding         ###   ########.fr       */
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
			
	public:
		LocationParser();
		LocationParser(std::string);
		~LocationParser();
		void	getInfo(void);
		std::string getLocation() const;

		bool isAllowedMethod(std::string meth);
		std::string getRoot() const;
};

#endif
