
void __cdecl16far FUN_1555_13e4(undefined2 *param_1)

{
  undefined2 uVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  undefined2 uVar5;
  undefined2 local_4;
  
  iVar4 = 0;
  uVar5 = (undefined2)((ulong)param_1 >> 0x10);
  local_4 = *param_1;
  uVar1 = ((undefined2 *)param_1)[1];
  do {
    DAT_3c04_017d = local_4;
    DAT_3c04_017f = uVar1;
    FUN_8611_006c();
    FUN_177b_0020(local_4,uVar1,0xf0001);
    FUN_177b_005f();
    uVar2 = *(undefined4 *)((undefined2 *)param_1 + 6);
    if (*(char *)((int)uVar2 + iVar4) == -1) {
      return;
    }
    iVar4 = iVar4 + 1;
    iVar3 = FUN_1a8b_0b4d(&local_4);
  } while (iVar3 != 0);
  return;
}


