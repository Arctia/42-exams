
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define ERR_ARGUMENTS "Error: argument\n"
#define ERR_OPERATION "Error: Operation file corrupted\n"

typedef struct s_rules
{
	int		width;
	int		height;
	char	bg_char;
} t_rules;

typedef struct s_rect
{
	char	r;
	float	x;
	float	y;
	float	width;
	float	height;
	char	chr;
	int		fill;
} t_rect;

void	print(char *str)
{
	while (*str)
		write(1, &(*str++), 1);
}

int	stamp_error(char *str)
{
	print(str);
	return (1);
}


void	free_matrix(int	**mtx)
{
	int	y = 0;

	if (mtx)
	{
		while (mtx[y])
			free(mtx[y++]);
		free(mtx);
	}
}

int	stamp_error_free(char *str, int **mtx)
{
	print(str);
	free_matrix(mtx);
	return (1);
}

int	inputs_are_good(t_rect *r)
{
	r->fill = FALSE;
	if (!(r->r == 'r' || r->r == 'R'))
		return (FALSE);
	if (r->r == 'R')
		r->fill = TRUE;
	if (!(r->width > 0 || r->height > 0))
		return (FALSE);
	return (TRUE); 
}

int outside_borders(float dxs, float dxe, float dys, float dye)
{
	if (dxs < 0.0 || dxe < 0.0 || dys < 0.0 || dye < 0.0)
		return (1);
	return (0);
}

int on_border(float dxs, float dxe, float dys, float dye)
{
	if (dxs < 1.0 || dxe < 1.0 || dys < 1.0 || dye < 1.0)
		return (1);
	return (0);
}

int	inside_rect(float x, float y, t_rect r)
{
	float xs = r.x;
	float ys = r.y;
	float xe = r.x + r.width;
	float ye = r.y + r.height;

	// border distances
	float dxs = x - xs;
	float dys = y - ys;
	float dxe = xe - x;
	float dye = ye - y;

	// not in rectangle, out of boundary
	if (outside_borders(dxs, dxe, dys, dye))
		return (0);

	// on border
	if (on_border(dxs, dxe, dys, dye))
		return (1);

	// should fill?
	if (r.r == 'R')
		return (1);

	return (0);
}

void	draw_rectangle(t_rules rule, int **mtx, t_rect r)
{
	int	x = 0;
	int	y = 0;

	while (y < rule.height)
	{
		x = 0;
		while (x < rule.width)
		{
			if (inside_rect(x, y, r))
				mtx[y][x] = r.chr;
			x++;
		}
		y++;
	}
}

int	scan_file(FILE *file)
{
	int	inputs;

	t_rules rule;
	rule.width = 0;
	rule.height = 0;
	rule.bg_char = '\0';

	inputs = fscanf(file, "%i %i %c\n", &rule.width, &rule.height, &rule.bg_char);
	if (inputs != 3
		|| !(rule.width > 0 && rule.width <= 300)
		|| !(rule.height > 0 && rule.height <= 300))
		return (stamp_error(ERR_OPERATION));

	// allocate matrix
	int **mtx;
	mtx = malloc(sizeof(int *) * (rule.height + 1));
	int y = 0;
	while (y < rule.height)
		mtx[y++] = malloc(sizeof(int) * (rule.width + 1));

	// fill field with bg and null terminate them
	y = 0;
	int x = 0;
	while (y < rule.height)
	{
		x = 0;
		while (x < rule.width)
		{
			mtx[y][x] = rule.bg_char;
			x++;
		}
		mtx[y][x] = '\0';
		y++;
	}
	mtx[y] = NULL;

	// init rectangle
	t_rect	rect;
	rect.r = 'r';
	rect.x = 0.0;
	rect.y = 0.0;
	rect.width = 0.0;
	rect.height = 0.0;
	rect.chr = '\0';

	// write into matrix the rectangle
	inputs = fscanf(file, "%c %f %f %f %f %c\n", &(rect.r), &rect.x, &rect.y, &rect.width, &rect.height, &rect.chr);
	while (inputs == 6)
	{
		if (!inputs_are_good(&rect))
			return (stamp_error_free(ERR_OPERATION, mtx));
		draw_rectangle(rule, mtx, rect);
		inputs = fscanf(file, "%c %f %f %f %f %c\n", &rect.r, &rect.x, &rect.y, &rect.width, &rect.height, &rect.chr); 
	}

	if (inputs != -1 && inputs != 6)
		return (stamp_error_free(ERR_OPERATION, mtx));

	// print the result
	y = 0;
	while (y < rule.height)
	{
		x = 0;
		while (x < rule.width)
		{
			write(1, &mtx[y][x], 1);
			x++;
		}
		write(1, "\n", 1);
		y++;
	}
	free_matrix(mtx);
	return (0);
}

int	main(int argc, char *argv[])
{
	if (argc != 2)
		return (stamp_error(ERR_ARGUMENTS));

	FILE *file;
	file = fopen(argv[1], "r");
	if (!file)
		return (stamp_error(ERR_ARGUMENTS));

	int	err = scan_file(file);
	fclose(file);

	return (err);
}
