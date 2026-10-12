
void __cdecl16far FUN_4fc9_0029(int param_1,int param_2)

{
  int iVar1;
  int iVar2;
  int iVar3;
  undefined2 uVar4;
  int local_6;
  int local_4;
  
  local_4 = dice(1,5,3);
  for (iVar3 = 0; iVar3 < local_4; iVar3 = iVar3 + 1) {
    iVar1 = dice(1,3,0);
    iVar2 = dice(1,8,-1);
    local_6 = param_1 + iVar1 * *(int *)((int)DAT_451b_002a + iVar2 * 4 + 0xbc);
    iVar1 = dice(1,3,0);
    iVar2 = dice(1,8,-1);
    iVar1 = param_2 + iVar1 * *(int *)((int)DAT_451b_002a + iVar2 * 4 + 0xbe);
    FUN_4bed_0470(&local_6);
    uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
    if (*(char *)((int)DAT_451b_002a + iVar1 * 0x70 + local_6 + 0x6120) == '\a') {
      *(undefined1 *)((int)DAT_451b_002a + iVar1 * 0x70 + local_6 + 0x6120) = 8;
      FUN_4bed_03aa(local_6,iVar1);
    }
  }
  return;
}


