/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  // TODO
  do{
    readChar();
  }while(currentChar!=EOF&&charCodes[currentChar]==CHAR_SPACE);
}

void skipComment() {
  // TODO
  do{
    readChar();
    if(currentChar==EOF) {
      error(ERR_ENDOFCOMMENT,lineNo,colNo);
      return;
    }
	if(currentChar=='\n')  {
	readChar();
	return;
	}
	
  }while(1);
  
}

Token* readIdentKeyword(void) {
  // TODO
  Token* token=makeToken(TK_IDENT,lineNo,colNo);
  int count=0;
  char ident[256];
 	int i;

  //Read string
  do{
    if(charCodes[currentChar]==CHAR_DIGIT||charCodes[currentChar]==CHAR_LETTER||currentChar==95){ //95  ascii '_'
      ident[count]=currentChar;
      count++;
    }else break;
    readChar();
  }while(currentChar!=EOF);
  ident[count]='\0';
  //Make token
  //printf("%s",ident);
  if(count>MAX_IDENT_LEN){
    //error(ERR_IDENTTOOLONG,lineNo,colNo);
    for ( i=0;i<MAX_IDENT_LEN;i++){
    	token->string[i] = ident[i];
	}
	token->string[15] ='\0';
	
    //strncat(token->string,ident,MAX_IDENT_LEN);
    //printf("%s",token->string);
    return token;
  } else strcpy(token->string,ident);
  TokenType type=checkKeyword(ident);
  if(type!=TK_NONE){
    token->tokenType=type;
  }
  return token;
}

Token* readNumber(void) {
  // TODO
  Token* token=makeToken(TK_NUMBER,lineNo,colNo);
  char numberString[256];
  //Suppose that numbers are less than 256 digits
  int i=0;
  do{
    if(charCodes[currentChar]==CHAR_DIGIT){
      numberString[i]=currentChar;
      i++;
    }else break;
    readChar();
  }while(currentChar!=EOF);
  numberString[i]='\0';
  char temp[11];
  if(strlen(numberString)>=10) {
	error(ERR_TOOLONGINTEGER,lineNo,colNo);
  strncat(temp,numberString,10);
  }
  else strcpy(temp,numberString);
  int length1=strlen(temp);
  char intMaxS[11];
  itoa(INT_MAX,intMaxS,10);
  int length2=strlen(intMaxS);
  //Compare the numberString with INTMAX
  //Get the number less or equal INTMAX
  if(length1==length2){
    i=0;
    int result=0;
    while(i<length1&&i<length2){
      if(temp[i]>intMaxS[i]){
        result=1;
        break;
      }
      i++;
    }
    if(result==1) temp[length1-1]='\0';
  }
  strcpy(token->string,temp);
  token->value=atoi(temp);
  return token;
}

Token* readConstChar(void) {
  // TODO
  Token* token=makeToken(TK_CHAR,lineNo,colNo);
  readChar();
  if(currentChar==EOF) {
    token=makeToken(TK_EOF,lineNo,colNo);
    error(ERR_INVALIDCHARCONSTANT,lineNo,colNo);
    return token;
  }
  token->string[0]=currentChar;
  token->string[1]='\0';
  if(!isprint(currentChar)) error(ERR_INVALIDSYMBOL,lineNo,colNo);
 
  // '\a'
  if (charCodes[currentChar]==CHAR_BSLASH) {
  		  readChar();
		  if ((charCodes[currentChar]!=CHAR_BSLASH)) token->string[0] = 39;
		  else token->string[0] = 92;
  }
   readChar();
  if(currentChar==EOF){
    token=makeToken(TK_EOF,lineNo,colNo);
    error(ERR_INVALIDCHARCONSTANT,lineNo,colNo);
    return token;
  }
  if(charCodes[currentChar]!=CHAR_SINGLEQUOTE) error(ERR_INVALIDCHARCONSTANT,lineNo,colNo);
  readChar();
  return token;
}


Token* getToken(void) {
  Token *token;
  int ln, cn;
  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LPAR: 
    token=makeToken(SB_LPAR,lineNo,colNo);
    readChar();
    if(currentChar==EOF){
      return token;
    }

    return token;
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS,lineNo,colNo);
    readChar();
    return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES,lineNo,colNo);
    readChar();
    return token;
  case CHAR_SLASH:
    token = makeToken(SB_SLASH,lineNo,colNo);
    readChar();
    if(currentChar==EOF){
      return token;
    }
    if (charCodes[currentChar] == CHAR_SLASH) {
	skipComment(); 
	return getToken();
	}
    return token;
  case CHAR_EQ:
    token = makeToken(SB_EQ,lineNo,colNo);
    readChar();
    return token;
  case CHAR_COMMA:
    token = makeToken(SB_COMMA,lineNo,colNo);
    readChar();
    return token;
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON,lineNo,colNo);
    readChar();
    return token;
  case CHAR_PERIOD:
    token = makeToken(SB_PERIOD,lineNo,colNo);
    readChar();
    return token;
  case CHAR_GT:
    token = makeToken(SB_GT,lineNo,colNo);
    readChar();
    if(currentChar==EOF||charCodes[currentChar]!=CHAR_EQ){
      return token;
    }
    token->tokenType=SB_GE;
    readChar();
    return token;
  case CHAR_LT:
    token = makeToken(SB_LT,lineNo,colNo);
    readChar();
    if(currentChar==EOF||charCodes[currentChar]!=CHAR_EQ){
      return token;
    }
    token->tokenType=SB_LE;
    readChar();
    return token;
  case CHAR_EXCLAIMATION:
    token = makeToken(SB_NEQ,lineNo,colNo);
    readChar();
    if(currentChar==EOF||charCodes[currentChar]!=CHAR_EQ){
      token->tokenType=TK_NONE;
      error(ERR_INVALIDSYMBOL,lineNo,colNo);
      if(currentChar!=EOF) readChar();
      return token;
    }
    readChar();
    return token;
  case CHAR_COLON:
    token = makeToken(SB_COLON,lineNo,colNo);
    readChar();
    if(currentChar==EOF||charCodes[currentChar]!=CHAR_EQ) return token;
    token->tokenType=SB_ASSIGN;
    readChar();
    return token;
  case CHAR_SINGLEQUOTE: return readConstChar();
  case EOF: 
    token = makeToken(TK_EOF,lineNo,colNo);
    return token;
  case CHAR_RPAR:
    token = makeToken(SB_RPAR,lineNo,colNo);
    readChar();
    return token;
   case CHAR_LBRACKET:
   	token = makeToken(TK_NONE,lineNo,colNo);
      token->tokenType=SB_LSEL;
      readChar();
      return token;
    case CHAR_RBRACKET:
    token = makeToken(TK_NONE,lineNo,colNo);
    token->tokenType=SB_RSEL;
    readChar();
    return token;
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

/*int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }

  return 0;
}*/

int main()
{

if (scan("example3.kpl") == IO_ERROR) {
   printf("Can\'t read input file!\n");
      }
return 0;

}


