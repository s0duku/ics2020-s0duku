#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

enum {
  TK_NOTYPE = 256,TK_END, TK_EQ,TK_NEQ,TK_AND,TK_DEREF,TK_ID, TK_NUM,

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces
  {"\\(", '('},         // lbrace
  {"0[xX][a-fA-F0-9]+", TK_NUM},
  {"[0-9]+", TK_NUM},      
  {"\\$[a-zA-Z]+",TK_ID},               // id
  {"/", '/'},         // div
  {"\\*", '*'},         // mul
  {"\\-", '-'},         // minus    
  {"\\+", '+'},         // plus
  {"\\=\\=", TK_EQ},        // equal
  {"\\!\\=", TK_NEQ},        // equal
  {"\\&\\&", TK_AND},        // equal
  {"\\)", ')'},         // rbrace
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

#define TOKEN_STR_MAX_LEN 32
#define TOKEN_BUF_MAX_LEN 32

typedef struct token {
  int type;
  char str[TOKEN_STR_MAX_LEN];
} Token;

static Token tokens[TOKEN_BUF_MAX_LEN] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case '+':
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case '-':
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case '*':
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case '/':
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case '(':
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case ')':
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case TK_EQ:
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case TK_NEQ:
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case TK_AND:
            tokens[nr_token++].type = rules[i].token_type;
            break;
          case TK_ID:
            assert(substr_len < sizeof(tokens[nr_token].str));
            tokens[nr_token].type = rules[i].token_type;
            memcpy(tokens[nr_token].str,substr_start+1,substr_len-1);
            tokens[nr_token++].str[substr_len-1] = 0; //fix bug , the old one is ...[substr_len]
            break;
          case TK_NUM:
            assert(substr_len < sizeof(tokens[nr_token].str));
            tokens[nr_token].type = rules[i].token_type;
            memcpy(tokens[nr_token].str,substr_start,substr_len);
            tokens[nr_token++].str[substr_len] = 0;
            break;
          case TK_NOTYPE:
            break;//fix bug , forget to imp this
          default:
            i = NR_REGEX;break; 
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  assert(nr_token < TOKEN_BUF_MAX_LEN);
  tokens[nr_token++].type = TK_END;
  return true;
}


static bool check_parentheses(uint32_t p,uint32_t q,bool *success){
  //state machine for checking brace
  uint32_t pos = p;
  uint32_t flag = 0;

  if (tokens[pos++].type != '('){
    return false;
  }

  while(pos <= q){
    if(tokens[pos].type == '('){
      flag++;
      pos++;
      while(pos <= q-1){
        if(tokens[pos].type == '(')
          flag++;
        else if(tokens[pos].type == ')')
          flag--;
        pos++;
      }
    }else if(tokens[pos].type == ')'){
      if (pos == q){
        return true;
      }else{
        return false;
      }
    }
    pos++;
  }

  if (flag == 0 && tokens[q].type == ')')
    return true;
  
  *success = false;
  printf("Bad Expression! p: %u, q: %u\n",p,q);
  return false;

}

static uint32_t get_op_pos(uint32_t p,uint32_t q,bool *success){
  uint32_t op_pos = 0;
  uint32_t pos = p;
  uint32_t flag = 0;
  uint32_t op = TK_NOTYPE;
  while (pos <= q ){
    if (tokens[pos].type == '('){
      flag++;
    }else if(tokens[pos].type == ')'){
      flag--;
    }else if(flag != 0){
      ;
    }else {
      if (tokens[pos].type == '+' || tokens[pos].type == '-'){
        if (op == '*' || op == '/' || op == TK_NOTYPE){
          op = tokens[pos].type;
          op_pos = pos;
        }
      }else if (tokens[pos].type == '*' || tokens[pos].type == '/'){
        if (op == TK_DEREF || op == TK_NOTYPE ){
          op = tokens[pos].type;
          op_pos = pos;
        }
      }else if (tokens[pos].type == TK_EQ || tokens[pos].type == TK_NEQ){
        if (op != TK_AND || op != TK_EQ || op != TK_NEQ){
          op = tokens[pos].type;
          op_pos = pos;
        }
      }else if (tokens[pos].type == TK_AND){
        if (op != TK_AND){
          op = tokens[pos].type;
          op_pos = pos;
        }
      }
    }
    pos++;
  }

  if (op == TK_NOTYPE){
    *success = false;
    printf("Bad Expression! p: %u, q: %u\n",p,q);
  }
  return op_pos;
}

static word_t eval(uint32_t p,uint32_t q,bool *success){
  word_t res = 0;
  assert( q < TOKEN_BUF_MAX_LEN);

  // fix bug, about check success, the old one is success == true instead of *success == true
  if (p > q){
    *success = false;
    printf("Bad Expression! p: %u, q: %u\n",p,q);
    return res;
  }

  if (*success == false)
    return res;
  
  if (p == q){
    if(tokens[p].type == TK_NUM){
      res = strtoul(tokens[p].str,NULL,0);
    }else if(tokens[p].type == TK_ID){
      //printf("Reg %s\n",tokens[p].str);
      res = isa_reg_str2val(tokens[p].str,success);
    }else{
      *success = false;
      printf("Bad Expression! p: %u, q: %u\n",p,q);
    }
  }else if(check_parentheses(p,q,success)){
    res = eval(p+1,q-1,success);
  }else if(*success == true){
    if (tokens[p].type == TK_DEREF){
      res = eval(p+1,q,success);
      if (*success == false)
        return res;
      if (res > PMEM_SIZE){
        *success = false;
        printf("Invalid Address!\n");
        return res;
      }
      // fix bug, the reason we doing gaddr-PMEM_BASE beacasue different isa has different address encode
      // this addr provide by user, we need to modify it
      return vaddr_read(res,4);
    }
    uint32_t op_pos = get_op_pos(p,q,success);
    if (*success == false)
      return res;
    uint32_t left = eval(p,op_pos-1,success);
    if (*success == false)
      return res;
    uint32_t right = eval(op_pos+1,q,success);
    if (*success == false)
      return res;
    
    switch(tokens[op_pos].type){
      case '+':
        res = left + right;
        break;
      case '-':
        res = left - right;
        break;
      case '*':
        res = left * right;
        break;
      case '/':
        if (right == 0){
          *success = false;
          printf("Expression div 0!\n");
        }else{
          res = left / right;
        }
        break;
      case TK_EQ:
        res = (left == right);
        break;
      case TK_NEQ:
        res = (left != right);
        break;
      case TK_AND:
        res = (left && right);
        break;
      default:
        *success = false;
        printf("Bad Expression! p: %u, q: %u\n",p,q);
        break;
    }
  }

  return res;
}



word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  *success = true;
  word_t res = 0;
  uint32_t p = 0;
  uint32_t q = TOKEN_BUF_MAX_LEN;

  for(uint32_t i = 0; i < TOKEN_BUF_MAX_LEN;i++){
    if (tokens[i].type == '*' && 
          (i == 0 || tokens[i - 1].type == '(' || tokens[i - 1].type == TK_DEREF || (tokens[i - 1].type != TK_ID 
            && tokens[i - 1].type != TK_NUM && tokens[i - 1].type != ')'))){
      tokens[i].type = TK_DEREF;
    }
    if (tokens[i].type == TK_END){
      // fix bug, add more check, forget about handling whole blank string
      if (i == 0){
        *success = false;
        printf("Empty Expression!\n");
        return res;
      }
      q = i-1;
      break;
    }
  }

  assert(tokens[q+1].type == TK_END);
  assert(p == 0);
  res = eval(p,q,success);
  if(*success == true)
    return res;
  
  return 0;
}