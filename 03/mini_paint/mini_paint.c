#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

#define ERR_ARGUMENTS "Error: argument\n"
#define ERR_OPERATION "Error: Operation file corrupted\n"

typedef struct s_rules
{
	int		w; // 0 < w <= 300
	int		h;
	char	c;
} t_rules;

typedef struct s_circle
{
	char	c; // c || C
	float	x; 
	float	y;
	float	r; // r > 0
	char	chr;

} t_circle;

int	stamp_error(char *str)
{
	while (*str)
		write(1, &(*str++), 1);
	return (1);
}

int	free_mtx(int **mtx)
{
	int	y;

	y = 0;
	if (mtx)
	{
		while (mtx[y])
			free(mtx[y++]);
		free(mtx);
	}
}

int stamp_free(char *str, int **mtx)
{
	free_mtx(mtx);
	return (stamp_error(str));
}

int invalid_size(t_rules rl)
{
	if (!(rl.w <= 300 && rl.w > 0))
		return (1);
	if (!(rl.h <= 300 && rl.h > 0))
		return (1);
	return (0);
}

int	inputs_are_good(t_circle circle)
{
	if (circle.c != 'c' && circle.c != 'C')
		return (0);
	if (circle.r <= 0)
		return (0);
	return (1);
}

int	drawable(float x, float y, t_circle circle)
{
	float distance;

	distance = sqrtf(powf(circle.x - x, 2) + powf(circle.y - y, 2));
	if (distance <= circle.r)
	{
		if (circle.r - distance <= 1.000000)
			return (1);
		if (circle.c == 'C')
			return (1);
	}
	return (0);
}

void	draw_circle(t_rules rules, int **mtx, t_circle circle)
{
	int	x = 0;
	int y = 0;

	while (y < rules.h)
	{
		x = 0;
		while (x < rules.w)
		{
			if (drawable(x, y, circle))
				mtx[y][x] = circle.chr;
			x++;
		}
		y++;
	}
}

int scan_file(FILE *file)
{
	int		inputs;

	t_rules rules;
	rules.w = 0;
	rules.h = 0;
	rules.c = '\0';

	inputs = fscanf(file, "%d %d %c\n", &rules.w, &rules.h, &rules.c);
	if (inputs != 3 || invalid_size(rules))
		return (stamp_error(ERR_OPERATION));

	t_circle circle;
	circle.c = '\0';
	circle.x = 0.0;
	circle.y = 0.0;
	circle.r = 0.0;
	circle.chr = '\0';

	inputs = fscanf(file, "%c %f %f %f %c\n", &circle.c, &circle.x, &circle.y, &circle.r, &circle.chr);
	if (inputs != -1 && inputs != 5)
		return (stamp_error(ERR_OPERATION));
	
	// create mtx to be printed
	int x;
	int y;

	int	**mtx;
	mtx = malloc(sizeof(int *) * (rules.h + 1));
	y = 0;
	while (y < rules.h)
		mtx[y++] = malloc(sizeof(int) * (rules.w + 1));

	// fill mtx with background char
	y = 0;
	while (y < rules.h)
	{
		x = 0;
		while (x < rules.w)
		{
			mtx[y][x] = rules.c;
			x++;
		}
		mtx[y][x] = '\0';
		y++;
	}
	mtx[y] = NULL;
		
	// write circle into matrix
	while (inputs == 5)
	{
		if (!inputs_are_good(circle))
			return (stamp_free(ERR_OPERATION, mtx));
		draw_circle(rules, mtx, circle);
		inputs = fscanf(file, "%c %f %f %f %c\n", &circle.c, &circle.x, &circle.y, &circle.r, &circle.chr);
	}

	if (inputs != -1)
		return (stamp_free(ERR_OPERATION, mtx));

	// stamp circles
	y = 0;
	while (y < rules.h)
	{
		x = 0;
		while (x < rules.w)
		{
			write(1, &(mtx[y][x]), 1);
			x++;
		}
		write(1, "\n", 1);
		y++;
	}
	free_mtx(mtx);
	return (0);
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return (stamp_error(ERR_ARGUMENTS));

	// open file
	FILE *file;
	file = fopen(argv[1], "r");
	if (!file)
		return (stamp_error(ERR_OPERATION));

	// scan file
	int err = scan_file(file);
	fclose(file);
	return (err);
}

