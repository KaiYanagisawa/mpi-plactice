/*
 *  Sample program of homology search using Smith-Waterman algorithm
 *
 *  Last updated: 2009/03/15
 *  Author: A.Naruse
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <mpi.h>

#define DIR_NONE 0
#define DIR_DIAG 1
#define DIR_LEFT 2
#define DIR_UP 3
#define LINE_MAX 2048

/*
 *
 */
typedef struct _sequence
{
	int len;
	char *name;
	char *array;
	int score;
} sequence;

typedef struct _sequence_set
{
	int num;
	int max;
	sequence *seq;
} sequence_set;

typedef struct _link_info
{
	char best_link;
	char link_diag;
	char link_left;
	char link_up;
} link_info;

/*
 *
 */
int gap_penalty_first = -12;
int gap_penalty_extend = -2;
int score_matrix[26][26];

/*
 *
 */
FILE *fopen_e(const char *file, const char *mode)
{
	char *str;
	FILE *fp;

	fp = fopen(file, mode);
	if (fp == NULL)
	{
		str = strerror(errno);
		fprintf(stderr, "fopen failed (%s): %s\n", file, str);
		exit(EXIT_FAILURE);
	}
	return (fp);
}

/*
 *
 */
void *malloc_e(size_t size, char *name)
{
	char *str;
	void *buf;

	buf = (void *)malloc(size);
	if (buf == NULL)
	{
		str = strerror(errno);
		fprintf(stderr, "malloc failed (%s): %s\n", name, str);
		exit(EXIT_FAILURE);
	}
	return (buf);
}

/*
 *
 */
void *realloc_e(void *ptr, size_t size, char *name)
{
	char *str;
	void *buf;

	buf = (void *)realloc(ptr, size);
	if (buf == NULL)
	{
		str = strerror(errno);
		fprintf(stderr, "realloc failed (%s): %s\n", name, str);
		exit(EXIT_FAILURE);
	}
	return (buf);
}

/*
 *
 */
void chomp(char *str)
{
	int i;

	i = strlen(str) - 1;
	while (i >= 0 && iscntrl(str[i]))
	{
		str[i--] = 0;
	}
}

/*
 *
 */
void load_score_matrix(char *file)
{
	int i, j;
	int jj[26];
	char line[LINE_MAX];
	char *ptr;
	char c;
	FILE *fp;

	fp = fopen_e(file, "r");

	for (i = 0; i < 26; i++)
	{
		for (j = 0; j < 26; j++)
		{
			score_matrix[i][j] = -100;
		}
	}

	for (i = 0; i < 26; i++)
	{
		jj[i] = i;
	}

	while (fgets(line, LINE_MAX, fp))
	{

		if (line[0] == '#')
		{
			/* comment lines */
			continue;
		}

		if (isspace(line[0]))
		{
			/* header line */
			ptr = line;
			j = 0;
			while (isprint(*ptr))
			{
				if (isalpha(*ptr))
				{
					c = toupper(*ptr) - 'A';
					jj[j] = c;
					j++;
				}
				ptr++;
			}
			continue;
		}

		if (isalpha(line[0]))
		{
			/* score line */
			ptr = line;
			i = toupper(*ptr) - 'A';
			ptr++;
			j = 0;
			while (isprint(*ptr))
			{
				if (isspace(*ptr))
				{
					ptr++;
					continue;
				}
				score_matrix[i][jj[j]] = (int)strtol(ptr, &ptr, 10);
				j++;
			}
			continue;
		}
	}

	fclose(fp);
}

/*
 *
 */
void set_seq_name(char *name, sequence *seq)
{
	size_t size;
	int len;

	len = strlen(name);
	size = sizeof(char) * (len + 1);
	seq->name = (char *)malloc_e(size, "seq->name");
	memcpy(seq->name, name, len);
	seq->name[len] = 0;
	chomp(seq->name);
}

/*
 *
 */
void set_seq_array(char *array, int len, sequence *seq)
{
	size_t size;

	seq->len = len;
	size = sizeof(char) * (len + 1);
	seq->array = (char *)malloc_e(size, "seq->array");
	memcpy(seq->array, array, len);
	seq->array[len] = 0; /* just in case */
}

/*
 *
 */
void load_sequence_set(char *file, sequence_set *set)
{
	size_t size;
	int max = 1024;
	int id = -1;
	int len = 0;
	char line[LINE_MAX];
	char *temp_array;
	char *ptr;
	FILE *fp;

	fp = fopen_e(file, "r");

	set->num = 0;
	set->max = 16;
	size = sizeof(sequence) * set->max;
	set->seq = (sequence *)malloc_e(size, "set->seq");

	size = sizeof(char) * max;
	temp_array = (char *)malloc_e(size, "temp_array");

	while (fgets(line, LINE_MAX, fp))
	{
		if (line[0] == '>')
		{

			/* header line */

			if (id >= 0)
			{
				set_seq_array(temp_array, len, &(set->seq[id]));
			}

			id = set->num++;
			len = 0;

			if (set->max <= set->num)
			{
				set->max *= 2;
				size = sizeof(sequence) * set->max;
				set->seq = (sequence *)realloc_e(set->seq, size, "set->seq");
			}

			set_seq_name(line + 1, &(set->seq[id]));
		}
		else
		{

			/* body line */

			ptr = line;
			while (isprint(*ptr))
			{
				if (!isalpha(*ptr))
				{
					ptr++;
					continue;
				}

				temp_array[len++] = toupper(*ptr) - 'A';
				ptr++;

				if (max <= len)
				{
					max *= 2;
					size = sizeof(char) * max;
					temp_array = (char *)realloc_e(temp_array, size, "temp_array");
				}
			}
			temp_array[len] = 0;
		}
	}

	if (id >= 0)
	{
		set_seq_array(temp_array, len, &(set->seq[id]));
	}

	free(temp_array);

	fclose(fp);
}

#define LINFO(P1, P0) linfo[(P1) * (len0 + 1) + (P0)]
/*
 *
 */
void show_alignment(sequence *seq0, sequence *seq1)
{
	int best_score = 0;
	int *diag[2];
	int *left;
	int up;
	int left_1st_gap;
	int left_ext_gap;
	int up_1st_gap;
	int up_ext_gap;
	int c0, c1;
	int i, ii;
	int j, jj;
	int pre, cur;
	int len0 = seq0->len;
	int len1 = seq1->len;
	int *p0;
	int *p1;
	int pmax;
	int t;
	int tlen = 0;
	char link;
	size_t size;
	link_info *linfo;

	if (seq0->score != seq1->score)
	{
		/*
		 *  when score is not the best score, return soon.
		 */
		return;
	}

	size = sizeof(link_info) * (len0 + 1) * (len1 + 1);
	linfo = (link_info *)malloc_e(size, "linfo");
	memset(linfo, 0, size);

	size = sizeof(int) * (seq0->len + 1);
	diag[0] = (int *)malloc_e(size, "diag[0]");
	diag[1] = (int *)malloc_e(size, "diag[1]");
	left = (int *)malloc_e(size, "left");

	memset(diag[0], 0, size);
	memset(diag[1], 0, size);
	memset(left, 0, size);
	up = 0;

	pre = 0;
	cur = 1;

	pmax = 1024;
	size = sizeof(int) * pmax;
	p0 = (int *)malloc_e(size, "p0");
	p1 = (int *)malloc_e(size, "p1");
	memset(p0, 0, size);
	memset(p1, 0, size);

	/*
	 *
	 */
	for (j = 0; j < len1; j++)
	{

		c1 = (int)(seq1->array[j]);
		up = 0;
		jj = j + 1;

		for (i = 0; i < len0; i++)
		{

			c0 = (int)(seq0->array[i]);
			ii = i + 1;

			/*
			 *  diagonal
			 */
			diag[cur][ii] = diag[pre][i] + score_matrix[c1][c0];
			if (diag[cur][ii] > 0)
			{
				LINFO(jj, ii).link_diag = LINFO(j, i).best_link;
				LINFO(jj, ii).best_link = DIR_DIAG;
			}
			else
			{
				diag[cur][ii] = 0;
				LINFO(jj, ii).link_diag = DIR_NONE;
				LINFO(jj, ii).best_link = DIR_NONE;
			}

			/*
			 *  left gap
			 */
			left_1st_gap = diag[pre][ii] + gap_penalty_first;
			left_ext_gap = left[ii] + gap_penalty_extend;
			if (left_1st_gap > left_ext_gap)
			{
				left[ii] = left_1st_gap;
				LINFO(jj, ii).link_left = LINFO(j, ii).best_link;
			}
			else
			{
				left[ii] = left_ext_gap;
				LINFO(jj, ii).link_left = DIR_LEFT;
			}
			if (left[ii] <= 0)
			{
				left[ii] = 0;
				LINFO(jj, ii).link_left = DIR_NONE;
			}
			else if (diag[cur][ii] < left[ii])
			{
				diag[cur][ii] = left[ii];
				LINFO(jj, ii).best_link = DIR_LEFT;
			}

			/*
			 *  up gap
			 */
			up_1st_gap = diag[cur][i] + gap_penalty_first;
			up_ext_gap = up + gap_penalty_extend;
			if (up_1st_gap > up_ext_gap)
			{
				up = up_1st_gap;
				LINFO(jj, ii).link_up = LINFO(jj, i).best_link;
			}
			else
			{
				up = up_ext_gap;
				LINFO(jj, ii).link_up = DIR_UP;
			}
			if (up <= 0)
			{
				up = 0;
				LINFO(jj, ii).link_up = DIR_NONE;
			}
			else if (diag[cur][ii] < up)
			{
				diag[cur][ii] = up;
				LINFO(jj, ii).best_link = DIR_UP;
			}

			/*
			 *  keep best score and position
			 */
			if (best_score < diag[cur][ii])
			{
				best_score = diag[cur][ii];
				p0[0] = ii;
				p1[0] = jj;
			}
		}

		pre ^= 1;
		cur ^= 1;
	}

	free(diag[0]);
	free(diag[1]);
	free(left);

	/*
	 *
	 */
	ii = p0[0];
	jj = p1[0];
	t = 0;
	link = LINFO(jj, ii).best_link;
	while (link != DIR_NONE)
	{
		t++;

		if (pmax <= t)
		{
			pmax *= 2;
			size = sizeof(int) * pmax;
			p0 = (int *)realloc_e(p0, size, "p0");
			p1 = (int *)realloc_e(p1, size, "p1");
		}

		if (link == DIR_DIAG)
		{
			link = LINFO(jj, ii).link_diag;
			p0[t] = --ii;
			p1[t] = --jj;
		}
		else if (link == DIR_LEFT)
		{
			link = LINFO(jj, ii).link_left;
			p0[t] = ii;
			p1[t] = --jj;
		}
		else if (link == DIR_UP)
		{
			link = LINFO(jj, ii).link_up;
			p0[t] = --ii;
			p1[t] = jj;
		}
	}
	tlen = t;

	/*
	 *  show result
	 */
	printf("Query sequence: %s\n", seq0->name);
	printf("Database sequence: %s\n", seq1->name);
	printf("Best score: %d\n", best_score);

	if (tlen > 0)
	{
		/* query */
		printf("Q:%7d ", p0[tlen - 1]);
		pre = -1;
		for (t = tlen - 1; t >= 0; t--)
		{
			cur = p0[t];
			if (cur != pre)
			{
				printf("%c", seq0->array[cur - 1] + 'A');
			}
			else
			{
				printf("-");
			}
			pre = cur;
		}
		printf(" %d\n", p0[0]);

		/* database */
		printf("D:%7d ", p1[tlen - 1]);
		pre = -1;
		for (t = tlen - 1; t >= 0; t--)
		{
			cur = p1[t];
			if (cur != pre)
			{
				printf("%c", seq1->array[cur - 1] + 'A');
			}
			else
			{
				printf("-");
			}
			pre = cur;
		}
		printf(" %d\n", p1[0]);
	}
	printf("\n");

	free(linfo);
	free(p0);
	free(p1);
}

/*
 *
 */
int get_smith_waterman_score(sequence *seq0, sequence *seq1)
{
	int best_score = 0;
	int *diag[2];
	int *left;
	int up;
	int left_1st_gap;
	int left_ext_gap;
	int up_1st_gap;
	int up_ext_gap;
	int c0, c1;
	int i, ii;
	int j;
	int pre, cur;
	size_t size;

	size = sizeof(int) * (seq0->len + 1);
	diag[0] = (int *)malloc_e(size, "diag[0]");
	diag[1] = (int *)malloc_e(size, "diag[1]");
	left = (int *)malloc_e(size, "left");

	memset(diag[0], 0, size);
	memset(diag[1], 0, size);
	memset(left, 0, size);
	up = 0;

	pre = 0;
	cur = 1;

	for (j = 0; j < seq1->len; j++)
	{

		c1 = (int)(seq1->array[j]);
		up = 0;

		for (i = 0; i < seq0->len; i++)
		{

			c0 = (int)(seq0->array[i]);
			ii = i + 1;

			/*
			 *  diagonal
			 */
			diag[cur][ii] = diag[pre][i] + score_matrix[c1][c0];
			if (diag[cur][ii] < 0)
			{
				diag[cur][ii] = 0;
			}

			/*
			 *  left gap
			 */
			left_1st_gap = diag[pre][ii] + gap_penalty_first;
			left_ext_gap = left[ii] + gap_penalty_extend;
			if (left_1st_gap > left_ext_gap)
			{
				left[ii] = left_1st_gap;
			}
			else
			{
				left[ii] = left_ext_gap;
			}
			if (left[ii] < 0)
			{
				left[ii] = 0;
			}
			else if (diag[cur][ii] < left[ii])
			{
				diag[cur][ii] = left[ii];
			}

			/*
			 *  up gap
			 */
			up_1st_gap = diag[cur][i] + gap_penalty_first;
			up_ext_gap = up + gap_penalty_extend;
			if (up_1st_gap > up_ext_gap)
			{
				up = up_1st_gap;
			}
			else
			{
				up = up_ext_gap;
			}
			if (up < 0)
			{
				up = 0;
			}
			else if (diag[cur][ii] < up)
			{
				diag[cur][ii] = up;
			}

			/*
			 *  keep best score
			 */
			if (best_score < diag[cur][ii])
			{
				best_score = diag[cur][ii];
			}
		}

		pre ^= 1;
		cur ^= 1;
	}

	free(diag[0]);
	free(diag[1]);
	free(left);

	return best_score;
}

/*
 *
 */
int main(int argc, char **argv)
{
	char *file_matrix;
	char *file_query;
	char *file_database;
	int id_query;
	int id_database;
	int best_score;
	int score;
	int rank, size;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	sequence_set query_set;
	sequence_set database_set;

	assert(argc >= 5);
	file_matrix = argv[1];
	file_query = argv[2];
	file_database = argv[3];

	load_score_matrix(file_matrix);
	load_sequence_set(file_query, &query_set);
	load_sequence_set(file_database, &database_set);

	if (rank == 0)
	{
		int query_number_per_rank;
		int surplus_query_number_per_rank;
		int rank0_query_number;

		query_number_per_rank         = query_set.num / size;
		surplus_query_number_per_rank = query_set.num % size;
		rank0_query_number 	          = query_number_per_rank + 
																		((surplus_query_number_per_rank > 0) ? 1 : 0);

		for (int i = 1; i < size; i++)
		{
			int query_start;
			int number_of_queries;

			query_start 			= query_number_per_rank * i + 
													((i < surplus_query_number_per_rank) ? i : surplus_query_number_per_rank);
			number_of_queries = query_number_per_rank + ((i < surplus_query_number_per_rank) ? 1 : 0);

			MPI_Send(&query_start, 1, MPI_INT, i, 10, MPI_COMM_WORLD);
			MPI_Send(&number_of_queries, 1, MPI_INT, i, 11, MPI_COMM_WORLD);
		}

		/*
		 *
		 */
		for (id_query = 0; id_query < rank0_query_number; id_query++)
		{
			/*
			 *  look for the best score of the query sequence.
			 */
			best_score = -1;

			for (id_database = 0; id_database < database_set.num; id_database++)
			{
				score = get_smith_waterman_score(&(query_set.seq[id_query]),
																				 &(database_set.seq[id_database]));
				if (best_score < score)
				{
					best_score = score;
				}
				database_set.seq[id_database].score = score;
			}

			query_set.seq[id_query].score = best_score;

			/*
			 *  show alignment of sequence pairs at the best score.
			 */
			for (id_database = 0; id_database < database_set.num; id_database++)
			{
				show_alignment(&(query_set.seq[id_query]),
											 &(database_set.seq[id_database]));
			}
		}
	}
	else
	{
		int flag;
		int query_start;
		int number_of_queries;

		MPI_Recv(&query_start, 1, MPI_INT, 0, 10, MPI_COMM_WORLD, &status);
		MPI_Recv(&number_of_queries, 1, MPI_INT, 0, 11, MPI_COMM_WORLD, &status);

		/*
		 *
		 */
		for (id_query = query_start; id_query < query_start + number_of_queries; id_query++)
		{
			/*
			 *  look for the best score of the query sequence.
			 */
			best_score = -1;

			for (id_database = 0; id_database < database_set.num; id_database++)
			{
				score = get_smith_waterman_score(&(query_set.seq[id_query]),
																				 &(database_set.seq[id_database]));
				if (best_score < score)
				{
					best_score = score;
				}
				database_set.seq[id_database].score = score;
			}

			query_set.seq[id_query].score = best_score;

			/*
			 *  show alignment of sequence pairs at the best score.
			 */
			for (id_database = 0; id_database < database_set.num; id_database++)
			{
				show_alignment(&(query_set.seq[id_query]),
											 &(database_set.seq[id_database]));
			}
		}
	}

	MPI_Finalize();

	return 0;
}
