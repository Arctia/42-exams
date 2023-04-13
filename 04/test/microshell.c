#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define CD_ERR "error: cd: cannot change directory to "
#define CD_ARG "error: cd: bad arguments"
#define EX_ERR "error: cannot execute "

int	stamp_error(char *str, char *str2)
{
	while (*str)
		write(2, &(*str++), 1);
	
	if (str2)
		while (*str2)
			write(2, &(*str2++), 1);
	
	write(2, "\n", 1);	

	return	(1);
}

int	do_cd(char **argv, int i)
{
	if (i != 1)
		return (stamp_error(CD_ARG, NULL));

	if (chdir(argv[1]))
		return (stamp_error(CD_ERR, argv[1]));

	return (0);
}

int	execute_command(char **argv, char **env, int *stdin, int i)
{
	argv[i] = NULL;

	*stdin = dup(0);
	close(*stdin);
	
	execve(argv[0], argv, env);
	return (stamp_error(EX_ERR, argv[0]));
}

int do_simple_command(char **argv, char **env, int *stdin, int i)
{
	if (!fork())
	{
		if (execute_command(argv, env, stdin, i))
			return (1);
	}
	else
	{
		close(*stdin);
		waitpid(-1, NULL, WUNTRACED);
		*stdin = dup(0);
	}
	return (0);
}

int	do_pipes(char **argv, char **env, int *stdin, int i)
{
	int pipes[2];

	pipe(pipes);
	if (!fork())
	{
		dup2(pipes[1], 1);
		close(pipes[0]);
		close(pipes[1]);
		if (execute_command(argv, env, stdin, i))
			return (1);
	}
	else
	{
		close(*stdin);
		close(pipes[1]);
		waitpid(-1, NULL, WUNTRACED);
		*stdin = pipes[0];
	}
	return (0);
}

int main(int cc, char **argv, char **env)
{
	(void) cc;
	
	int	i = 0;
	int	stdin = dup(0);

	while (argv[i] && argv[i + 1])
	{
		argv = &argv[i];
		i = 0;

		while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
			i++;

		if (!strcmp(argv[0], "cd"))
			do_cd(argv, i);
		else if (i && (!argv[i] || !strcmp(argv[i], ";")))
		{
			if (do_simple_command(argv, env, &stdin, i))
				return (1);
		}
		else if (i && !strcmp(argv[i], "|"))
		{
			if (do_pipes(argv, env, &stdin, i))
				return (1);
		}
	}
	close(stdin);
	return (0);
}
