
void __cdecl16far _fputc(char param_1,FILE *param_2)

{
  param_2->_flags = param_2->_flags + -1;
  fputc((int)param_1,(FILE *)param_2);
  return;
}


