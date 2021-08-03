#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt)
{
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer) + 1);

  strcpy(cpy, buffer);
  cpy[strlen(cpy) - 1] = '\0';
  return cpy;
}

void add_history(char* unused)
{
}

#else
#include <editline/readline.h>
#include <histedit.h>
#endif


/* Create Esayieration of Possible ideg Types */
enum { IDEG_SAYI, IDEG_HATA, IDEG_ISLEC, IDEG_IXADE };

/* Declare New ideg Struct */
typedef struct ideg {
  int tip;
  long sayi;

  char* sem;
  char* hata;

  int tur;
  struct ideg** hucre;
} ideg;


/* Construct a pointer to a new Number ideg */
ideg* ideg_sayi(long x)
{
  ideg* v = malloc(sizeof(ideg));

  v->tip = IDEG_SAYI;
  v->sayi = x;
  return v;
}
/* Construct a pointer to a new Error ideg */
ideg* ideg_hata(char* m)
{
  ideg* v = malloc(sizeof(ideg));

  v->tip = IDEG_HATA;
  v->hata = malloc(strlen(m) + 1);
  strcpy(v->hata, m);
  return v;
}
/* Construct a pointer to a new Symbol ideg */
ideg* ideg_sem(char* s)
{
  ideg* v = malloc(sizeof(ideg));

  v->tip = IDEG_ISLEC;
  v->sem = malloc(strlen(s) + 1);
  strcpy(v->sem, s);
  return v;
}
/* A pointer to a new empty Sexpr ideg */
ideg* ideg_ixade(void)
{
  ideg* v = malloc(sizeof(ideg));
  printf("const x");
  v->tip = IDEG_IXADE;
  v->tur = 0;
  v->hucre = NULL;
  return v;
}
void ideg_print(ideg* v);
ideg* degerle_op(ideg* x, char* op);

void ixade_print(ideg* v, char o, char c)
{
  putchar(o);
  printf("xprint");
  for (int i = 0; i < v->tur; i++)
    {
      ideg_print(v->hucre[i]);
      if (i != (v->tur - 1))
        {
          putchar(' ');
        }
    }
  putchar(c);
}


/* Print an "ideg" */
void ideg_print(ideg* v)
{
  printf("id print");
  switch (v->tip)
    {
    /* In the case the tip is a sayi print it */
    /* Then 'break' out of the switch. */
    case IDEG_SAYI: printf("%li", v->sayi); break;

    /* In the case the tip is an error */
    case IDEG_HATA:
      /* Check what tip of error it is and print it */
      printf("%s",v->hata);

    case IDEG_ISLEC: printf("%s", v->sem);

    case IDEG_IXADE: ixade_print(v, '(', ')');
      break;
    }
}





void ideg_sil(ideg* v)
{
  printf("sil");
  switch (v->tip)
    {
    case IDEG_SAYI: break;

    case IDEG_HATA:
      free(v->hata);
      break;

    case IDEG_ISLEC:
      free(v->sem);
      break;

    case IDEG_IXADE:
      for (int i = 0; i < v->tur; i++)
        {
          ideg_sil(v->hucre[i]);
        }

      free(v->hucre);
      break;
    }
}

ideg* ideg_yukle(ideg* v, ideg* x)
{
  printf("yukle");
  v->tur++;
  v->hucre = realloc(v->hucre, sizeof(ideg*) * v->tur);
  v->hucre[v->tur - 1] = x;
  return v;
}

ideg* oku_say(mpc_ast_t* t)
{
  printf("oku say");
  long x = strtol(t->contents, NULL, 10);

  return errno != ERANGE ? ideg_sayi(x) : ideg_hata("invalid number");
}

ideg* ideg_oku(mpc_ast_t* t)
{
  printf("1");
  if (strstr(t->tag, "sayi"))
    return oku_say(t);
  if (strstr(t->tag, "islec"))
    return ideg_sem(t->contents);

  ideg* x = NULL;

 
  if (strcmp(t->tag, ">") == 0){
     x = ideg_ixade();
  }
  if(strstr(t->tag, "ixade")){
    x = ideg_ixade();
  }


  for (int i = 0; i < t->children_num; i++)
    {
      if (strcmp(t->children[i]->contents, "(") == 0)
        {
          continue;
        }
      if (strcmp(t->children[i]->contents, ")") == 0)
        {
          continue;
        }
      if (strcmp(t->children[i]->contents, "}") == 0)
        {
          continue;
        }
      if (strcmp(t->children[i]->contents, "{") == 0)
        {
          continue;
        }
      if (strcmp(t->children[i]->tag, "regex") == 0)
        {
          continue;
        }
      x = ideg_yukle(x, ideg_oku(t->children[i]));
    }
  return x;
}
ideg* ideg_cıkra(ideg* v, int n)
{
  printf("cıkra");
  ideg* x = v->hucre[n];

  memmove(&v->hucre[n], v->hucre[n + 1], sizeof(ideg*) * (v->tur) - n - 1);

  v->tur--;

  v->hucre = realloc(&v->hucre, sizeof(ideg*) * v->tur);

  return x;
}
ideg* ideg_al(ideg* v, int n)
{
  printf("al");
  ideg* x = ideg_cıkra(v, n);

  ideg_sil(v);
  return x;
}
ideg* ixade_degerle(ideg* v)
{
  for (int i = 0; i < v->tur; i++)
    {
      printf("2");
      v->hucre[i] = ixade_degerle(v->hucre[i]);
    }

  for (int i = 0; i < v->tur; i++)
    {
      if (v->hucre[i]->tip == IDEG_HATA)
        {
          printf("3");
          return ideg_al(v, i);
        }
      ;
    }

  if (v->tur == 0)
    {
      printf("4");
      return v;
    }
  if (v->tur == 1)
    {
      printf("5");
      return ideg_al(v, 0);
    }
  ideg* d = ideg_cıkra(v, 0);

  if (d->tip != IDEG_ISLEC)
    {
      printf("err");
      ideg_sil(d);
      ideg_sil(v);
      return ideg_hata("İxlem işleçsiz başlamayaz");
    }


  ideg* sonuc = degerle_op(v, d->sem);

  return sonuc;
}

ideg* degerle_op(ideg* x, char* op)
{
  printf("degop");
  for (int i = 0; i < x->tur; i++)
    {
      if (x->hucre[i]->tip != IDEG_SAYI)
        {
          return ideg_hata("İşlem yapılacak öge sayı olmak zorunda!");
        }
    }


  /* Otherwise do maths on the sayi values */
  if (strcmp(op, "+") == 0)
    {
      for (int i = 1; i < x->tur; i++)
        {
          x->hucre[0]->sayi += ideg_cıkra(x, i)->sayi;
        }
    }

  if (strcmp(op, "-") == 0)
    {
      for (int i = 1; i < x->tur; i++)
        {
          x->hucre[0]->sayi -= ideg_cıkra(x, i)->sayi;
        }
    }

  if (strcmp(op, "*") == 0)
    {
      for (int i = 1; i < x->tur; i++)
        {
          x->hucre[0]->sayi *= ideg_cıkra(x, i)->sayi;
        }
    }

  if (strcmp(op, "/") == 0)
    for (int i = 1; i < x->tur; i++)
      {
        if (x->hucre[i]->sayi != 0)
          {
            x->hucre[0]->sayi /= ideg_cıkra(x, i)->sayi;
          }
        else
          {
            return ideg_hata("Sıfıra bölünmez!");
          }
      }
}
ideg* degerle(ideg* v)
{
  printf("deg");
  if (v->tip == IDEG_IXADE)
    {
      return(ixade_degerle(v));
    }
  return v;
}
int main(int argc, char** argv)
{
  mpc_parser_t* Sayi = mpc_new("sayi");
  mpc_parser_t* Islec = mpc_new("islec");
  mpc_parser_t* Ifade = mpc_new("ifade");
  mpc_parser_t* Ixade = mpc_new("ixade");
  mpc_parser_t* igal = mpc_new("igal");

  mpca_lang(MPCA_LANG_DEFAULT,
            "                                                     \
      sayi   : /-?[0-9]+/ ;                             \
      islec : '+' | '-' | '*' | '/' ;                  \
      ixade     :'(' <ifade>* ')';  \
      ifade     : <sayi> | <islec> | <ixade> ;  \
      igal    : /^/ <ifade>* /$/ ;             \
    ",
            Sayi, Islec, Ifade, Ixade, igal);

  puts("igal Version 0.0.0.0.7");
  puts("Çıkmak için ktrl+c\n");

  while (1)
    {
      char* giris = readline("igal> ");
      add_history(giris);

      mpc_result_t r;
      if (mpc_parse("<stdin>", giris, igal, &r))
        {
          ideg* sonuc = degerle(ideg_oku(r.output));
          ideg_print(sonuc);
          ideg_sil(sonuc);
          mpc_ast_delete(r.output);
        }
      else
        {
          mpc_err_print(r.error);
          mpc_err_delete(r.error);
        }


      free(giris);
    }

  mpc_cleanup(5, Sayi, Islec, Ifade, Ixade,igal);

  return 0;
}
