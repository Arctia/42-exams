
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define ERR_OPERATION "Error: Operation file corrupted\n"
#define ERR_ARGUMENTS "Error: argument\n"

typedef struct s_rules
{
	int		w;
	int		h;
	char	c;
}	t_rules;

typedef struct s_rect
{
	char	r;
	float	x;
	float	y;
	float	w;
	float	h;
	char	c;
} 	t_rect;

t_rules	init_rules(void)
{
	t_rules r;

	r.w = 0;
	r.h = 0;
	r.c = 0;

	return (r);
}

t_rect init_rect(void)
{
	t_rect r;

	r.r = 'r';
	r.x = 0.0;
	r.y = 0.0;
	r.w = 0.0;
	r.h = 0.0;
	r.c = 0;

	return (r);
}

int	stamp_error(char *str)
{
	while (*str)
		write(1, &(*str++), 1);
	return (1);
}

void	free_matrix(int **mtx)
{
	int	i;

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
	free_matrix(mtx);
	return stamp_error(str);
}

int	**create_mtx(t_rules r)
{
	int	**mtx;
	int	x;
	int	y;

	y = 0;
	mtx = malloc(sizeof(int *) * (r.h + 1));
	if (!mtx)
		return (NULL);
	while (y < r.h)
	{
		mtx[y] = malloc(sizeof(int) * (r.w + 1));
		if (!mtx[y])
		{
			free_matrix(mtx);
			return (NULL);
		}
		y++;
	}
	mtx[y] = NULL;

	y = 0;
	while (y < r.h)
	{
		x = 0;
		while (x < r.w)
			mtx[y][x++] = r.c;
		mtx[y][x] = 0;
		y++;
	}

	return (mtx);
}

int rules_ok(t_rules r)
{
	if (!(r.w > 0 && r.w <= 300))
		return (0);
	if (!(r.h > 0 && r.h <= 300))
		return (0);
	return (1);
}

int	rect_ok(t_rect r)
{
	if (!(r.r == 'r' || r.r == 'R'))
		return (0);
	if (!(r.w > 0.0 && r.h > 0.0))
		return (0);
	return (1);
}

void	stamp_mtx(int **mtx)
{
	int x = 0;
	int	y = 0;

	while (mtx[y])
	{
		x = 0;
		while (mtx[y][x])
			write(1, &(mtx[y][x++]), 1);
		write(1, "\n", 1);
		y++;
	}
}

int drawable(float x, float y, t_rect r)
{
	float xs = r.x;
	float ys = r.y;
	float xe = r.x + r.w;
	float ye = r.y + r.h;

	if (x < xs || x > xe || y < ys || y > ye)
		return (0);

	float dxs = x - xs;
	float dys = y - ys;
	float dxe = xe - x;
	float dye = ye - y;

	if (dxs < 1.0 || dys < 1.0 || dxe < 1.0 || dye < 1.0)
		return (1);
	if (r.r == 'R')
		return (1);
	return (0);
}

void	write_rect(t_rules rules, int **mtx, t_rect rect)
{
	int x = 0;
	int y = 0;

	while (y < rules.h)
	{
		x = 0;
		while (x < rules.w)
		{
			if (drawable(x, y, rect))
				mtx[y][x] = rect.c;
			x++;
		}
		y++;
	}
}

int	scan_file(FILE *file)
{
	int	inputs;
	t_rules rules = init_rules();
	t_rect rect = init_rect();

	inputs = fscanf(file, "%d %d %c\n", &rules.w, &rules.h, &rules.c);
	if (inputs != 3 || !rules_ok(rules))
		return (stamp_error(ERR_OPERATION));

	int	**mtx = create_mtx(rules);
	if (!mtx)
		return (stamp_error(ERR_OPERATION));

	inputs = fscanf(file, "%c %f %f %f %f %c\n", &rect.r, &rect.x, &rect.y, &rect.w, &rect.h, &rect.c);
	while (inputs == 6)
	{
		if (!rect_ok(rect))
			return (stamp_error_free(ERR_OPERATION, mtx));
		write_rect(rules, mtx, rect);
		inputs = fscanf(file, "%c %f %f %f %f %c\n", &rect.r, &rect.x, &rect.y, &rect.w, &rect.h, &rect.c);
	}

	if (inputs != -1)
		return (stamp_error_free(ERR_OPERATION, mtx));

	stamp_mtx(mtx);
	free_matrix(mtx);
	return (0);
}

int	main(int argc, char **argv)
{
	if (argc != 2)
		return stamp_error(ERR_ARGUMENTS);

	FILE	*file;

	file = fopen(argv[1], "r");
	if (!file)
		return stamp_error(ERR_OPERATION);

	int err = scan_file(file);
	fclose(file);
	return (err);
}
