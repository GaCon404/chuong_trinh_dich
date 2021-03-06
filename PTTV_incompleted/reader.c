/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include "reader.h"

FILE *inputStream;

int lineNo, colNo;
int currentChar;

int readChar(void) {
  currentChar = getc(inputStream);
  colNo ++;
  if (currentChar == '\n') {
    lineNo ++;
    colNo = 0;
  }
  return currentChar;
}

int dem(void)
{
  int count = 0;
  int curchar = getc(inputStream);
  colNo ++;
  if (curchar == '\n') {
    lineNo ++;
    colNo = 0;
  }
  return currentChar;
}

int openInputStream(char *fileName) {
  inputStream = fopen("C:\\Users\\Admin\\Documents\\GitHub\\chuong_trinh_dich\\PTTV_incompleted\\example2.kpl", "rt");
    if (inputStream == NULL)
    {printf("Khong thay");return IO_ERROR;}
    else printf("Da mo xong");

  lineNo = 1;
  colNo = 0;
  readChar();
  return IO_SUCCESS;
}

void closeInputStream() {
  fclose(inputStream);
}

