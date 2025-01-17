#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define EPSILON (1.0e-6)

struct simplex_t
{
	int m;		/* Constraints. */
	int n;		/* Decision variables. */
	int *var;	/* 0..n - 1 are non basic. */
	double **a; /* A. */
	double *b;	/* B. */
	double *x;	/* x. */
	double *c;	/* c. */
	double y;	/* y. */
};

typedef struct simplex_t simplex_t;
struct node_t{
	int m; /* Constraints. */
	int n; /* Decision variables. */
	int k; /* Parent branches on xk. */
	int h; /* Branch on xh. */
	double xh; /* xh. */
	double ak; /* Parent ak. */
	double bk; /* Parent bk. */
	double *min; /* Lower bounds. */
	double *max; /* Upper bounds. */
	double **a; /* A. */
	double *b; /* b. */
	double *x; /* x. */
	double *c; /* c. */
	double z; /* z. */
};

struct set_t { 
	struct list_t *first;
};

struct list_t{
	struct list_t *next;
	struct node_t *element;
};

int init(simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var);
void prepare(simplex_t *s, int k);
void pivot(simplex_t *s, int row, int col);
int select_nonbasic(simplex_t *s);
int initial(simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var);
double xSimplex(int m, int n, double **a, double *b, double *c, double *x, double y, int *var, int h);
double simplex(int m, int n, double **a, double *b, double *c, double *x, double y);

typedef struct node_t node_t;
typedef struct list_t list_t;
typedef struct set_t set_t;
node_t *initial_node(int m, int n, double **a, double *b, double *c);
node_t *extend(node_t *p, int m, int n, double **a, double *b, double *c, int k, double ak, double bk);
int is_integer(double *xp);
int integer(node_t *p);
void bound(node_t *p, set_t *h, double *zp, double *x);
int branch(node_t *q, double z);
void succ(node_t *p, set_t *h, int m, int n, double **a, double *b, double *c, int k, double ak, double bk, double *zp, double *x);
double intopt(int m, int n, double **a, double *b, double *c, double *x);

node_t *pop(set_t *headRef);
void addNode(set_t *set, node_t *newElement);
void free_node(node_t *q);
int init(simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var)
{

	int i;
	int k;
	s->m = m;
	s->n = n;
	s->a = a;
	s->b = b;
	s->c = c;
	s->x = x;
	s->y = y;
	s->var = var;
	if (s->var == NULL)
	{
		//s->var = calloc(m+n+1);
		s->var = calloc(m + n + 1, sizeof(int));
		
		for ( i = 0; i < m + n; i++)
		 {
		 	s->var[i] = i;
		 }
	}

	for (k = 0, i = 1; i < m; i++)
	{
		if (s->b[i] < s->b[k])
		{
			k = i;
		}
	}

	return k;
}


void pivot(simplex_t *s, int row, int col)
{

	double **a = s->a;
	double *b = s->b;
	double *c = s->c;
	int m = s->m;
	int n = s->n;
	int i, j, t;

	t = s->var[col];
	s->var[col] = s->var[n + row];
	s->var[n + row] = t;
	s->y = s->y + c[col] * b[row] / a[row][col];
	for (i = 0; i < n; i++)
	{
		if (i != col)
		{
			c[i] = c[i] - c[col] * a[row][i] / a[row][col];
		}
	}
	c[col] = -c[col] / a[row][col];
	for (i = 0; i < m; i++)
	{
		if (i != row)
		{
			b[i] = b[i] - a[i][col] * b[row] / a[row][col];
		}
	}
	for (i = 0; i < m; i++)
	{
		if (i != row)
		{
			for (j = 0; j < n; j++)
			{
				if (j != col)
				{
					a[i][j] = a[i][j] - a[i][col] * a[row][j] / a[row][col];
				}
			}
		}
	}

	for (i = 0; i < m; i++)
	{
		if (i != row)
		{
			a[i][col] = -a[i][col] / a[row][col];
		}
	}

	for (i = 0; i < n; i++)
	{

		if (i != col)
		{
			a[row][i] = a[row][i] / a[row][col];
		}
	}


	b[row] = b[row] / a[row][col];
	
	a[row][col] = 1 / a[row][col];

	return;
}

void prepare(simplex_t *s, int k)
{

	int m = s->m;
	int n = s->n;
	int i;

	for (i = m + n; i > n; i--)
	{
		s->var[i] = s->var[i - 1];
	}
	s->var[n] = m + n;
	n++;
	for (i = 0; i < m; i++)
	{
		s->a[i][n - 1] = -1; //någon konstig pil i metaspråket eller pseudokoden
	}

	s->x = calloc(m + n, sizeof(double));
	s->c = calloc(n, sizeof(double));
	s->c[n - 1] = -1;
	s->n = n;
	pivot(s, k, n - 1);

}

int select_nonbasic(simplex_t *s)
{

	int i;
	for (i = 0; i < s->n; i++)
	{

		if (s->c[i] > EPSILON)
		{
			return i;
		}
	}

	return -1;
}

double xSimplex(int m, int n, double **a, double *b, double *c, double *x, double y, int *var, int h)
{
	
	simplex_t s;
	int i, row, col;

	if (!initial(&s, m, n, a, b, c, x, y, var))
	{
		free(s.var);
		return NAN;
	}

	while ((col = select_nonbasic(&s)) >= 0)
	{
		row = -1;
		for (i = 0; i < m; i++)
		{
			if ((a[i][col] > EPSILON) && (row < 0 || (b[i] / a[i][col] < b[row] / a[row][col])))
			{
				row = i;
			}
		}

		if (row < 0)
		{
			free(s.var);
			return INFINITY;
		}
		pivot(&s, row, col);
	}

	if (h == 0)
	{
		for (i = 0; i < n; i++)
		{
			if (s.var[i] < n)
			{
				x[s.var[i]] = 0;
			}
		}
		for (i = 0; i < m; i++)
		{
			if (s.var[n + i] < n)
			{
				x[s.var[n + i]] = s.b[i];
			}
		}
		free(s.var);
	} else {
		for (i = 0; i < n; i++)
		{
			x[i] = 0;
		}
		for (i = n; i < n + m; i++)
		{
			x[i] = s.b[i - n];
		}
	}
	
	return s.y;
}

int initial(simplex_t *s, int m, int n, double **a, double *b, double *c, double *x, double y, int *var)
{

	int i;
	int j;
	int k;
	double w;
	k = init(s, m, n, a, b, c, x, y, var);
	if (b[k] >= 0) // kanske ändra till s->b[k] >= 0
	{
		return 1; // feasible
	}

	prepare(s, k);
	n = s->n;
	s->y = xSimplex(m, n, s->a, s->b, s->c, s->x, 0, s->var, 1);
	for(i = 0; i < m + n; i++){
		if(s->var[i] == m+n-1){
			if(fabs(s->x[i]) > EPSILON){
				free(s->x);
				free(s->c);
				return 0;
			} else {
				break;
			}
		}
	}

	if(i >= n){
		for(j = k = 0; k < n; k++){
			if(fabs(s->a[i-n][k]) > fabs(s->a[i-n][j])){
				j = k;
			}
		}
		pivot(s, i-n, j);
		i = j;
	}

	if(i < n-1){
		k = s->var[i];
		s->var[i] = s->var[n-1];
		s->var[n-1] = k;
		for(k = 0; k < m; k++){
			w = s->a[k][n-1];
			s->a[k][n-1] = s->a[k][i];
			s->a[k][i] = w;
		} 
	} else {
		
	}
	free(s->c);
	s->c = c;
	s->y = y;
	


	for(int k= n-1; k <	n + m - 1; k++){
		s->var[k] = s->var[k+1];
	}
	n = s->n = s->n -1; //kanske ändra till n = --s->n;

	double *t = calloc(n, sizeof(double));

	for(k = 0; k < n; k++){
		for(j = 0; j < n; j++){
			if (k == s->var[j]){
				t[j] += s->c[k];
				goto next_k;
			}
		}
		for(j = 0; j < m; j++){
			if(s->var[n+j] == k){
				break;
			}
		}
		s->y = s->y + s->c[k] * s->b[j];
		for(i = 0; i < n; i++){
			t[i] = t[i] - s->c[k] * s->a[j][i];
		}
		next_k:;
	}
		for (int i = 0; i < n; i++)
	{
		s->c[i] = t[i];
	}
	free(t);
	free(s->x);
	return 1;
} 

double simplex(int m, int n, double **a, double *b, double *c, double *x, double y)
{
	
	return xSimplex(m, n, a, b, c, x, y, NULL, 0);
}

node_t *pop(set_t *headRef){
	list_t *head = headRef->first;
	headRef->first = head->next;
	node_t *ret = head->element;
	free(head);
    return ret;
 }
 
void free_node(node_t *p){
	free(p->min);
	free(p->max);
	for(int i = 0; i < p->m+1; i++){
		free(p->a[i]);
	}
	free(p->a);
	free(p->b);
	free(p->c);
	free(p->x);
	free(p);
}

void addNode(set_t *set, node_t *newElement){
	

	list_t *current = calloc(1, sizeof(list_t));
	current->element = newElement;
	
	if(set->first != NULL){
		current->next = set->first;
	}
	set->first = current;
	

}


node_t *initial_node(int m, int n, double **a, double *b, double *c){
	
	node_t *p = calloc(1, sizeof(node_t));
	int i, j;
	p->a = calloc(m+1, sizeof(double *));
	for(i = 0; i < m+1; i++){
 		p->a[i] = calloc(n+1, sizeof(double));
	}

	for (i = 0; i < m; i++) {
		for(j = 0; j < n; j++) {
			p->a[i][j] = a[i][j];
		}
	}
	p->b = calloc(m+1, sizeof(double));
	p->c = calloc(n+1, sizeof(double));
	p->x = calloc(n+1, sizeof(double));
	p->min = calloc(n, sizeof(double));
	p->max = calloc(n, sizeof(double));
	p->m = m;
	p->n = n;
	memcpy(p->b, b, sizeof(double) * m);
	memcpy(p->c, c, sizeof(double) * n);
	for(i = 0; i < n; i++){
		p->min[i] = -INFINITY;
		p->max[i] = INFINITY;
	}
	
	return p;
}

node_t *extend(node_t *p, int m, int n, double **a, 
				double *b, double *c, int k, double ak, double bk){
	
	node_t *q = calloc(1, sizeof(node_t));
	int i = 0;
	int j = 0;
	q->k = k;
	q->ak = ak;
	q->bk = bk;
	if((ak > 0) && (p->max[k] < INFINITY)){
		q->m = p->m;
	} else if(ak < 0 && p->min[k] > 0){
		q->m = p->m;
	} else {
		q->m = p->m + 1;
	}
	q->n = p->n;
	q->h = -1;
	q->a = calloc(q->m+1, sizeof(double *));
	for(i = 0; i < q->m+1; i++){
		q->a[i] = calloc(q->n+1, sizeof(double));
	}
	q->b = calloc(q->m+1, sizeof(double));
	q->c = calloc(q->n+1, sizeof(double));
	q->x = calloc(q->n+1, sizeof(double));
	q->min = calloc(n, sizeof(double));
	q->max = calloc(n, sizeof(double));
	for(i = 0; i < p->n; i++){
		q->min[i] = p->min[i];
		q->max[i] = p->max[i];
	}
	for(i = 0; i < m; i++){
		for(j = 0; j < n; j++){
			q->a[i][j] = a[i][j];
		}
		q->b[i] = b[i];
	}
	for(i = 0; i < m; i++){
		q->b[i] = b[i];
	}
	for(i = 0; i < n; i++){
		q->c[i] = c[i];
	}
	
	if(ak > 0){
		if(q->max[k] == INFINITY || bk < q->max[k]){
			q->max[k] = bk;
		}
	} else if(q->min[k] == -INFINITY || -bk > q->min[k]){
		q->min[k] = -bk;
	}
	for(i = m, j = 0; j < n; j++){
		if(q->min[j] > -INFINITY){
			q->a[i][j] = -1;
			q->b[i] = -q->min[j];
			i++;
		}
		if(q->max[j] < INFINITY){
			q->a[i][j] = 1;
			q->b[i] = q->max[j];
			i++;
		}
	}
	
	return q;
}

int is_integer(double *xp){
	double x = *xp;
	double r = round(x);
	if(fabs(r-x)< EPSILON){
		*xp = r;
		return 1;
	}else{
		return 0;
	}
}

int integer(node_t *p){
	int i;
	for(i =0; i<p->n; i++){
		if(!is_integer(&p->x[i])){
			return 0;
		}

	}
	return 1;
}

void bound(node_t *p, set_t *h, double *zp, double *x) {

	if(p->z > *zp){
		*zp = p->z;
		memcpy(x, p->x, sizeof(double) * p->n);

		list_t *q, *prev, *next;
		if(h->first == NULL){
			return;
		}

		while(h->first->element->z < p-> z){
			q = h->first;	
			h->first = q->next;
			if(h->first == NULL){
				return;
			}
		}
		prev = h->first;
		q = prev->next;
		while(q != NULL){
			next = q->next;
			if (q->element->z < p->z){
				prev->next = q->next;
				free(q->element->min);
                free(q->element->max);
                free(q->element);
                free(q);
			} else{
				prev = q;
			}
			q = next;
		}
		
	}
	
}

int branch(node_t *q, double z){
	double min, max;
	if(q->z < z){
		return 0;
	}
	for(int h = 0; h < q->n; h++){
		if(!is_integer(&q->x[h])){
			if(q->min[h] == -INFINITY){
				min = 0;
			}else{
				min = q->min[h];
			}
			max = q->max[h];
			if(floor(q->x[h]) < min || ceil(q->x[h]) > max){
				continue;
			}
			q->h = h;
			q->xh = q->x[h];

			for(int i = 0; i < q->m+1; i++){
				free(q->a[i]);
			}
			free(q->a);
			free(q->b);
			free(q->c);
			free(q->x);
			return 1;
		}

	}
	return 0;
	
}

void succ(node_t *p, set_t *h, int m, int n, double **a, double *b, double *c, int k, double ak, double bk, double *zp, double *x){
	
	node_t *q = extend(p, m, n, a, b, c, k, ak, bk);
	if (q == NULL){
		return;
	}
	q->z = simplex(q->m, q->n, q->a, q->b, q->c, q->x, 0);
	if (isfinite(q->z)){
		 if (integer(q)){
		 	bound(q, h, zp, x);
		 } else if (branch (q, *zp)){
			addNode(h, q);
			return;
		 }
		 
	}
	
	free_node(q);

}

double intopt(int m, int n, double **a, double *b, double *c, double *x){

	node_t *p = initial_node(m, n, a, b, c);
	set_t *h = calloc(1, sizeof(set_t));
	h->first = calloc(1, sizeof(list_t));
	h->first->element = p;
	
	double z  = -INFINITY;
	p->z = simplex(p->m, p->n, p->a, p->b, p->c, p->x, 0);
	if( integer(p) || !isfinite(p->z)){
		z = p->z;

		if(integer(p)){
			// x = p->x;
			memcpy(x, p->x, sizeof(double) *p->n);
		}
		free(h->first);
		free(h);
		free_node(p);
 
		return z;
	}
	branch(p, z);
	
	while(h->first != NULL){
		p = pop(h);
		succ(p, h, m, n, a, b, c, p->h, 1, floor(p->xh) , &z, x);
		succ(p, h, m, n, a, b, c, p->h, -1, -ceil(p->xh) , &z, x);
		free(p->min);
		free(p->max);
		free(p);
		// free_node(p);
		
	}
	free(h->first);
	free(h);
	
	if(z == -INFINITY){
		return NAN;
	}else{
		return z;
	}
}





int main(int argc, char **argv)
{
	int m;
	int n;
	double **a;
	double *b;
	double *c;
	double *x;
	double y = 0;


	// for(int i = 0; i < 11; i++){
	// 	global_array[i] = i;
	// }
	// free(global_array);

	scanf("%d %d", &m, &n);

	// c vector
	c = calloc(n, sizeof(double));
	
	for (int i = 0; i < n; i++)
	{
		scanf("%lf", &c[i]);
	}

	// a vector
	a = calloc(m, sizeof(double *));
	for (int i = 0; i < m; i++)
	{
		a[i] = calloc(n + 1, sizeof(double));
	}

	for (int i = 0; i < m; i++)
	{
		for (int k = 0; k < n; k++)
		{
			scanf("%lf", &a[i][k]);
		}
	}

	// b vector
	b = calloc(m, sizeof(double));
	for (int i = 0; i < m; i++)
	{
		scanf("%lf", &b[i]);
	}

	// Print section --------------------------------

	// print c vector
	printf("max z = %5.3lfx_0", c[0]);
	for (int i = 1; i < n; i++)
	{
		printf(" + %5.3lfx_%d", c[i], i);
	}
	printf("\n");
	// \u2264

	// print matrix
	for (int i = 0; i < m; i++)
	{
		printf("%13.3lfx_0 + ", a[i][0]);
		for (int k = 1; k < n; k++)
		{
			printf("%5.3lfx_%d ", a[i][k], k);
		}
		printf("\u2264 %5.3lf", b[i]);
		printf("\n");
	}

	x = calloc(n + 1, sizeof(double));
	y = intopt(m, n, a, b, c, x);

	// printf("leaving simplex");
	// printf("\n");
	printf("Simplex: %lf\n", y);

	// free section -------------->
	for (int i = 0; i < m; i++)
	{
		free(a[i]);
	}
	free(a);
	free(b);
	free(c);
	free(x);
}

