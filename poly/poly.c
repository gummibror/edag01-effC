#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "error.h"
#include "poly.h"


struct poly_t
{
    int *coefficients;
    int *exponents;
    int terms;
};

poly_t *new_poly_from_string(const char *str)
{

    poly_t *poly = malloc(sizeof(poly_t));  

    // oklart vilka som behövs över huvud taget.
    // och vilka som behöver initialiseras här.
    int inc_coef;
    int inc_expo;
    int inc_sign;

    int terms = 1;
    poly->terms = 0;
    poly->exponents = malloc(sizeof(int) * terms);
    poly->coefficients = malloc(sizeof(int) * terms);

    int c;

    c = *str++;
    while (true)
    {

        // oklart vilka som behöver startas om här och till vad.
        inc_expo = 0;
        inc_coef = 1;
        inc_sign = 1;

        if (c == '-')
        {
            inc_sign = -1;
            c = *str++;
        }
        else if (c == '+')
        {
            inc_sign = 1;
            c = *str++;
        }
        if (c == ' ')
        {
            c = *str++;
        }

        if (isdigit(c))
        {
            inc_coef = 0;
            // c = *str++;
            while (isdigit(c))
            {
                inc_coef = inc_coef * 10 + c - '0';
                c = *str++;
            }
            // printf("testar koeff: %d\n", inc_coef);
        }

        if (c == 'x')
        {
            c = *str++;
            if (c == '^')
            {
                c = *str++;
                // inc_expo =  c -'0';
                while (isdigit(c))
                {
                    inc_expo = inc_expo * 10 + c - '0';
                    c = *str++;
                }
            }
            else
            {
                inc_expo = 1;
            }
        }
        else
        {
            inc_expo = 0;
        }

        if (c == ' ')
        {
            c = *str++;
        }

        // lösa placera i rätt index,
        // reallocate för dynamisk storlek.

        inc_coef *= inc_sign;
        poly->coefficients[terms - 1] = inc_coef;
        poly->exponents[terms - 1] = inc_expo;
        /*
          printf("coef: %d\n", inc_coef);
          printf("expo: %d\n", inc_expo);
        */

        if (c == '\0')
        {
            break;
        }
        terms++;

        poly->coefficients = realloc(poly->coefficients, sizeof(int) * terms);
        poly->exponents = realloc(poly->exponents, sizeof(int) * terms);
    }
    poly->terms = terms;
    return poly;
}

void free_poly(poly_t *poly)
{
    free(poly->coefficients);
    free(poly->exponents);
    free(poly);
}

poly_t *mul(poly_t *poly1, poly_t *poly2)
{

    //kanske ger problem om pointers som likställs med andra pointers pekar på samma sak

    int terms1, terms2;
    int *coefs1;
    int *coefs2;

    int *expos1;
    int *expos2;
    int i, j;

    terms1 = poly1->terms;
    coefs1 = poly1->coefficients;
    expos1 = poly1->exponents;

    terms2 = poly2->terms;
    coefs2 = poly2->coefficients;
    expos2 = poly2->exponents;

    poly_t *temp = malloc(sizeof(poly_t));
    int e = 0;
    int nt = terms1 * terms2;
    temp->exponents = malloc(sizeof(int) * nt);
    temp->coefficients = malloc(sizeof(int) * nt);

    // kanske en lösning med 2 loopar för att få varje term
    // och sen en tredje för at jämför ny term med redan existerande.
   
    // räknar ut alla nya termer
    for (i = 0; i < terms1; i++)
    {
        for (j = 0; j < terms2; j++)
        {
            temp->coefficients[e] = coefs1[i] * coefs2[j];
            temp->exponents[e] = expos1[i] + expos2[j];
            e++;
        }
    }
    temp->terms=e;
   // printf("polymul efter multi \n");
   // print_poly(temp);

    // kanske en lösning där jag sparar index på de som har blivit "uppätna" så de inte körs igen.
   
    // försök till att sätta samman termer som har samma exponent.
    bool* taken = malloc(sizeof(bool) * nt);

    poly_t* new_pol = malloc(sizeof(poly_t));
    new_pol->exponents = malloc(sizeof(int) * nt);
    new_pol->coefficients = malloc(sizeof(int) * nt);

    e = 0;
    for (i = 0; i < nt; i++)
    {   
        //printf("taken: %d\n", taken[i]);
        if (!taken[i])
        {
            new_pol->exponents[e] = temp->exponents[i];
            new_pol->coefficients[e]= temp->coefficients[i];
           
            taken[i] = true;

            for (j = 0; j < nt; j++) // kanske kan sätta j= i+1 för att göra programmet snabbare
            {
                if (!taken[j])
                {
                    if (new_pol->exponents[e] == temp->exponents[j])
                    {
                        new_pol->coefficients[e] = new_pol->coefficients[e] + temp->coefficients[j];
                        taken[j] = true;
                    }
                }
            }
             e++;
        }
    }

    new_pol->terms = e;
    new_pol->coefficients = realloc(new_pol->coefficients, sizeof(int)*e);
    new_pol->exponents = realloc(new_pol->exponents, sizeof(int)*e);
  //  printf("polymul efter addera termer med varandra: \n");
   // print_poly(new_pol);
    //sortera termerna efter grad på exponenten.
    int keyexpo;
    int keycoef;
    for(i=0; i < e; i++){
        keyexpo = new_pol->exponents[i];
        keycoef = new_pol->coefficients[i];
        j = i - 1;

        while (j>=0 && new_pol->exponents[j] < keyexpo)
        {
            new_pol->exponents[j+1] = new_pol->exponents[j];
            new_pol->coefficients[j+1] = new_pol->coefficients[j];
            j--;
        }

        new_pol->exponents[j+1] = keyexpo;
        new_pol->coefficients[j+1]=keycoef;
        
    }
    free_poly(temp);
    free(taken);

  //  printf("polymul efter sortering: \n");
  //  print_poly(new_pol);
   

    return new_pol;
}

void print_poly(poly_t *poly)
{
    int *coefs = poly->coefficients;
    int *expos = poly->exponents;
    int terms = poly->terms;
    int coef;
    int expo;
    int i;

    for (i = 0; i < terms; i++)
    {
        coef = coefs[i];
        expo = expos[i];

        // printf("%d", coef);
        if (coef < 0)
        {
            printf("- ");
        }
        else if (i != 0)
        {
            printf("+ ");
        }
        if (coef > 1 || expo == 0)
        {
            printf("%d", coef);
        }
        else if (coef < 0)
        {
            printf("%d", coef * -1);
        }
        // potentiellt lägga till en grej för att se om det är sista för att hantera whitespaces.
        if (expo > 1)
        {
            printf("x^%d ", expo);
        }
        else if (expo == 1)
        {
            printf("x ");
        }
    }
    printf("\n");
}
/*
int main(void)
{
    poly_t* p;
    poly_t* q;
    poly_t* z;

    p = new_poly_from_string("x^2 - 7x + 1");
    q = new_poly_from_string("3x + 2");

    print_poly(p);
    print_poly(q);

    z = mul(p, q);

    printf("mul resultat: ");
    print_poly(z);

    free_poly(p);
    free_poly(q);
    free_poly(z);

    poly_t *r;
    poly_t *s;
    poly_t *z2;

    r = new_poly_from_string("x^10000000 + 2");
    s = new_poly_from_string("2x^2 + 3x + 4");

    print_poly(r);
    print_poly(s);

    z2 = mul(r,s);

    printf("mul2 resultat: ");
    print_poly(z2);

    free_poly(r);
    free_poly(s);
    free_poly(z2);
}
*/