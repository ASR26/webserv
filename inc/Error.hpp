/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysmeding <ysmeding@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 13:08:41 by ysmeding          #+#    #+#             */
/*   Updated: 2023/11/23 15:32:01 by ysmeding         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HPP
# define ERROR_HPP

# include <string>
# include <map>
# include <errno.h>

class Error
{
	private:

	public:


		static void functionError();
		static void functionError(std::string errstr);
		static std::string responseCode(int err);
};

#endif