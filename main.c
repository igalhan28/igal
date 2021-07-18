#include "mpc.h"

#ifdef _WIN32

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#include <histedit.h>
#endif


/* Create Esayieration of Possible ideg Types */
enum { IDEG_SAYI, IDEG_HATA , IDEG_ISLEC, IDEG_IXADE};

/* Declare New ideg Struct */
typedef struct {
  int tip;
  long sayi;

  char* sem;
  char* hata;

  int tur;
  struct ideg** hucre;
} ideg;

/* Create a new sayi tip ideg */
ideg ideg_sayi(long x) {
  ideg v;
  v.tip = IDEG_SAYI;
  v.sayi = x;
  return v;
}

/* Create a new error tip ideg */
ideg ideg_err(char* x) {
  ideg v;
  v.tip = IDEG_HATA;
  v.hata = x;
  return v;
}

/* Print an "ideg" */
void ideg_print(ideg v) {
  switch (v.tip) {
    /* In the case the tip is a sayi print it */
    /* Then 'break' out of the switch. */
    case IDEG_SAYI: printf("%li", v.sayi); break;
    
    /* In the case the tip is an error */
    case IDEG_HATA:
      /* Check what tip of error it is and print it */
      printf(v.hata);
    break;
  }
}

/* Print an "ideg" followed by a newline */
void ideg_println(ideg v) { ideg_print(v); putchar('\n'); }

ideg degerle_op(ideg x, char* op, ideg y) {
  
  /* If either value is an error return it */
  if (x.tip == IDEG_HATA) { return x; }
  if (y.tip == IDEG_HATA) { return y; }
  
  /* Otherwise do maths on the sayi values */
  if (strcmp(op, "+") == 0) { return ideg_sayi(x.sayi + y.sayi); }
  if (strcmp(op, "-") == 0) { return ideg_sayi(x.sayi - y.sayi); }
  if (strcmp(op, "*") == 0) { return ideg_sayi(x.sayi * y.sayi); }
  if (strcmp(op, "/") == 0) {
    /* If second operand is zero return error */
    return y.sayi == 0 
      ? ideg_err("Sıfıra bölünemez!") 
      : ideg_sayi(x.sayi / y.sayi);
  }
  
  return ideg_err("Geçersiz İşlem");
}

ideg degerle(mpc_ast_t* t) {
  
  if (strstr(t->tag, "sayi")) {
    /* Check if there is some error in conversion */
    errno = 0;
    long x = strtol(t->contents, NULL, 10);
    return errno != ERANGE ? ideg_sayi(x) : ideg_err("Sayı çok büyük");
  }
  
  char* op = t->children[1]->contents;  
  ideg x = degerle(t->children[2]);
  
  int i = 3;
  while (strstr(t->children[i]->tag, "ifade")) {
    x = degerle_op(x, op, degerle(t->children[i]));
    i++;
  }
  
  return x;  
}

int main(int argc, char** argv) {
  
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
      igal    : /^/ <sayi> <ifade>+ /$/ ;             \
    ",
    Sayi, Islec, Ifade, igal);
  
  puts("igal Version 0.0.0.0.7");
  puts("Çıkmak için ktrl+c\n");
  
  while (1) {
  
    char* giris = readline("igal> ");
    add_history(giris);
    
    mpc_result_t r;
    if (mpc_parse("<stdin>", giris, igal, &r)) {
      ideg sonuc = degerle(r.output);
      ideg_println(sonuc);
      mpc_ast_delete(r.output);
    } else {    
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    
    free(giris);
    
  }
  
  mpc_cleanup(4, Sayi, Islec, Ifade, igal);
  
  return 0;
}



/* Construct a pointer to a new Number ideg */
ideg* ideg_sayi(long x) {
  ideg* v = malloc(sizeof(ideg));
  v->tip = IDEG_SAYI;
  v->sayi = x;
  return v;
}
/* Construct a pointer to a new Error ideg */
ideg* ideg_hata(char* m) {
  ideg* v = malloc(sizeof(ideg));
  v->tip = IDEG_HATA;
  v->hata = malloc(strlen(m) + 1);
  strcpy(v->hata, m);
  return v;
}
/* Construct a pointer to a new Symbol ideg */
ideg* ideg_sem(char* s) {
  ideg* v = malloc(sizeof(ideg));
  v->tip = IDEG_ISLEC;
  v->sem = malloc(strlen(s) + 1);
  strcpy(v->sem, s);
  return v;
}
/* A pointer to a new empty Sexpr ideg */
ideg* ideg_sexpr(void) {
  ideg* v = malloc(sizeof(ideg));
  v->tip = IDEG_IXADE;
  v->tur = 0;
  v->hucre = NULL;
  return v;
}





void ideg_sil(ideg* v){
  switch (v->tip)
  {
  case IDEG_SAYI:break;
  
  case IDEG_HATA:
    free(v->hata);
    break;
  
  case IDEG_ISLEC:
    free(v->sem);
  break;
  case IDEG_IXADE:
    ideg_sil(v->hucre);
  break;
  }
}