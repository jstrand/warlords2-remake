
int __cdecl16far terrain_near(int x,int y,int type)

{
  int iVar1;
  int iVar2;
  undefined2 uVar3;
  
  iVar1 = 0;
  while( true ) {
    if (7 < iVar1) {
      return 0;
    }
    uVar3 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    iVar2 = (int)DAT_451b_002a + iVar1 * 4;
    if ((((SCARRY2(x,*(int *)(iVar2 + 0xbc)) == x + *(int *)(iVar2 + 0xbc) < 0) &&
         (SCARRY2(y,*(int *)(iVar2 + 0xbe)) == y + *(int *)(iVar2 + 0xbe) < 0)) &&
        (x + *(int *)(iVar2 + 0xbc) < 0x70)) &&
       ((y + *(int *)(iVar2 + 0xbe) < 0x9c &&
        (*(char *)((int)DAT_451b_002a + (y + *(int *)(iVar2 + 0xbe)) * 0x70 + x +
                   *(int *)(iVar2 + 0xbc) + 0x6120) == type)))) break;
    iVar1 = iVar1 + 1;
  }
  return 1;
}


