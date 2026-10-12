
void * __cdecl16far dos_load_file(char *name,long *size)

{
  code *pcVar1;
  void *pvVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  bool bVar6;
  undefined4 uVar7;
  
  bVar6 = false;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if (!bVar6) {
    bVar6 = false;
    pcVar1 = (code *)swi(0x21);
    uVar7 = (*pcVar1)();
    iVar5 = (int)((ulong)uVar7 >> 0x10);
    if (!bVar6) {
      if ((long *)size != (long *)0x0) {
        *(int *)size = (int)uVar7;
        *(int *)((int)(long *)size + 2) = iVar5;
      }
      pvVar2 = (void *)FUN_255e_07c1(uVar7);
      if ((pvVar2 != (void *)0x0) || (iVar5 != 0)) {
        pcVar1 = (code *)swi(0x21);
        (*pcVar1)();
        iVar4 = -0x8000;
        do {
          pcVar1 = (code *)swi(0x21);
          iVar3 = (*pcVar1)();
        } while (iVar3 == iVar4);
        pcVar1 = (code *)swi(0x21);
        (*pcVar1)();
        return (void *)CONCAT22(iVar5,pvVar2);
      }
    }
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
  }
  return (void *)0x0;
}


