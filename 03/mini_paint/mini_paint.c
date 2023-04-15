#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define ERR_ARG "Error: argument\n"
#define ERR_OP "Error: Operation file corrupted\n"

typedef struct s_rules
{
	int		w;
	int 	h;
	char 	c;

}	t_rules;

typedef struct s_circle
{
	char	c;
	float	x;
	float	y;
	float	r;
	char	f;
}	t_circle;

t_rules	init_rules(void)
{
	t_rules r;

	r.w = 0;
	r.h = 0;
	r.c = 0;

	return (r);
}

t_circle init_circle(void)
{
	t_circle c;

	c.c = 'c';
	c.x = 0.0;
	c.y = 0.0;
	c.r = 0.0;
	c.f = 0;
	
	return (c);
}

int	stamp_error(char *str)
{
	while (*str)
		write(1, &(*str++), 1);
	return (1);
}

void	free_mtx(int **mtx)
{
	int i;

	i = 0;
	if (mtx)
	{
		while (mtx[i])
			free(mtx[i++]);
		free(mtx);
	}
}

int	stamp_error_free(char *str, int **mtx)
{
	free_mtx(mtx);
	return (stamp_error(str));
}

int	rules_ok(t_rules r)
{
	if (!(r.w > 0 && r.w <= 300))
		return (0);
	if (!(r.h > 0 && r.h <= 300))
		return (0);
	return (1);
}

int circle_ok(t_circle c)
{
	if (!(c.c == 'c' || c.c == 'C'))
		return (0);
	if (!(c.r > 0.0))
		return (0);
	return (1);
}

int	**create_mtx(t_rules r)
{
	int **mtx;
	int x = 0;
	int y = 0;

	mtx = malloc(sizeof(int *) * (r.h + 1));
	if (!mtx)
		return (NULL);
	while (y < r.h)
	{
		mtx[y] = malloc(sizeof(int) * (r.w + 1));
		if (!mtx[y])
		{
			free_mtx(mtx);
			return (NULL);
		}
		y++;
	}

	y = 0;
	while (y < r.h)
	{
		x = 0;
		while (x < r.w)
		{
			mtx[y][x] = r.c;
			x++;
		}
		mtx[y][x] = '\0';
		y++;
	}
	mtx[y] = NULL;
	
	return (mtx);
}

void	stamp_mtx(int **mtx)
{
	int x = 0;
	int y = 0;

	while (mtx[y])
	{
		x = 0;
		while (mtx[y][x])
		{
			write(1, &(mtx[y][x]), 1);
			x++;
		}
		write(1, "\n", 1);
		y++;
	}
}

int drawable(float x, float y, t_circle c)
{
	float ds;

	ds = sqrtf(powf(c.x - x, 2.) + powf(c.y - y, 2.));
	if (ds <= c.r)
	{
		if (c.r - ds < 1.000000)
			return (1);
		if (c.c == 'C')
			return (1);
	}
	return (0);
}

void	write_circle(t_rules r, int **mtx, t_circle c)
{
	int x = 0;
	int y = 0;

	while (y < r.h)
	{
		x = 0;
		while (x < r.w)
		{
			if (drawable(x, y, c))
				mtx[y][x] = c.f;
			x++;
		}
		y++;
	}
}

int	scan_file(FILE *file)
{
	int inputs;
	t_rules rules = init_rules();
	t_circle circle = init_circle();

	inputs = fscanf(file, "%d %d %c\n", &rules.w, &rules.h, &rules.c);
	if (inputs != 3 || !rules_ok(rules))
		return (stamp_error(ERR_OP));

	int **mtx = create_mtx(rules);
	if (!mtx)
		return (stamp_error(ERR_OP));
	
	inputs = fscanf(file, "%c %f %f %f %c\n", &circle.c, &circle.x, &circle.y, &circle.r, &circle.f);
	while (inputs == 5)
	{
		if (!circle_ok(circle))
			return (stamp_error_free(ERR_OP, mtx));
		write_circle(rules, mtx, circle);
		inputs = fscanf(file, "%c %f %f %f %c\n", &circle.c, &circle.x, &circle.y, &circle.r, &circle.f);
	}

	if (inputs != -1)
		return (stamp_error_free(ERR_OP, mtx));

	stamp_mtx(mtx);
	free_mtx(mtx);
	return (0);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return (stamp_error(ERR_ARG));

	FILE *file;
	file = fopen(argv[1], "r");
	if (!file)
		return (stamp_error(ERR_OP));

	int	err = scan_file(file);
	fclose(file);
	return (err);
}
