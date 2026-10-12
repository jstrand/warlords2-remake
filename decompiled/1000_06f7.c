
int __cdecl16far
dos_load_driver(undefined2 param_1,undefined2 param_2,undefined2 *param_3,int *param_4)

{
  code *pcVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  bool bVar6;
  long lVar7;
  
  bVar6 = false;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if (!bVar6) {
    bVar6 = false;
    pcVar1 = (code *)swi(0x21);
    lVar7 = (*pcVar1)();
    if (!bVar6) {
      if ((undefined2 *)param_3 != (undefined2 *)0x0) {
        *param_3 = (int)lVar7;
        ((undefined2 *)param_3)[1] = (int)((ulong)lVar7 >> 0x10);
      }
      iVar5 = (int)((ulong)(lVar7 + 0x10) >> 0x10);
      iVar2 = FUN_255e_07c1(lVar7 + 0x10);
      if ((iVar2 != 0) || (iVar5 != 0)) {
        pcVar1 = (code *)swi(0x21);
        (*pcVar1)();
        iVar4 = -0x8000;
        do {
          pcVar1 = (code *)swi(0x21);
          iVar3 = (*pcVar1)();
        } while (iVar3 == iVar4);
        pcVar1 = (code *)swi(0x21);
        (*pcVar1)();
        *param_4 = (iVar2 + 0xfU >> 4) + iVar5;
        return iVar2;
      }
    }
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
  }
  return 0;
}


