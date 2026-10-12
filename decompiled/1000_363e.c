
int __cdecl16near brk(void *__addr)

{
  int iVar1;
  undefined1 in_CF;
  undefined1 in_ZF;
  undefined2 in_stack_00000004;
  
  __N_PCMP();
  if (((!(bool)in_CF) && (__N_PCMP(), (bool)in_CF || (bool)in_ZF)) &&
     (iVar1 = __brk(__addr,in_stack_00000004), iVar1 != 0)) {
    return 0;
  }
  return -1;
}


