
void __cdecl16far FUN_4c49_104a(int *param_1)

{
  int iVar1;
  undefined2 uVar2;
  
  for (iVar1 = 0; iVar1 < 8; iVar1 = iVar1 + 1) {
    uVar2 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    if ((*(int *)((int)DAT_451b_002a + iVar1 * 4 + 0xbc) == *param_1) &&
       (((int *)param_1)[1] == *(int *)((int)DAT_451b_002a + iVar1 * 4 + 0xbe))) break;
  }
  iVar1 = dice(1,5,iVar1 + -2);
  *(undefined4 *)param_1 = *(undefined4 *)((int)DAT_451b_002a + (iVar1 % 8) * 4 + 0xbc);
  return;
}


