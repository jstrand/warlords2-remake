
long * __cdecl16far ftell(long *__return_storage_ptr__,FILE *__stream)

{
  int iVar1;
  long *plVar2;
  long lVar3;
  long lVar4;
  int local_6;
  
  lVar3 = lseek((int)(char)__return_storage_ptr__[1],0,1);
  if (lVar3 == -1) {
    plVar2 = (long *)0xffff;
  }
  else if ((int)*___return_storage_ptr__ < 0) {
    lVar4 = lVar3;
    if ((*(uint *)((char)__return_storage_ptr__[1] * 2 + 0x3cfe) & 0x800) != 0) {
      lVar4 = lseek((int)(char)__return_storage_ptr__[1],0,2);
      if (lVar4 == -1) {
        return (long *)0xffff;
      }
      lVar3 = lseek((int)(char)__return_storage_ptr__[1],lVar3,0);
      if (lVar3 == -1) {
        return (long *)0xffff;
      }
    }
    iVar1 = __ftell_adjust();
    local_6 = (int)lVar4;
    plVar2 = (long *)(local_6 + iVar1);
  }
  else {
    iVar1 = __ftell_adjust();
    plVar2 = (long *)((int)lVar3 - iVar1);
  }
  return plVar2;
}


