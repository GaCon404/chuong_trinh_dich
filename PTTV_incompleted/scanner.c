/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
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
  if(currentChar != EOF) {
    readChar();
  }
}

void skipComment() {
  do {
    readChar();
  } while(currentChar != EOF && charCodes[currentChar] != CHAR_TIMES);

  if(currentChar == EOF) {
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
  }
  readChar();
  if(charCodes[currentChar] == CHAR_RPAR) {
    readChar();
  } else {
    skipComment();
  }
}

Token* readIdentKeyword(void) {
  Token *token = makeToken(TK_NONE, lineNo, colNo);
  int counter = 0;

  do {    
    if(counter < 10) {
      token->string[counter++] = (char)currentChar;
    }
    readChar();
  } while((charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] ==CHAR_DIGIT) && currentChar != EOF);

  token->string[counter] = '\0';
  token->tokenType = checkKeyword(token->string); // kiem tra xem token co phu hop khong
  if(token->tokenType == TK_NONE) { // kiem tra loi (TK_NONE la khong phai key)
    token->tokenType = TK_IDENT;
  }

  return token;
}

Token* readNumber(void) {
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int counter = 0;

  while(charCodes[currentChar] == CHAR_DIGIT) {
    token->string[counter++] = currentChar;
    readChar();
  }
  
  token->string[counter] = '\0';
  token->value = atoi(token->string);

  return token;
}

Token* readConstChar(void) {
  Token *token = makeToken(TK_CHAR, lineNo, colNo);
  readChar();
  //kiem tra ky tu rong
  if(currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
    return token;
  }
  //kiem tra dau ngat cau, dong /n vs /t
  else if(charCodes[currentChar] == CHAR_SLASH) {
    token->string[0] = currentChar;
    readChar();
    if(currentChar != 'n' && currentChar != 't' && charCodes[currentChar] != CHAR_SINGLEQUOTE) {
      error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
    }
    token->string[1] = currentChar;
    token->string[2] = '\0';
    readChar(); 
    readChar();
    return token;
  }
  //check dau '\'
  else if(charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
  }
  else {
    token->string[0] = currentChar; 
    token->string[1] = '\0';


    readChar();
    // het file
    if(currentChar == EOF) {
      token->tokenType = TK_NONE;
      error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);
      return token;
    }
  }

  if(charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    readChar();
    return token;
  } else {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, lineNo, colNo);  
    return token;
  }
}

Token* getToken(void) {
  Token *token;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);
  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();

  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_TIMES:
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SLASH:
    readChar();
    if(charCodes[currentChar] == CHAR_SPACE) {
      token = makeToken(SB_SLASH, lineNo, colNo);
      return token;
    } else if(charCodes[currentChar] == CHAR_SLASH) {
    }
    else {
      error(ERR_INVALIDSYMBOL, lineNo, colNo);
    }
  case CHAR_LPAR:
    token = makeToken(TK_NONE, lineNo, colNo);
    readChar();
    if(charCodes[currentChar] == CHAR_TIMES) {
      skipComment();
      return getToken();
    }
    if(charCodes[currentChar] == CHAR_SPACE) {
      readChar();
      return getToken();
    }
    if(charCodes[currentChar] == CHAR_PERIOD) {
      token->tokenType = SB_LSEL;
      readChar();
      return token;
    } 
    else {
      token->tokenType = SB_LPAR;
      return token;
    }
    break;
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COMMA: 
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SEMICOLON: 
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;   
  case CHAR_PERIOD:
    token = makeToken(SB_PERIOD, lineNo, colNo);
    readChar();
    if(charCodes[currentChar] == CHAR_RPAR && currentChar != EOF) {
      token->tokenType = SB_RSEL;
      readChar();
      return token;
    }
    return token;
  case CHAR_COLON: 
    token = makeToken(SB_COLON, lineNo, colNo);  
    readChar();
    if(charCodes[currentChar] == CHAR_EQ) {
      token->tokenType = SB_ASSIGN;
      readChar();
    }
    return token;
  case CHAR_SINGLEQUOTE: return readConstChar();
  case CHAR_EQ: 
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_EXCLAIMATION: 
    token = makeToken(TK_NONE, lineNo, colNo);
    readChar();
    if(charCodes[currentChar] != CHAR_EQ) {
      error(ERR_INVALIDSYMBOL, lineNo, colNo);
    } else {
      token->tokenType = SB_NEQ;
    }
    return token;
  case CHAR_GT: 
    token = makeToken(SB_GT, lineNo, colNo);
    readChar();
    if(charCodes[currentChar] == CHAR_EQ) {
      token->tokenType = SB_GE;
      readChar();
    }
    return token;
  case CHAR_LT: 
    token = makeToken(SB_LT, lineNo, colNo);
    readChar();
    if(charCodes[currentChar] == CHAR_EQ) {
      token->tokenType = SB_LE;
      readChar();
    } 
    else if(charCodes[currentChar] == CHAR_GT) {
      token->tokenType = SB_NEQ;
    }
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

//ham xuat ra file .txt
void writeToken(Token *token) {
  FILE *outputFile;
  outputFile = fopen("C:\\Users\\trong\\OneDrive\\Documents\\GitHub\\chuong_trinh_dich\\PTTV_incompleted\\save.txt","a+");
  fprintf(outputFile,"%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: fprintf(outputFile,"TK_NONE\n"); break;
  case TK_IDENT: fprintf(outputFile,"TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: fprintf(outputFile,"TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: fprintf(outputFile,"TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: fprintf(outputFile,"TK_EOF\n"); break;

  case KW_PROGRAM: fprintf(outputFile,"KW_PROGRAM\n"); break;
  case KW_CONST: fprintf(outputFile,"KW_CONST\n"); break;
  case KW_TYPE: fprintf(outputFile,"KW_TYPE\n"); break;
  case KW_VAR: fprintf(outputFile,"KW_VAR\n"); break;
  case KW_INTEGER: fprintf(outputFile,"KW_INTEGER\n"); break;
  case KW_CHAR: fprintf(outputFile,"KW_CHAR\n"); break;
  case KW_ARRAY: fprintf(outputFile,"KW_ARRAY\n"); break;
  case KW_OF: fprintf(outputFile,"KW_OF\n"); break;
  case KW_FUNCTION: fprintf(outputFile,"KW_FUNCTION\n"); break;
  case KW_PROCEDURE: fprintf(outputFile,"KW_PROCEDURE\n"); break;
  case KW_BEGIN: fprintf(outputFile,"KW_BEGIN\n"); break;
  case KW_END: fprintf(outputFile,"KW_END\n"); break;
  case KW_CALL: fprintf(outputFile,"KW_CALL\n"); break;
  case KW_IF: fprintf(outputFile,"KW_IF\n"); break;
  case KW_THEN: fprintf(outputFile,"KW_THEN\n"); break;
  case KW_ELSE: fprintf(outputFile,"KW_ELSE\n"); break;
  case KW_WHILE: fprintf(outputFile,"KW_WHILE\n"); break;
  case KW_DO: fprintf(outputFile,"KW_DO\n"); break;
  case KW_FOR: fprintf(outputFile,"KW_FOR\n"); break;
  case KW_TO: fprintf(outputFile,"KW_TO\n"); break;

  case SB_SEMICOLON: fprintf(outputFile,"SB_SEMICOLON\n"); break;
  case SB_COLON: fprintf(outputFile,"SB_COLON\n"); break;
  case SB_PERIOD: fprintf(outputFile,"SB_PERIOD\n"); break;
  case SB_COMMA: fprintf(outputFile,"SB_COMMA\n"); break;
  case SB_ASSIGN: fprintf(outputFile,"SB_ASSIGN\n"); break;
  case SB_EQ: fprintf(outputFile,"SB_EQ\n"); break;
  case SB_NEQ: fprintf(outputFile,"SB_NEQ\n"); break;
  case SB_LT: fprintf(outputFile,"SB_LT\n"); break;
  case SB_LE: fprintf(outputFile,"SB_LE\n"); break;
  case SB_GT: fprintf(outputFile,"SB_GT\n"); break;
  case SB_GE: fprintf(outputFile,"SB_GE\n"); break;
  case SB_PLUS: fprintf(outputFile,"SB_PLUS\n"); break;
  case SB_MINUS: fprintf(outputFile,"SB_MINUS\n"); break;
  case SB_TIMES: fprintf(outputFile,"SB_TIMES\n"); break;
  case SB_SLASH: fprintf(outputFile,"SB_SLASH\n"); break;
  case SB_LPAR: fprintf(outputFile,"SB_LPAR\n"); break;
  case SB_RPAR: fprintf(outputFile,"SB_RPAR\n"); break;
  case SB_LSEL: fprintf(outputFile,"SB_LSEL\n"); break;
  case SB_RSEL: fprintf(outputFile,"SB_RSEL\n"); break;
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
//ham scan sau do ghi vao file
int write_to(char *fileName1)
{
  Token *token;

  if (openInputStream(fileName1) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    writeToken(token);
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

if (write_to("example2.kpl") == IO_ERROR) {
   printf("Can\'t read input file!\n");
      }
return 0;

if (scan("example2.kpl") == IO_ERROR) {
   printf("Can\'t read input file!\n");
      }
return 0;

}