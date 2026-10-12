
void __cdecl16far FUN_4c49_05c7(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  
  for (iVar3 = 0; iVar3 < 4; iVar3 = iVar3 + 1) {
    iVar4 = 0;
    while( true ) {
      uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar2 = (int)DAT_451b_002a;
      if (*(int *)(iVar2 + iVar3 * 2 + 0xfc) + -1 <= iVar4) break;
      if (((((*(int *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x104) == 0) ||
            (*(int *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x106) == 0)) ||
           (*(int *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x104) == 0x6f)) ||
          (*(int *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x106) == 0x9b)) &&
         (((*(int *)(iVar2 + iVar3 * 0x50 + (iVar4 + 1) * 4 + 0x104) == 0 ||
           (*(int *)(iVar2 + iVar3 * 0x50 + (iVar4 + 1) * 4 + 0x106) == 0)) ||
          ((*(int *)(iVar2 + iVar3 * 0x50 + (iVar4 + 1) * 4 + 0x104) == 0x6f ||
           (*(int *)(iVar2 + iVar3 * 0x50 + (iVar4 + 1) * 4 + 0x106) == 0x9b)))))) {
        FUN_4c49_0b0e(*(undefined4 *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x104),
                      *(undefined4 *)(iVar2 + iVar3 * 0x50 + (iVar4 + 1) * 4 + 0x104));
      }
      else {
        FUN_4c49_0a0e(*(undefined4 *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x104),
                      *(undefined4 *)(iVar2 + iVar3 * 0x50 + (iVar4 + 1) * 4 + 0x104));
      }
      iVar4 = iVar4 + 1;
    }
    iVar1 = (iVar3 + 1) % 4;
    if ((((*(int *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x104) == 0) ||
         (*(int *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x106) == 0)) ||
        ((*(int *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x104) == 0x6f ||
         (*(int *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x106) == 0x9b)))) &&
       ((((*(int *)(iVar2 + iVar1 * 0x50 + 0x104) == 0 ||
          (*(int *)(iVar2 + iVar1 * 0x50 + 0x106) == 0)) ||
         (*(int *)(iVar2 + iVar1 * 0x50 + 0x104) == 0x6f)) ||
        (*(int *)(iVar2 + iVar1 * 0x50 + 0x106) == 0x9b)))) {
      FUN_4c49_0b0e(*(undefined4 *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x104),
                    *(undefined4 *)(iVar2 + iVar1 * 0x50 + 0x104));
    }
    else {
      FUN_4c49_0a0e(*(undefined4 *)(iVar2 + iVar3 * 0x50 + iVar4 * 4 + 0x104),
                    *(undefined4 *)(iVar2 + iVar1 * 0x50 + 0x104));
    }
  }
  return;
}


