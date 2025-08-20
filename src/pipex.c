/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dgessner <dgessner@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/18 18:24:54 by dgessner          #+#    #+#             */
/*   Updated: 2025/05/25 15:23:12 by dgessner         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static char	*get_path(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

static char	*find_cmd(char *cmd, char **envp)
{
	char	**paths;
	char	*tmp;
	char	*full;
	int		i;

	if (access(cmd, X_OK) == 0)
		return (cmd);
	paths = ft_split(get_path(envp), ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		full = ft_strjoin(tmp, cmd);
		free(tmp);
		if (access(full, X_OK) == 0)
			return (full);
		free(full);
		i++;
	}
	free_split(paths);
	return (NULL);
}

static void	first_child(int infile, int *pipe_fd, char **argv, char **envp)
{
	char	**cmd;

	dup2(infile, 0);
	dup2(pipe_fd[1], 1);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	cmd = ft_split(argv[2], ' ');
	if (!cmd)
	{
		perror("pipex: split");
		exit(1);
	}
	execve(find_cmd(cmd[0], envp), cmd, envp);
	perror("pipex: execve cmd1");
	exit(1);
}

static void	second_child(int outfile, int *pipe_fd, char **argv, char **envp)
{
	char	**cmd;

	dup2(pipe_fd[0], 0);
	dup2(outfile, 1);
	close(pipe_fd[1]);
	close(pipe_fd[0]);
	cmd = ft_split(argv[3], ' ');
	if (!cmd)
	{
		perror("pipex: split");
		exit(1);
	}
	execve(find_cmd(cmd[0], envp), cmd, envp);
	perror("pipex: execve cmd2");
	exit(1);
}

void	pipex(int infile, int outfile, char **argv, char **envp)
{
	int		pipe_fd[2];
	pid_t	pid1;
	pid_t	pid2;

	if (pipe(pipe_fd) == -1)
		return (perror("pipex: pipe"));
	pid1 = fork();
	if (pid1 == -1)
		return (perror("pipex: fork1"));
	if (pid1 == 0)
		first_child(infile, pipe_fd, argv, envp);
	pid2 = fork();
	if (pid2 == -1)
		return (perror("pipex: fork2"));
	if (pid2 == 0)
		second_child(outfile, pipe_fd, argv, envp);
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	waitpid(pid1, NULL, 0);
	waitpid(pid2, NULL, 0);
}
