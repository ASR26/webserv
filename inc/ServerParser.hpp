/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gromero- <gromero-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 15:19:04 by gromero-          #+#    #+#             */
/*   Updated: 2023/12/11 11:28:07 by gromero-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# define SERVERPARSER_HPP

# include "webserv.hpp"
# include "LocationParser.hpp"
# include <fstream>

class	ServerParser
{
	private:
		std::vector<LocationParser> location;
		int							port;
		std::vector<std::string>	s_name;
		std::map<int, std::string>	error;
		int							c_size;

	public:
		ServerParser();
		ServerParser(std::string);
		~ServerParser();
		void getInfo(void);
		int getServerSocket() const;
		void addRequest(int fd);
		void formResponse();
};

#endif
