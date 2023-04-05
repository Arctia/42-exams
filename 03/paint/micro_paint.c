
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

int on_axe(float val)
{
	if (val >= 0 && val < 1.0000000)
		return (1);
	return (0);
}

int	inside(float start, float point, float end)
{
	if (point <= end && point >= start)
		return (1);
	return (0);
}

int	inside_rect(int x, int y, t_rect r)
{
	float		xs = r.x * (1.0000000);
	float		ys = r.y * (1.0000000);
	float		xe = (r.x + r.width) * 1.0000000;
	float		ye = (r.y + r.height) * 1.0000000;

	float	diff_xs = (float) x - xs;
	float	diff_ys = (float) y - ys;
	float	diff_xe = xe - (float) x;
	float	diff_ye = ye - (float) y;

	if ((int) xe == (int) xs || (int) ys == (int) ye)
		return (0);

	if (r.fill == 0)
	{
		if (on_axe(diff_xs) && (on_axe(diff_ys) || on_axe(diff_ye)))
		  	return (1);
		else if (on_axe(diff_xe) && (on_axe(diff_ys) || on_axe(diff_ye)))
		 	return (1);

		if (on_axe(diff_xs) && (inside(ys, y, ye)))
			return (1);
		if (on_axe(diff_xe) && (inside(ys, y, ye)))
			return (1);
		if (on_axe(diff_ys) && inside(xs, x, xe))
			return (1);
		if (on_axe(diff_ye) && inside(xs, x, xe))
			return (1);
	}
	else if (r.fill == 1) 
	{	
		if (inside(xs, x, xe) && inside(ys, y, ye))
			return (1);
	}
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
	t_rules rule;
	int	inputs;

	rule.width = 0;
	rule.height = 0;
	rule.bg_char = '\0';


	inputs = fscanf(file, "%i %i %c\n", &rule.width, &rule.height, &rule.bg_char);
	if (inputs != 3
		|| !(rule.width > 0 && rule.width <= 300)
		|| !(rule.height > 0 && rule.height <= 300))
	{
		print(ERR_OPERATION);
		return (2);
	}

	t_rect	rect;
	rect.r = 'r';
	rect.x = 0.000000;
	rect.y = 0.000000;
	rect.width = 0.000000;
	rect.height = 0.000000;
	rect.chr = '\0';

	inputs = fscanf(file, "%c %f %f %f %f %c\n", &(rect.r), &rect.x, &rect.y, &rect.width, &rect.height, &rect.chr);
	
	// init mtx


	// printf("%i %i %c\n", rule.width, rule.height, rule.bg_char);
	// printf("%c %f %f %f %f %c\n", rect.r, rect.x, rect.y, rect.width, rect.height, rect.chr);
	//exit(0);
	if (inputs != -1 && inputs != 6)
	{
		print(ERR_OPERATION);
		return (2);
	}

	// allocate matrix
	int **mtx; // [rule.height + 1][rule.width + 1];
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

	// write into matrix the rectangle
	while (inputs == 6)
	{
		if (!inputs_are_good(&rect))
		{
			print(ERR_OPERATION);
			free_matrix(mtx);
			return (2);
		}
		// write into mtx the rectangle
		draw_rectangle(rule, mtx, rect);
		inputs = fscanf(file, "%c %f %f %f %f %c\n", &rect.r, &rect.x, &rect.y, &rect.width, &rect.height, &rect.chr); 
	}


	if (inputs != -1)
	{
		print(ERR_OPERATION);
		free_matrix(mtx);
		return (2);
	}

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
	{
		print(ERR_ARGUMENTS);
		return (1);
	}

	FILE *file;

	file = fopen(argv[1], "r");
	if (!file)
	{
		print(ERR_OPERATION);
		return (2);
	}
	return (scan_file(file));
}
