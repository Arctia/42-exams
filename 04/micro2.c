
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define CD_ARGS "error: cd: bad arguments"
#define CD_ERR "error: cd: cannot change directory to "
#define EXEC_ERR "error: cannot execute "
#define FATAL "error: fatal"

int	stamp_error(char *s1, char *s2)
{
	while (*s1)
		write(2, &(*s1++), 1);
	if (s2)
	{
		while (*s2)
			write(2, &(*s2++), 1);
	}
	write(2, "\n", 1);
	return (1);
}

int	do_cd(int i, char **argv)
{
	if (i != 1)
		return (stamp_error(CD_ARGS, NULL));
	if (chdir(argv[1]))
		return (stamp_error(CD_ERR, argv[1]));
	return (0);
}

int	execute(int i, char **argv, int *stdin, char **env)
{
	argv[i] = NULL;

	dup2(*stdin, 0);
	close(*stdin);

	execve(argv[0], argv, env);
	return (stamp_error(EXEC_ERR, argv[0]));
}

int	do_simple_command(int i, char **argv, int *stdin, char **env)
{
	if (!fork())
	{
		if (execute(i, argv, stdin, env))
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

int	do_pipes(int i, char **argv, int *stdin, char **env)
{
	int	pipes[2];

	pipe(pipes);
	if (!fork())
	{
		dup2(pipes[1], 1);
		close(pipes[1]);
		close(pipes[0]);
		if (execute(i, argv, stdin, env))
			return (1);
	}
	else
	{
		close(*stdin);
		close(pipes[1]);
		//waitpid(-1, NULL, WUNTRACED);
		*stdin = pipes[0];
	}
	return (0);

}

int	main(int argc, char **argv, char **env)
{
	(void) argc;

	int	stdin;
	int	i;

	stdin = dup(0);
	i = 0;

	while (argv[i] && argv[i + 1])
	{
		argv = &argv[i + 1];
		i = 0;

		while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
			i++;

		if (strcmp(argv[0], "cd") == 0)
			do_cd(i, argv);
		else if (i > 0 && (!argv[i] || strcmp(argv[i], ";") == 0))
		{
			if (do_simple_command(i, argv, &stdin, env))
				return (1);
		}
		else if (i > 0 && (strcmp(argv[i], "|") == 0))
		{
			if (do_pipes(i, argv, &stdin, env))
				return (1);
		}
	}
	close(stdin);
	return (0);
}
