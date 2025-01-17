#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

struct word{
    int count;
    int len;
    char* word;
};

void print_word(struct word word){
   // printf("count: %d\n", word.count);
    //printf("word length: %d \nword: ", word.len);
    for(int x = 0; x < word.len; x++){
        printf("%c",word.word[x]);
    }
   // printf("\n");
}

void print_words(struct word *words, int word_count){

    for(int j=0; j< word_count; j++){
        print_word(words[j]);
        printf(": count is:   %d\n", words[j].count);
        printf("\n");
    }
}

bool is_prime(int line){
    if(line == 1){
        return 0;
    }
    for(int j = 2; j<line; j++){
        if(line % j== 0){
            return 0;
        }
    }
    return line != 1;
}
int word_is_in(struct word *words, int word_count, char* word){
    //printf("word is in called, word count: %d\n", word_count);
    for(int i=0; i<word_count; i++){
      //  printf("inne i loopen\n");
       // print_word(words[i]);

        if((strcmp(words[i].word, word)) == 0)
        {
       //     printf("it is in\n");
            return i;
        }
    }
    return -1;
}

struct word most_frequent(struct word *words, int word_count){
    struct word mf = words[0];
    int mf_count;
    int nw_count;
    
    for(int i= 1; i < word_count; i++){
        mf_count = mf.count;
        nw_count = words[i].count;
        
        if(mf_count < nw_count || (mf_count == nw_count && strcmp(words[i].word, mf.word) < 0)){
            mf = words[i];
        }
    }
    return mf;
}

int main(void)
{

    bool fin = false;
    int c;
    char* inc_word = (char*)malloc(sizeof(char)*BUFSIZ);
    int i = 0;
    int j;
    int line = 1;
    int word_count = 0;
    struct word *words = malloc(sizeof(struct word));

    //samla bokstäver till slutet på en rad.
    //kopiera dom till en word och ändra count
    //samla words i en lista. 
    //när en rad är klar sök igenom listan för ordet, finns den så öka count
    //finns den inte lägg till
   // printf("start program\n");
    while(!fin){
        c = getchar();
        if(c == EOF){
          //  printf("EOF\n");
            break;
        }

        if (c == '\n')
        {   
            inc_word[i] = '\0';
          //  printf("början på ny line\n");
            int index;
            int prime;
            struct word *temp = malloc(sizeof(struct word));
            temp->count = 1;
            temp->len = i;
            temp->word = malloc((sizeof(char) * i) + 1);
            strncpy(temp->word, inc_word, i);
            temp->word[i] = '\0';
            if((prime = is_prime(line))){
                printf("trying to delete ");
                print_word(*temp);
                printf(": ");
                index = word_is_in(words, word_count, temp->word);
                if((index >=0 ) && (words[index].count)!= 0){
                    words[index].count = 0;
                    printf("deleted\n");
                } else{
                    printf("not found\n");
                }
                free(temp->word);
                free(temp);

            }
            else if ((index = word_is_in(words, word_count, temp->word))>=0)
            {
            //    printf("word count +1, index: %d\n", index);
                if(words[index].count == 0){
                    printf("added ");
                }else {
                    printf("counted ");
                }
                print_word(*temp);
                printf("\n");
                words[index].count += 1;
                free(temp->word);
                free(temp);
            }
            else
            {
               // printf("new word\n");
                word_count++;
                words = realloc(words, sizeof(struct word) * word_count);
                words[word_count - 1] = *temp;
                printf("added ");
                print_word(*temp);
                printf("\n");
    
                free(temp);
            }

            i = 0;
           // printf("new line\n");
            line++;
            continue;
        } 
        
        
        else {
               // printf("fyller på nytt ord\n");
                inc_word[i] = c;
                i++;
            //    printf("c är: %c\n", c);
        }
            

    

    }
 //   printf("print words: \n");
    //print_words(words, word_count);

    struct word mf = most_frequent(words, word_count);
    
    printf("result: ");
    print_word(mf);
    printf(" %d\n", mf.count);

    for(j=0; j<word_count; j++){
        free(words[j].word);
    }

    free(words);
    free(inc_word);
    return 0;
}