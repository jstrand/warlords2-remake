
tm * __cdecl16far getdate(char *__string)

{
  code *pcVar1;
  tm *ptVar2;
  undefined2 in_CX;
  char extraout_DL;
  char extraout_DH;
  undefined2 in_stack_00000006;
  
  pcVar1 = (code *)swi(0x21);
  ptVar2 = (tm *)(*pcVar1)();
  __string[4] = (char)ptVar2;
  *___string = extraout_DL;
  __string[1] = extraout_DH;
  *(undefined2 *)(__string + 2) = in_CX;
  return ptVar2;
}


