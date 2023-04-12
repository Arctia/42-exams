
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define	CD_ARGS "error: cd: bad arguments"
#define CD_ERR "error: cd: cannot change directory to "
#define EXEC_ERR "error: cannot execute "

int	stamp_error(char *str1, char *str2)
{
	while (*str1)
		write(2, &(*str1++), 1);
	
	if (str2)
	{
		while (*str2)
			write(2, &(*str2++), 1);
	}
	write(2, "\n", 1);

	return (1);
}

int	is_cd_do_it(int argc, char **argv)
{
	if (strcmp(argv[0], "cd"))
		return (0);

	if (argc != 2)
		stamp_error(CD_ARGS, NULL);
	else if (chdir(argv[1]))
		stamp_error(CD_ERR, argv[1]);

	return (1);
}

int	execute(int i, char **argv, int stdin, char **env)
{
	dup2(stdin, 0);
	close(stdin);

	argv[i] = NULL;

	execve(argv[0], argv, env);

	return (stamp_error(EXEC_ERR, argv[0]));
}

int	simple_command(int i, char **argv, int *stdin, char **env)
{
	if (fork() == 0)
	{
		if (execute(i, argv, *stdin, env))
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

	// create pipes
	pipe(pipes);
	// fork
	if (!fork())
	{
		// dup stdout
		dup2(pipes[1], 1);
		// close pipes
		close(pipes[1]);
		close(pipes[0]);
		// execute
		if (execute(i, argv, *stdin, env))
			return (1);
	}
	else
	{
		// close stdin
		close(*stdin);
		// close pipe out
		close(pipes[1]);
		// replace stdin with pipe input
		*stdin = pipes[0];
	}
	return (0);
}

int	main(int argc, char **argv, char **env)
{
	int	i;
	int	stdin;

	(void) argc;
	stdin = dup(0);
	i = 0;

	while (argv[i] && argv[i + 1])
	{
		// increase argv until it is after ; or | 
		argv = &argv[i + 1];
		i = 0;

		// check if argv[i] is | or ; and increase i if not, reaching end of command streak
		while (argv[i] && strcmp(argv[i], ";") && strcmp(argv[i], "|"))
			i++;

		// if command is cd
		if (is_cd_do_it(i, argv))
			;
		else if (!argv[i] || !strcmp(argv[i], ";"))
		{
			if (simple_command(i, argv, &stdin, env))
				return (1);
		}
		else if (!strcmp(argv[i], "|"))
		{
			if (do_pipes(i, argv, &stdin, env))
				return (1);
		}
	}
	close(stdin);
	return (0);
}
