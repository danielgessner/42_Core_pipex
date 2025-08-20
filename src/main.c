/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgessner <dgessner@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 18:22:08 by dgessner          #+#    #+#             */
/*   Updated: 2025/05/25 16:36:56 by dgessner         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include <fcntl.h>

int	main(int argc, char **argv, char **envp)
{
	int	infile;
	int	outfile;

	if (argc != 5)
	{
		perror("wrong number of arguments");
		return (1);
	}
	infile = open(argv[1], O_RDONLY, 0777);
	if (infile < 0)
	{
		perror("pipex infile");
		return (1);
	}
	outfile = open(argv[4], O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (outfile < 0)
	{
		perror("pipex: outfile");
		close(infile);
		return (1);
	}
	pipex(infile, outfile, argv, envp);
	close(infile);
	close(outfile);
	return (0);
}
